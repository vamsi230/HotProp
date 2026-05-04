#include "PH_GameState.h"
#include "PH_PlayerState.h"
#include "Characters/PlayerTypes/PH_Player_Hunter.h"
#include "Characters/PlayerTypes/PH_Player_Prop.h"
#include "Net/UnrealNetwork.h"

void APH_GameState::OnRep_GameSessionChanged()
{
    if (bGameEnded)
    {
        StopCountdown();
        
    }
}

void APH_GameState::UpdateGamePhaseUI_Implementation()
{
}

void APH_GameState::UpdateRoleUI_Implementation()
{
}

void APH_GameState::OnRep_CountdownChanged()
{
    UpdateCountdownUI();
}

void APH_GameState::UpdateCountdownUI_Implementation()
{
}

void APH_GameState::ShowEndGameUI_Implementation()
{
}

void APH_GameState::OnRep_GameStateChanged()
{
    switch (CurrentGameState)
    {
        case EGameState::GameStart:
            GameStartPhase();
            break;
        case EGameState::HidePhase:
            HidePhase();
            break;
        case EGameState::HuntPhase:
            HuntPhase();
            break;
        case EGameState::ResultPhase:
            ResultPhase();
            break;
        default:
            break;
    }
    UpdateGamePhaseUI();
}

//To Change GameState
void APH_GameState::ChangeGameState(EGameState NewState)
{
    if (CurrentGameState == NewState) return;
    CurrentGameState = NewState;
    OnRep_GameStateChanged();
}

//UI Timer
void APH_GameState::StartCountdown(int32 Seconds)
{
    if (!HasAuthority()) return;
    CountdownValue = Seconds;
    OnRep_CountdownChanged(); // update server UI immediately
    GetWorld()->GetTimerManager().SetTimer(
        CountdownTimerHandle, this,
        &APH_GameState::DecrementCountdown,
        1.0f, true);
}
void APH_GameState::DecrementCountdown()
{
    if (!HasAuthority()) return;
    CountdownValue--;
    OnRep_CountdownChanged(); // manual call on server
    
    if (CountdownValue <= 0)
        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
}
void APH_GameState::StopCountdown()
{
    GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
    CountdownValue = 0;
    OnRep_CountdownChanged();
}

void APH_GameState::GameStartPhase()
{
    bGameEnded = false;
}

void APH_GameState::HidePhase()
{
    if (!HasAuthority()) return;
    bGameEnded = false;
    for (APlayerState* PS : PlayerArray)
    {
        APH_PlayerState* PlayerState = Cast<APH_PlayerState>(PS);
        if (!PlayerState) continue;
        if (PlayerState->PlayerRole == EPlayerRole::Hunter)
        {
            APH_Player_Hunter* HunterPawn = Cast<APH_Player_Hunter>(PlayerState->GetPawn());
            if (!HunterPawn) continue;
            HunterPawn->DisableHunt();
        }
    }
}

void APH_GameState::HuntPhase()
{
    if (!HasAuthority()) return;
    bGameEnded = false;
    for (APlayerState* PS : PlayerArray)
    {
        APH_PlayerState* PlayerState = Cast<APH_PlayerState>(PS);
        if (!PlayerState) continue;
        if (PlayerState->PlayerRole == EPlayerRole::Hunter)
        {
            APH_Player_Hunter* HunterPawn = Cast<APH_Player_Hunter>(PlayerState->GetPawn());
            if (!HunterPawn) continue;
            HunterPawn->EnableHunt();
        }
        else
        {
            APH_Player_Prop* PropPawn = Cast<APH_Player_Prop>(PlayerState->GetPawn());
            if (!PropPawn) continue;
            PropPawn->EnablePingSystem();
        }
    }
}

void APH_GameState::ResultPhase()
{
    ShowEndGameUI();
    if (!HasAuthority()) return;
    
    for (APlayerState* PS : PlayerArray)
    {
        APH_PlayerState* PlayerState = Cast<APH_PlayerState>(PS);
        if (PlayerState->PlayerRole == EPlayerRole::Hunter)
        {
            APH_Player_Hunter* HunterPawn = Cast<APH_Player_Hunter>(PlayerState->GetPawn());
            if (!HunterPawn) continue;
            HunterPawn->Lose();
        }
    }
}

void APH_GameState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(APH_GameState, CurrentGameState);
    DOREPLIFETIME(APH_GameState, bGameEnded);
    DOREPLIFETIME(APH_GameState, CountdownValue);
}