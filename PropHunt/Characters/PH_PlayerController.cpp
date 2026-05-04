// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_PlayerController.h"

void APH_PlayerController::Client_ShowSwapNotification_Implementation(bool bIsNowHunter, APawn* NewPawn)
{
	SwapNotification(bIsNowHunter, NewPawn);
}
void APH_PlayerController::SwapNotification_Implementation(bool bIsHunter, APawn* NewPawn)
{
}



void APH_PlayerController::Client_ShowMorphNotification_Implementation()
{
	MorphNotification();
}
void APH_PlayerController::MorphNotification_Implementation()
{
}

void APH_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	BindHUDToPawn(InPawn);
}
