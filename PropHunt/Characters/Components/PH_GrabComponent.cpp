// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_GrabComponent.h"

#include "Camera/CameraComponent.h"
#include "PropHunt/Characters/PH_BasePlayer.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Props/PH_MorphableObject.h"


// Sets default values for this component's properties
UPH_GrabComponent::UPH_GrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPH_GrabComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!GetOwner()->HasAuthority()) return;
	if (!bIsGrabbing) return;

	APH_BasePlayer* BaseCharacter = Cast<APH_BasePlayer>(GetOwner());
	if (!BaseCharacter || !BaseCharacter->PhysicsHandle) return;

	if (!BaseCharacter->PhysicsHandle->GetGrabbedComponent()) return;

	UCameraComponent* CameraComp = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (!CameraComp) return;

	
	FVector TargetLocation = CameraComp->GetComponentLocation() + (CameraComp->GetForwardVector() * HoldingDistance);
	FRotator TargetRotation = CameraComp->GetComponentRotation();
	BaseCharacter->PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, TargetRotation);
}


// Called when the game starts
void UPH_GrabComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UPH_GrabComponent::Grab()
{
	if (bIsGrabbing) return;
		
	UCameraComponent* CameraComp = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (!CameraComp) return;
	
	FHitResult HitResult;
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + CameraComp->GetForwardVector() * 1000.f;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	
	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, params);
	DrawDebugLine(GetWorld(),Start,End,Hit ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);
	
	if (Hit && HitResult.GetActor())
	{
		APH_MorphableObject* MorphableObj = Cast<APH_MorphableObject>(HitResult.GetActor());
		if (MorphableObj && MorphableObj->Mesh && MorphableObj->Mesh->IsSimulatingPhysics())
		{
			APH_BasePlayer* BaseCharacter = Cast<APH_BasePlayer>(GetOwner());
			if (!BaseCharacter || !BaseCharacter->PhysicsHandle) return;

			BaseCharacter->PhysicsHandle->GrabComponentAtLocationWithRotation(
				MorphableObj->Mesh,
				SocketName,
				MorphableObj->Mesh->GetComponentLocation(),
				MorphableObj->Mesh->GetComponentRotation()
			);
			
			bIsGrabbing = true;
		}
	}
}

void UPH_GrabComponent::Release()
{
	APH_BasePlayer* BaseCharacter = Cast<APH_BasePlayer>(GetOwner());
	if (!BaseCharacter || !BaseCharacter->PhysicsHandle) return;

	if (BaseCharacter->PhysicsHandle->GetGrabbedComponent())
	{
		UPrimitiveComponent* GrabbedComp = BaseCharacter->PhysicsHandle->GetGrabbedComponent();
		if (!GrabbedComp) return;
		BaseCharacter->PhysicsHandle->ReleaseComponent();
		
		UCameraComponent* CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			GrabbedComp->AddImpulse(CameraComponent->GetForwardVector() * ReleaseForce, NAME_None, true);
		}
		
		bIsGrabbing = false;
	}
}
