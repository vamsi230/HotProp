// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PH_GameState.generated.h"


UENUM(BlueprintType)
enum class EGameState : uint8
{
	GameStart,
	HidePhase,
	HuntPhase,
	ResultPhase
};

UCLASS()
class PROPHUNT_API APH_GameState : public AGameState
{
	GENERATED_BODY()

public:
	
	
	// Countdown
	UPROPERTY(ReplicatedUsing = OnRep_CountdownChanged, EditAnywhere, BlueprintReadWrite)
	int32 CountdownValue = 0;

	FTimerHandle CountdownTimerHandle;

	UFUNCTION()
	void OnRep_CountdownChanged();

	UFUNCTION(BlueprintNativeEvent, Category = "UI")
	void UpdateCountdownUI();
	
	void StartCountdown(int32 Seconds);
	
	void StopCountdown();
	void DecrementCountdown();
	
	UPROPERTY(ReplicatedUsing=OnRep_GameSessionChanged, EditAnywhere, BlueprintReadWrite)
	bool bGameEnded;	
	
	UPROPERTY(ReplicatedUsing = OnRep_GameStateChanged, EditAnywhere, BlueprintReadWrite)
	EGameState CurrentGameState;
	
	UFUNCTION()
	void ChangeGameState(EGameState NewState);
	
	UFUNCTION(BlueprintNativeEvent)
	void UpdateRoleUI();
	
	UFUNCTION(BlueprintNativeEvent)
	void UpdateGamePhaseUI();
	
	UFUNCTION(BlueprintNativeEvent)
	void ShowEndGameUI();
	
	//OnRep_Functions
	UFUNCTION(BlueprintCallable)
	void OnRep_GameStateChanged();
	
	UFUNCTION(BlueprintCallable)
	void OnRep_GameSessionChanged();
	
	
	FTimerHandle GameSessionTimerHandle;
	
	//Functions
	UFUNCTION(BlueprintCallable)
	void GameStartPhase();
	UFUNCTION(BlueprintCallable)
	void HidePhase();
	UFUNCTION(BlueprintCallable)
	void HuntPhase();
	UFUNCTION(BlueprintCallable)
	void ResultPhase();
	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
