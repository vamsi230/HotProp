// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/PH_BasePlayer.h"
#include "PH_Player_Prop.generated.h"

class UPH_DA_Props;
class UPH_GrabComponent;
class UPH_MorphComponent;

UCLASS()
class PROPHUNT_API APH_Player_Prop : public APH_BasePlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APH_Player_Prop();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop Data")
	UPH_DA_Props* MasterPropData;
	
	UPROPERTY(ReplicatedUsing=OnRep_PropChanged)
	int32 CurrentPropIndex;
	
	UPROPERTY(ReplicatedUsing=OnRep_PropChanged)
	FVector ReplicatedPropScale;
	
	UFUNCTION()
	void OnRep_PropChanged();
	void ApplyRevertVisuals();
	
	//Ping System
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ping")
	USoundBase* PingSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ping")
	float PingCooldown;
	
	FTimerHandle PingTimerHandle;
	
	UFUNCTION()
	void PlayPingSound();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayPingSound();
	
	UFUNCTION()
	void EnablePingSystem();
	
	//Interface Functions
	virtual void TakeDamage_Implementation(int DamageAmount) override;
	virtual void Heal_Implementation(int HealAmount) override;
	virtual bool IsFullHealth_Implementation() override;
	
	
	virtual void OnRep_OnHealthChanged();
	//Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MorphAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LockAction;
			
	
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPH_MorphComponent* MorphComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* PropMesh;
	
	//Morph Settings
	
	UFUNCTION()
	void ApplyLockState();
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Morph Settings")
	bool bIsMorphed;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Morph Settings")  
	bool bCanMorph;
	
	UPROPERTY(ReplicatedUsing=OnRep_Locked, EditAnywhere, BlueprintReadWrite, Category = "Morph Settings")
	bool bLocked;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Lock();
	
	//On_Rep Functions

	UFUNCTION()
	void OnRep_Locked();
	
	//Functions
	UFUNCTION(BlueprintCallable)
	void Lock();
	
	virtual void Move(const FInputActionValue& Value) override;
	
	UFUNCTION(BlueprintCallable)
	void TryBecomeProp();
	
	UFUNCTION()
	void HandlePropTrace();
	
	UFUNCTION(Server, Reliable)
	void Server_TryBecomeProp();
	
	UFUNCTION()
	int32 FindPropIndexFromActor(AActor* HitActor);

	
	UFUNCTION(BlueprintCallable)
	void LockInputFunction();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
