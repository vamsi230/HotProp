// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PH_HealOrb.generated.h"

class URotatingMovementComponent;

UCLASS()
class PROPHUNT_API APH_HealOrb : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APH_HealOrb();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	URotatingMovementComponent* RotatingComponent;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heal Properties")
	int32 HealAmount;
	
	UPROPERTY(ReplicatedUsing = OnRep_Destroyed)
	bool bIsDestroyed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	
	UFUNCTION()
	void OnRep_Destroyed();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
