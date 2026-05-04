// Fill out your copyright notice in the Description page of Project Settings.

#include "PH_GameMode.h"
#include "PH_GameState.h"
#include "PH_PlayerState.h"
#include "Characters/PH_PlayerController.h"
#include "Characters/PlayerTypes/PH_Player_Hunter.h"
#include "Characters/PlayerTypes/PH_Player_Prop.h"
#include "GameFramework/GameState.h"

APH_GameMode::APH_GameMode()
{
	DefaultPawnClass = APH_Player_Prop::StaticClass();
	bSwapInProgress = false;
	HasGameStarted = false;
}

void APH_GameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(
		StartCheckTimer,
		this,
		&APH_GameMode::StartGame,
		1.0f,
		false
	);
}

void APH_GameMode::StartGame()
{
	if (HasGameStarted) return;
	APH_GameState* GS = GetGameState<APH_GameState>();
	if (!GS)
	{
		RetryStartCheck();
		return;
	}
	
	HasGameStarted = true;

	GS->ChangeGameState(EGameState::GameStart);
	GS->StartCountdown(ChoosingHunterPlayerTimer);

	GetWorld()->GetTimerManager().SetTimer(
		ChoosingHunterPlayerTimerHandle,
		this,
		&APH_GameMode::ChooseHunter,
		ChoosingHunterPlayerTimer,
		false
	);
}

void APH_GameMode::RetryStartCheck()
{
	GetWorld()->GetTimerManager().SetTimer(
		StartCheckTimer,
		this,
		&APH_GameMode::StartGame,
		0.5f,
		false
	);
}

void APH_GameMode::ChooseHunter()
{
	APH_GameState* GS = GetGameState<APH_GameState>();
	if (!GS)
	{
		RetryChooseHunter();
		return;
	}
	
	TArray<APlayerController*> ValidPlayers;
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (!PS) continue;

		APlayerController* PC = Cast<APlayerController>(PS->GetOwner());
		if (!PC) continue;

		if (!PC->GetPawn()) continue;

		ValidPlayers.Add(PC);
	}

	if (ValidPlayers.Num() == 0)
	{
		RetryChooseHunter();
		return;
	}

	int32 HunterIndex = FMath::RandRange(0, ValidPlayers.Num() - 1);
	APlayerController* HunterPC = ValidPlayers[HunterIndex];

	APawn* OldPawn = HunterPC->GetPawn();
	FTransform SpawnTransform = OldPawn->GetActorTransform();

	if (!HunterPawnClass) return;

	APH_Player_Hunter* HunterPawn = GetWorld()->SpawnActor<APH_Player_Hunter>(HunterPawnClass, SpawnTransform);
	if (!HunterPawn)
	{
		RetryChooseHunter();
		return;
	}
	
	HunterPC->UnPossess();

	OldPawn->Destroy();

	HunterPC->Possess(HunterPawn);

	APH_PlayerState* MyPS = HunterPC->GetPlayerState<APH_PlayerState>();
	if (MyPS)
	{
		MyPS->PlayerRole = EPlayerRole::Hunter;
		MyPS->HandleRoleChange();
	}

	APH_Player_Hunter* HunterPlayer = Cast<APH_Player_Hunter>(HunterPawn);
	if (HunterPlayer)
	{
		HunterPlayer->RandomSpawner();
	}
	
	HidePhase();
}

void APH_GameMode::RetryChooseHunter()
{
	GetWorld()->GetTimerManager().SetTimer(
		ChooseHunterTimer,
		this,
		&APH_GameMode::ChooseHunter,
		0.5f,
		false
	);
}

void APH_GameMode::HidePhase()
{
	APH_GameState* GS = Cast<APH_GameState>(GetWorld()->GetGameState());
	if (!GS) return;

	GS->ChangeGameState(EGameState::HidePhase);
	GS->StartCountdown(HidePhaseTimer);

	GetWorld()->GetTimerManager().SetTimer(HidePhaseTimerHandle, this, &APH_GameMode::HuntPhase, HidePhaseTimer, false);
}

void APH_GameMode::HuntPhase()
{
	APH_GameState* GS = Cast<APH_GameState>(GetWorld()->GetGameState());
	if (!GS) return;

	GS->ChangeGameState(EGameState::HuntPhase);
	GS->StartCountdown(HuntPhaseTimer);

	GetWorld()->GetTimerManager().SetTimer(HuntPhaseTimerHandle, this, &APH_GameMode::ResultPhase, HuntPhaseTimer, false);
}

void APH_GameMode::ResultPhase()
{
	APH_GameState* GS = Cast<APH_GameState>(GetWorld()->GetGameState());
	if (!GS) return;

	GS->ChangeGameState(EGameState::ResultPhase);
	GS->StartCountdown(ResultPhaseTimer);

	GetWorld()->GetTimerManager().SetTimer(ResultPhaseTimerHandle, this, &APH_GameMode::EndGame, ResultPhaseTimer, false);
}

void APH_GameMode::EndGame_Implementation()
{
	APH_GameState* GS = Cast<APH_GameState>(GetWorld()->GetGameState());
	if (!GS) return;

	GS->bGameEnded = true;

	int32 PlayerCount = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			PC->ClientTravel(TEXT("/Game/Maps/Level_MainMenu"), TRAVEL_Absolute);
			PlayerCount++;
		}
	}
}

void APH_GameMode::SwitchRoles(AController* HunterController, AController* PropController)
{
	if (bSwapInProgress) return;
	bSwapInProgress = true;

	APH_PlayerState* HunterPS = HunterController->GetPlayerState<APH_PlayerState>();
	APH_PlayerState* PropPS   = PropController->GetPlayerState<APH_PlayerState>();

	if (!HunterPS) return;
	if (!PropPS) return;

	HunterPS->PlayerRole = EPlayerRole::Prop;
	PropPS->PlayerRole   = EPlayerRole::Hunter;

	HunterPS->HandleRoleChange();
	PropPS->HandleRoleChange();

	APawn* HunterPawn = HunterController->GetPawn();
	APawn* PropPawn   = PropController->GetPawn();

	if (!HunterPawn) return;

	if (!PropPawn) return;

	HunterController->UnPossess();
	PropController->UnPossess();

	PropController->Possess(HunterPawn);
	HunterController->Possess(PropPawn);

	APH_Player_Hunter* HunterPlayer_Fx = Cast<APH_Player_Hunter>(HunterPawn);
	APH_Player_Prop*   PropPlayer_Fx   = Cast<APH_Player_Prop>(PropPawn);

	if (HunterPlayer_Fx)
	{
		HunterPlayer_Fx->MultiCast_SwapFx();
	}

	if (PropPlayer_Fx)
	{
		PropPlayer_Fx->MultiCast_SwapFx();
	}


	APH_PlayerController* OldHunterPC = Cast<APH_PlayerController>(HunterController);
	APH_PlayerController* OldPropPC   = Cast<APH_PlayerController>(PropController);

	if (OldHunterPC)
	{
		OldHunterPC->Client_ShowSwapNotification(false, PropPawn);
	}

	if (OldPropPC)
	{
		OldPropPC->Client_ShowSwapNotification(true, HunterPawn);
	}

	APH_Player_Prop* NewProp = Cast<APH_Player_Prop>(PropPawn);
	if (NewProp)
	{
		NewProp->CallResetHealth();
	}

	APH_Player_Hunter* NewHunter = Cast<APH_Player_Hunter>(HunterPawn);
	if (NewHunter)
	{
		NewHunter->CallResetHealth();
		NewHunter->CallRandomSpawn();
	}

	GetWorld()->GetTimerManager().SetTimer(SwapLockTimer, [this]()
	{
		bSwapInProgress = false;
	}, ResetSwapFloatCooldown, false);
}