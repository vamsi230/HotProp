// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PH_PlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Prop,
	Hunter
};

UCLASS(Blueprintable)
class PROPHUNT_API APH_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	
	void HandleRoleChange();
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerRoleChanged, VisibleAnywhere, BlueprintReadOnly, Category = "Player Details")
	EPlayerRole PlayerRole;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Details")
	TArray<AActor*> HunterSpawnPoints;
	
	UFUNCTION()
	void OnRep_PlayerRoleChanged();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Details")
	bool bGameStarted;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Player Details")
	FString PlayerDisplayName;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bIsReady;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
