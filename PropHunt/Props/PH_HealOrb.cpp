// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_HealOrb.h"

#include "Characters/PH_BasePlayer.h"
#include "Core/PH_PlayerState.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Interfaces/PH_Interface_DamageSystem.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APH_HealOrb::APH_HealOrb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingComponent->RotationRate = FRotator(0.f, 90.f, 0.f);
}

void APH_HealOrb::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	if (!OtherActor->Implements<UPH_Interface_DamageSystem>()) return;
	
	APH_BasePlayer* Player = Cast<APH_BasePlayer>(OtherActor);
	if (!Player) return;
	
	if (IPH_Interface_DamageSystem::Execute_IsFullHealth(Player)) return;
	
	APH_PlayerState* PS = Cast<APH_PlayerState>(Player->GetPlayerState());
	if (!PS || PS->PlayerRole == EPlayerRole::Hunter) return;
	
	
	IPH_Interface_DamageSystem::Execute_Heal(Player, HealAmount);
	bIsDestroyed = true;
	OnRep_Destroyed();
}

// Called when the game starts or when spawned
void APH_HealOrb::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &APH_HealOrb::OnOverlapBegin);
	bIsDestroyed = false;
}

void APH_HealOrb::OnRep_Destroyed()
{
	if (!bIsDestroyed) return;	
	Destroy();
}


void APH_HealOrb::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APH_HealOrb, bIsDestroyed);
}

