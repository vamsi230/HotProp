// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_BasePlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationSystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PH_GrabComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


// Sets default values
APH_BasePlayer::APH_BasePlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MaxHealth = 100;
	
	//CameraBoom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	//Follow Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	
	GrabComponent = CreateDefaultSubobject<UPH_GrabComponent>(TEXT("GrabComponent"));
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	
	bReplicates = true;

	bCanMove = true;
	bCanJump = true;
	bCanLook = true;
}

void APH_BasePlayer::Server_ResetHealth_Implementation()
{
	ResetHealth();
}


void APH_BasePlayer::ResetHealth()
{
	CurrentHealth = MaxHealth;
	bCanHeal = true;
	Multicast_OnHealthReset();
}

void APH_BasePlayer::CallResetHealth()
{
	if (HasAuthority())
	{
		ResetHealth();
	}
	else
	{
		Server_ResetHealth();
	}
}

void APH_BasePlayer::Multicast_OnHealthReset_Implementation()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void APH_BasePlayer::CallRandomSpawn()
{
	if (HasAuthority())
	{
		RandomSpawner();
	}
	else
	{
		Server_RandomSpawner();
	}
}

FVector GetRandomNavLocation(UWorld* World, FVector Origin, float Radius)
{
	if (!World) return Origin;

	FNavLocation OutLoc;

	if (auto* NavSys = UNavigationSystemV1::GetCurrent(World))
	{
		if (NavSys->GetRandomReachablePointInRadius(Origin, Radius, OutLoc))
		{
			return OutLoc.Location;
		}
	}

	return Origin;
}

void APH_BasePlayer::RandomSpawner()
{
	SetActorLocation(GetRandomNavLocation(GetWorld(), GetActorLocation(), 2000.f));
}

void APH_BasePlayer::OnRep_HealthChanged()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

// Called when the game starts or when spawned
void APH_BasePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	bCanHeal = true;
	
	//Storing Original Values of Capsule Component
	OriginalCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	OriginalCapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	
	//Player Controller
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	bCanJump = true;
	bCanMove = true;
}


// Called to bind functionality to input
void APH_BasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APH_BasePlayer::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APH_BasePlayer::Look);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APH_BasePlayer::StartJump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APH_BasePlayer::StopJump);
		EnhancedInput->BindAction(GrabInput, ETriggerEvent::Started, this, &APH_BasePlayer::GrabInputFunction);
		EnhancedInput->BindAction(ReleaseAction, ETriggerEvent::Started, this, &APH_BasePlayer::ReleaseInputFunction);
	}
}

void APH_BasePlayer::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APH_BasePlayer, MaxHealth);
	DOREPLIFETIME(APH_BasePlayer, CurrentHealth);
	DOREPLIFETIME(APH_BasePlayer, bCanHeal);
}


void APH_BasePlayer::Move(const FInputActionValue& Value)
{
	if (!bCanMove) return;
	if (!Controller) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementVector.IsNearlyZero()) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection,  MovementVector.X);
}

void APH_BasePlayer::Look(const FInputActionValue& Value)
{
	if (!bCanLook) return;
	const FVector2D LookAxis = Value.Get<FVector2D>();

	if (LookAxis.IsNearlyZero())
		return;

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

void APH_BasePlayer::StartJump()
{
	if (!bCanJump) return;
	Jump();
}

void APH_BasePlayer::StopJump()
{
	StopJumping();
}

void APH_BasePlayer::Server_RandomSpawner_Implementation()
{
	RandomSpawner();
}

void APH_BasePlayer::PlaySwapFx()
{
	UGameplayStatics::PlaySoundAtLocation(this, SwapSound, GetActorLocation());
}

void APH_BasePlayer::MultiCast_SwapFx_Implementation()
{
	PlaySwapFx();
}

//Grab and Throw
void APH_BasePlayer::GrabInputFunction()
{
	if (HasAuthority())
	{
		if (GrabComponent)
		{
			GrabComponent->Grab();
		}
	}
	else
	{
		Server_Grab();
	}
}

void APH_BasePlayer::ReleaseInputFunction()
{
	if (HasAuthority())
	{
		if (GrabComponent)
		{
			GrabComponent->Release();
		}
	}
	else
	{
		Server_Release();
	}
}

//Server RPCs
void APH_BasePlayer::Server_Grab_Implementation()
{
	GrabComponent->Grab();
}

void APH_BasePlayer::Server_Release_Implementation()
{
	GrabComponent->Release();
}



