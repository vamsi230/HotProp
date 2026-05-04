// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PH_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROPHUNT_API APH_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(Client, Reliable)
	void Client_ShowSwapNotification(bool bIsNowHunter, APawn* NewPawn);
	
	UFUNCTION(Client, Reliable)
	void Client_ShowMorphNotification();
	

	UFUNCTION(BlueprintImplementableEvent)
	void BindHUDToPawn(APawn* NewPawn);
	
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void SwapNotification(bool bIsHunter, APawn* NewPawn);
	
	UFUNCTION(BlueprintNativeEvent)
	void MorphNotification();

	
};
