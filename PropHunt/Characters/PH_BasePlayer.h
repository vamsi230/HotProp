// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PH_Interface_DamageSystem.h"
#include "PH_BasePlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPH_GrabComponent;
class UPhysicsHandleComponent;

UCLASS(BlueprintType)
class PROPHUNT_API APH_BasePlayer : public ACharacter, public IPH_Interface_DamageSystem
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APH_BasePlayer();
	
	//Delegate
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, int, Current, int, Max);

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Delegates")
	FOnHealthChanged OnHealthChanged;
	
	
	//Health Properties
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 MaxHealth;
	UPROPERTY(ReplicatedUsing=OnRep_HealthChanged, EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 DefaultHealth;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bCanHeal;
	
	//Reset Health on Swap
	UFUNCTION(BlueprintCallable)
	void ResetHealth();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnHealthReset();
	
	UFUNCTION()
	void CallResetHealth();
	
	void CallRandomSpawn();
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RandomSpawner();
	
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* CameraBoom;
	
	//On_Rep
	UFUNCTION()
	virtual void OnRep_HealthChanged();
	//Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* CrouchAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* GrabInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ReleaseAction;
	
	//PlayerState
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	bool bCanJump;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	bool bCanMove;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerState")
	bool bCanLook;
	
	//CrouchState
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CrouchSettings")
	float CapsuleRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CrouchSettings")
	float CapsuleHalfHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CrouchSettings")
	float OriginalCapsuleRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CrouchSettings")
	float OriginalCapsuleHalfHeight;
	
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPH_GrabComponent* GrabComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPhysicsHandleComponent* PhysicsHandle;

	// Runtime only
	UPROPERTY()
	AActor* CurrentInteractable = nullptr;

	UFUNCTION()
	void PlaySwapFx();

	//Server RPCs
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Grab();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Release();
	
	UFUNCTION(Server, Reliable)
	void Server_RandomSpawner();
	
	UFUNCTION(Server, Reliable)
	void Server_ResetHealth();
	
	UFUNCTION(NetMulticast, Unreliable)
	void MultiCast_SwapFx();
	
	//SoundFx
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundFX")
	USoundBase* SpawnSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundFX")
	USoundBase* SwapSound;
	
	//Grab And Throw
	UFUNCTION(BlueprintCallable)
	void GrabInputFunction();
	
	UFUNCTION(BlueprintCallable)
	void ReleaseInputFunction();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	virtual void Move(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void Look(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void StartJump();
	UFUNCTION(BlueprintCallable)
	void StopJump();
	
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
