// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PH_MorphableObject.generated.h"


class UPhysicsHandleComponent;
class UWidgetComponent;
class APH_BasePlayer;
class UStaticMeshComponent;
class USphereComponent;
class UPH_DA_Props;

UCLASS()
class PROPHUNT_API APH_MorphableObject : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APH_MorphableObject();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPhysicsHandleComponent* PhysicsHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, Category="Mesh")
	USphereComponent* SphereComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	int32 PropId;
};
