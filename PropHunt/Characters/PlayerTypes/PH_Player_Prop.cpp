// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_Player_Prop.h"
#include "EnhancedInputComponent.h"
#include "Characters/Components/PH_MorphComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Data Assets/PH_DA_Props.h"
#include "Props/PH_MorphableObject.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APH_Player_Prop::APH_Player_Prop()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	MorphComponent = CreateDefaultSubobject<UPH_MorphComponent>(TEXT("MorphComponent"));
	
	PropMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropMesh"));
	PropMesh->SetupAttachment(RootComponent); // Re-attach to RootComponent (Capsule) to avoid inheriting Skeletal Mesh offsets
	PropMesh->SetHiddenInGame(true);
	PropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;
	CurrentPropIndex = -1;
	ReplicatedPropScale = FVector(1.f, 1.f, 1.f);
}

void APH_Player_Prop::OnRep_PropChanged()
{
	if (!MasterPropData || !MasterPropData->PropList.IsValidIndex(CurrentPropIndex))
	{
		ApplyRevertVisuals();
		return;
	}

	const FPropData& Data = MasterPropData->PropList[CurrentPropIndex];

	PropMesh->SetStaticMesh(Data.PropMesh);
	PropMesh->SetRelativeScale3D(ReplicatedPropScale);
	
	// Reset offset
	PropMesh->SetRelativeLocation(FVector::ZeroVector);
	PropMesh->SetRelativeRotation(FRotator::ZeroRotator);
	
	PropMesh->SetHiddenInGame(false);
	GetMesh()->SetHiddenInGame(true);

	if (Data.PropMesh)
	{
		FBoxSphereBounds Bounds = Data.PropMesh->GetBounds();
		// Apply scaling to the bounds
		float ScaledX = Bounds.BoxExtent.X * ReplicatedPropScale.X;
		float ScaledY = Bounds.BoxExtent.Y * ReplicatedPropScale.Y;
		float ScaledZ = Bounds.BoxExtent.Z * ReplicatedPropScale.Z;
		
		float NewRadius = FMath::Max(10.f, FMath::Max(ScaledX, ScaledY));
		float NewHalfHeight = FMath::Max(10.f, ScaledZ);
		
		// Move the capsule's origin up so the bottom of the capsule doesn't sink into the floor
		GetCapsuleComponent()->SetCapsuleSize(NewRadius, NewHalfHeight);

		// The pivot of a Static Mesh can be anywhere.
		// To align the bottom of the mesh with the bottom of the capsule (-NewHalfHeight):
		// We calculate the distance from the pivot to the bottom of the mesh bounds, and subtract it.
		float MeshBottomZ = (Bounds.Origin.Z - Bounds.BoxExtent.Z) * ReplicatedPropScale.Z;
		float BottomOffset = -NewHalfHeight - MeshBottomZ;
		
		PropMesh->SetRelativeLocation(FVector(0.f, 0.f, BottomOffset));
	}
}

void APH_Player_Prop::ApplyRevertVisuals()
{
	if (!PropMesh) return;

	PropMesh->SetStaticMesh(nullptr);
	PropMesh->EmptyOverrideMaterials();
	PropMesh->SetHiddenInGame(true);
	GetMesh()->SetHiddenInGame(false);
	
	GetCapsuleComponent()->SetCapsuleSize(OriginalCapsuleRadius, OriginalCapsuleHalfHeight);
}

void APH_Player_Prop::PlayPingSound()
{
	if (PingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PingSound, GetActorLocation());
	}
}

void APH_Player_Prop::Multicast_PlayPingSound_Implementation()
{
	PlayPingSound();
}

// Called when the game starts or when spawned
void APH_Player_Prop::BeginPlay()
{
	Super::BeginPlay();
	bCanMorph = true;
	bIsMorphed = false;
	bLocked = false;
}

// Called to bind functionality to input
void APH_Player_Prop::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MorphAction, ETriggerEvent::Started, this, &APH_Player_Prop::TryBecomeProp);
		EnhancedInput->BindAction(LockAction, ETriggerEvent::Started, this, &APH_Player_Prop::LockInputFunction);
		
		
	}
}

//Replication of variables
void APH_Player_Prop::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APH_Player_Prop, bIsMorphed);
	DOREPLIFETIME(APH_Player_Prop, bCanMorph);
	DOREPLIFETIME(APH_Player_Prop, bLocked);
	DOREPLIFETIME(APH_Player_Prop, PingSound);
	DOREPLIFETIME(APH_Player_Prop, CurrentPropIndex);
	DOREPLIFETIME(APH_Player_Prop, ReplicatedPropScale);
}

//Setting The variable bLocked
void APH_Player_Prop::Move(const FInputActionValue& Value)
{
	if (bLocked) return;
	Super::Move(Value);
}
void APH_Player_Prop::TryBecomeProp()
{
	if (HasAuthority())
	{
		HandlePropTrace();
	}
	else
	{
		Server_TryBecomeProp();
	}
}

void APH_Player_Prop::HandlePropTrace()
{
	if (MorphComponent)
	{
		MorphComponent->ActivateAbility();
	}
}

int32 APH_Player_Prop::FindPropIndexFromActor(AActor* Actor)
{
	if (!MasterPropData) return INDEX_NONE;
	
	APH_MorphableObject* MorphableObj = Cast<APH_MorphableObject>(Actor);
	if (!MorphableObj) return INDEX_NONE;

	if (MasterPropData->PropList.IsValidIndex(MorphableObj->PropId))
	{
		return MorphableObj->PropId;
	}

	return INDEX_NONE;
}


void APH_Player_Prop::EnablePingSystem()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(PingTimerHandle, this, &APH_Player_Prop::Multicast_PlayPingSound, PingCooldown, true);
	}
}

void APH_Player_Prop::TakeDamage_Implementation(int DamageAmount)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth -= DamageAmount;
		CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
		OnRep_HealthChanged();
	}
	else
	{
		bCanHeal = false;
	}
}

void APH_Player_Prop::Heal_Implementation(int HealAmount)
{
	if (CurrentHealth == MaxHealth || !bCanHeal) return;
	CurrentHealth += HealAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
	OnRep_HealthChanged();
}

bool APH_Player_Prop::IsFullHealth_Implementation()
{
	return CurrentHealth >= MaxHealth;
}

void APH_Player_Prop::OnRep_OnHealthChanged()
{
	Super::OnRep_HealthChanged();
}

void APH_Player_Prop::Server_TryBecomeProp_Implementation()
{
	HandlePropTrace();
}

//LockState
void APH_Player_Prop::LockInputFunction()
{
	if (HasAuthority())
	{
		Lock();
	}
	else
	{
		Server_Lock();
	}
}

void APH_Player_Prop::ApplyLockState()
{
	if (bLocked)
	{
		bCanMove = false;
		bCanJump = false;
		bCanLook = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		bCanMove = true;
		bCanJump = true;
		bCanLook = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void APH_Player_Prop::Lock()
{
	if (!bIsMorphed)
	{
		bLocked = false;
		ApplyLockState();
	}
	else
	{
		bLocked = !bLocked;
		ApplyLockState();
	}
}

//On_Rep Functions
void APH_Player_Prop::OnRep_Locked()
{
	ApplyLockState();
}

void APH_Player_Prop::Server_Lock_Implementation()
{
	Lock();
}
