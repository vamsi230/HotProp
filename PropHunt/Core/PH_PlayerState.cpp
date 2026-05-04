// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_PlayerState.h"
#include "PH_GameState.h"
#include "Net/UnrealNetwork.h"


void APH_PlayerState::OnRep_PlayerRoleChanged()
{
	HandleRoleChange();
}

void APH_PlayerState::HandleRoleChange()
{
	APH_GameState* GS = Cast<APH_GameState>(GetWorld()->GetGameState());
	if (!GS) return;
	GS->UpdateRoleUI();
}

void APH_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APH_PlayerState, PlayerDisplayName);
	DOREPLIFETIME(APH_PlayerState, bIsReady);
	DOREPLIFETIME(APH_PlayerState, PlayerRole);
}

