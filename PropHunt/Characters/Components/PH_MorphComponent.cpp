// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_MorphComponent.h"

#include "Camera/CameraComponent.h"
#include "Characters/PH_PlayerController.h"
#include "Characters/PlayerTypes/PH_Player_Prop.h"
#include "Data Assets/PH_DA_Props.h"

UPH_MorphComponent::UPH_MorphComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UPH_MorphComponent::BeginPlay()
{
	Super::BeginPlay();
	bAbilityCooldown = false;
}

void UPH_MorphComponent::ActivateAbility()
{
	if (!GetOwner()->HasAuthority()) return;
	if (bAbilityCooldown) return;
	
	APH_Player_Prop* PropCharacter = Cast<APH_Player_Prop>(GetOwner());
	if (!PropCharacter) return;
	
	UCameraComponent* CameraComp = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (!CameraComp) return;

	FVector Start = CameraComp->GetComponentLocation();
	FVector End = Start + CameraComp->GetForwardVector() * TraceDistance;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	
	DrawDebugLine(GetWorld(),Start,End,bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);
	
	if (bHit && HitResult.GetActor())
	{
		int32 FoundIndex = PropCharacter->FindPropIndexFromActor(HitResult.GetActor());

		if (FoundIndex != INDEX_NONE)
		{
			// Reset Revert timer because we successfully morphed
			GetWorld()->GetTimerManager().ClearTimer(MorphRevertTimer);

			PropCharacter->CurrentPropIndex = FoundIndex;
			float PropHealth = PropCharacter->MasterPropData->PropList[FoundIndex].PropHealth;
			if (!PropCharacter->MasterPropData) return;
			PropCharacter->MaxHealth = PropHealth;
			PropCharacter->CurrentHealth = PropCharacter->MaxHealth;
			
			// Copy the world scale of the hit component
			UPrimitiveComponent* HitComp = HitResult.GetComponent();
			if (HitComp)
			{
				PropCharacter->ReplicatedPropScale = HitComp->GetComponentScale();
			}
			else
			{
				PropCharacter->ReplicatedPropScale = FVector(1.f);
			}

			PropCharacter->bIsMorphed = true;
			
			// Applies mesh and scales capsule directly on the server
			PropCharacter->OnRep_PropChanged();
			
			APH_PlayerController* PC = Cast<APH_PlayerController>(PropCharacter->GetController());
			if (PC)
			{
				PC->Client_ShowMorphNotification();
			}

			bAbilityCooldown = true;
			GetWorld()->GetTimerManager().SetTimer(MorphRevertTimer, this, &UPH_MorphComponent::Revert, MorphTimer, false);
			GetWorld()->GetTimerManager().SetTimer(AbilityCooldownTimer, this, &UPH_MorphComponent::AbilityCooldown, AbilityTimer, false);
			
			OnAbilityUsed.Broadcast();
		}
	}
}

void UPH_MorphComponent::Revert()
{
	APH_Player_Prop* PropCharacter = Cast<APH_Player_Prop>(GetOwner());
	if (!PropCharacter) return;
	
	PropCharacter->CurrentPropIndex = -1;
	PropCharacter->bIsMorphed = false;
	PropCharacter->MaxHealth = PropCharacter->DefaultHealth;
	PropCharacter->CurrentHealth = PropCharacter->MaxHealth;
	PropCharacter->OnRep_PropChanged();
	
	//lock
	PropCharacter->bLocked = false;
	PropCharacter->LockInputFunction();
}

void UPH_MorphComponent::AbilityCooldown()
{
	bAbilityCooldown = false;
}
