// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/PH_BasePlayer.h"
#include "PH_Player_Hunter.generated.h"

class APH_GameMode;

UCLASS()
class PROPHUNT_API APH_Player_Hunter : public APH_BasePlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APH_Player_Hunter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gun Properties")
	UStaticMeshComponent* GunMesh;
	

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gun Properties")
	int32 MaxMagazineCount;
	
	UFUNCTION()
	void ResetMagazine();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gun Properties")
	int32 CurrentMagazineCount;
	
	UFUNCTION(BlueprintCallable)
	void Lose();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Properties")
	int32 SelfDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Properties")
	int32 ShootDamage;
	//Hide Phase
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Player Details")
	bool bCanHunt = true;
	
	//Swap Roles
	void SwapRoles(AController* Old, AController* New);
	virtual void Move(const FInputActionValue& Value) override;
	
	void EnableHunt();
	void DisableHunt();
	
	//Timers
	FTimerHandle BurstTimerHandle;
	FTimerHandle ShootCooldownTimer;
	FTimerHandle TempHandle;

	
	//Gun Mechanics
	void FireSingleShot();
	void ResetCanShoot();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool CanShoot = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ReloadTime;
	
	//ShootRay Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ray Properties")
	float RayDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ray Properties")
	float RayThickness;
	
	//Animations
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* DeathAM;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Gun Properties")
	UAnimMontage* ShootAM;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Gun Properties")	
	UAnimMontage* ReloadAM;
	
	//Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ReloadAction;
	
	//Interface
	virtual void TakeDamage_Implementation(int DamageAmount) override;

	//Call Functions
	void CallShoot();
	void CallReload();
	
	
	//Multicast RPCs
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayReloadFx();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayShootFx();
	
	
	//SoundFX
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SoundFX")
	USoundBase* ShootSound;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "SoundFX")
	USoundBase* ReloadSound;
	
	//Timers
	FTimerHandle ShootCooldownHandle;
	FTimerHandle ReloadHandle;
	
	//Actions
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Shoot();
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Reload();
	
	
	
	//Server RPCs
	UFUNCTION(Server, Reliable)
	void Server_Shoot();
	
	UFUNCTION(Server, Reliable)
	void Server_Reload();
 
	virtual void OnRep_HealthChanged() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
