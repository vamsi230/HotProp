// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PH_MorphComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROPHUNT_API UPH_MorphComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPH_MorphComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void ActivateAbility();
	void Revert();

	UFUNCTION()
	void AbilityCooldown();
	
	UPROPERTY(EditDefaultsOnly)
	float TraceDistance = 1000.f;
	
	
	FTimerHandle AbilityCooldownTimer;
	FTimerHandle MorphRevertTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	float AbilityTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	float MorphTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	bool bAbilityCooldown;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityUsed);
	
	FOnAbilityUsed OnAbilityUsed;
};
