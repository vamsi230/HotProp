// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PH_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROPHUNT_API APH_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	APH_GameMode();
	
	virtual void BeginPlay() override;
	
	//Timers
	FTimerHandle CountdownHandle;
	FTimerHandle ChooseHunterTimer;
	FTimerHandle CheckPlayersTimer;
	FTimerHandle SwapLockTimer;
	FTimerHandle StartCheckTimer;
	
	//Pawn Classes
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> HunterPawnClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> PropPawnClass;
	
	//Timer Handles
	FTimerHandle ChoosingHunterPlayerTimerHandle;
	FTimerHandle HidePhaseTimerHandle;
	FTimerHandle HuntPhaseTimerHandle;
	FTimerHandle ResultPhaseTimerHandle;
	
	//Timers
	UPROPERTY(EditDefaultsOnly)
	float ChoosingHunterPlayerTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float HidePhaseTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float HuntPhaseTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float ResultPhaseTimer;
	
	//Checks
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APlayerController*> PlayerControllers;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasGameStarted;
	
	//Phases
	UFUNCTION()
	void RetryStartCheck();

	UFUNCTION()
	void StartGame();
	
	UFUNCTION()
	void ChooseHunter();

	UFUNCTION()
	void RetryChooseHunter();
	
	UFUNCTION()
	void HidePhase();
	
	UFUNCTION()
	void HuntPhase();
	
	UFUNCTION()
	void ResultPhase();
	
	UFUNCTION(BlueprintNativeEvent)
	void EndGame();
	
	//Switch Roles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSwapInProgress;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResetSwapFloatCooldown;
	
	UFUNCTION()
	void SwitchRoles(AController* HunterController, AController* PropController);
};
