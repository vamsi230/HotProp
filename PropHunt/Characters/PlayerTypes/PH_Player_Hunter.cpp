// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_Player_Hunter.h"
#include "EnhancedInputComponent.h"
#include "PH_Player_Prop.h"
#include "Core/PH_GameMode.h"
#include "Core/PH_GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Interfaces/PH_Interface_DamageSystem.h"
#include "Props/PH_MorphableObject.h"

// Sets default values
APH_Player_Hunter::APH_Player_Hunter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));
}

// Called when the game starts or when spawned
void APH_Player_Hunter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentMagazineCount = MaxMagazineCount;
	
	CanShoot = false;
	bCanHunt = false;
}

void APH_Player_Hunter::Shoot()
{
	if (!CanShoot) return;
	CanShoot = false;

	FireSingleShot();
	GetWorldTimerManager().SetTimer(
		ShootCooldownHandle,
		this,
		&APH_Player_Hunter::ResetCanShoot,
		FireRate,
		false
	);
}
void APH_Player_Hunter::ResetCanShoot()
{
	CanShoot = true;
}
void APH_Player_Hunter::ResetMagazine()
{
	CurrentMagazineCount = MaxMagazineCount;
	CanShoot = true;
	
}
void APH_Player_Hunter::Reload()
{
	if (GetWorldTimerManager().IsTimerActive(ReloadHandle)) return;
	if (CurrentMagazineCount >= MaxMagazineCount) return;
	Multicast_PlayReloadFx();

	CanShoot = false;
	GetWorldTimerManager().SetTimer(
		ReloadHandle,
		this,
		&APH_Player_Hunter::ResetMagazine,
		ReloadTime,
		false
	);
}


void APH_Player_Hunter::SwapRoles(AController* Old, AController* New)
{
	if (!Old || !New) return;
	APH_GameMode* GM = Cast<APH_GameMode>(UGameplayStatics::GetGameMode(this));
	GM->SwitchRoles(Old, New);
}

void APH_Player_Hunter::DisableHunt()
{
	bCanHunt = false;
	CanShoot = false;
}
void APH_Player_Hunter::EnableHunt()
{
	bCanHunt = true;
	CanShoot = true;
}
void APH_Player_Hunter::Lose()
{
	PlayAnimMontage(DeathAM);
	DisableHunt();
}

void APH_Player_Hunter::FireSingleShot()
{
	if (!HasAuthority()) return;
	
	if (CurrentMagazineCount <= 0)
	{
		CallReload();
		return;
	}
	
	CurrentMagazineCount--;
	Multicast_PlayShootFx();

	FHitResult HitResult;
	if (!GetController()) return;
	//FVector StartLocation = GetMesh()->GetSocketLocation("BulletSpawnLocation");
	FVector CameraLocation;
	FRotator CameraRotation;

	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector End = CameraLocation + (CameraRotation.Vector() * RayDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		End,
		ECC_GameTraceChannel1,
		Params
	);
	
	DrawDebugLine(
		GetWorld(),
		CameraLocation,
		End,
		FColor::Red,
		false,
		1.f,
		0,
		RayThickness
	);

	if (bHit)
	{
		AActor* HitProp = HitResult.GetActor();
		if (!HitProp) return;
		
		if (HitProp->Implements<UPH_Interface_DamageSystem>())
		{
			IPH_Interface_DamageSystem::Execute_TakeDamage(HitProp, ShootDamage);

			if (APH_Player_Prop* HitPropCharacter = Cast<APH_Player_Prop>(HitProp))
			{
				if (HitPropCharacter->CurrentHealth <= 0)
				{
					SwapRoles(GetController(), HitPropCharacter->GetController());
				}
			}
		}
		else if (HitProp->IsA(APH_MorphableObject::StaticClass()))
		{
			IPH_Interface_DamageSystem::Execute_TakeDamage(this, SelfDamage);
		}
	}
}


//MultiCast RPCs
void APH_Player_Hunter::Multicast_PlayReloadFx_Implementation()
{
	PlayAnimMontage(ReloadAM);
	UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	
}
void APH_Player_Hunter::Multicast_PlayShootFx_Implementation()
{
	PlayAnimMontage(ShootAM);
	UGameplayStatics::PlaySoundAtLocation(this, ShootSound, GetActorLocation());
}

void APH_Player_Hunter::TakeDamage_Implementation(int DamageAmount)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth -= DamageAmount;
		CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth); 
	}
	
	if (CurrentHealth <= 0 && HasAuthority())
	{
		APH_GameMode* GM = Cast<APH_GameMode>(UGameplayStatics::GetGameMode(this));
		if (GM)
		{
			GM->ResultPhase();
		}
	}
}

void APH_Player_Hunter::CallShoot()
{
	if (HasAuthority())
	{
		Shoot();
	}
	else
	{
		Server_Shoot();
	}
}
void APH_Player_Hunter::CallReload()
{
	if (HasAuthority())
	{
		Reload();
	}
	else
	{
		Server_Reload();
	}
}

void APH_Player_Hunter::Server_Shoot_Implementation()
{
	Shoot();
}
void APH_Player_Hunter::Server_Reload_Implementation()
{
	Reload();
}

void APH_Player_Hunter::OnRep_HealthChanged()
{
	Super::OnRep_HealthChanged();
}


void APH_Player_Hunter::Move(const FInputActionValue& Value)
{
	if (!bCanHunt) return;
	Super::Move(Value);
}

// Called to bind functionality to input
void APH_Player_Hunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(ShootAction, ETriggerEvent::Started, this, &APH_Player_Hunter::CallShoot);
		EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &APH_Player_Hunter::CallReload);
	}
}

void APH_Player_Hunter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APH_Player_Hunter, bCanHunt);
	DOREPLIFETIME(APH_Player_Hunter, CanShoot);
	DOREPLIFETIME(APH_Player_Hunter, DeathAM);
	DOREPLIFETIME(APH_Player_Hunter, ShootAM);
	DOREPLIFETIME(APH_Player_Hunter, ReloadAM);
	DOREPLIFETIME(APH_Player_Hunter, ShootSound);
	DOREPLIFETIME(APH_Player_Hunter, ReloadSound);
}

