// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PH_GrabComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROPHUNT_API UPH_GrabComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPH_GrabComponent();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	void Grab();
	void Release();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bIsGrabbing;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FName SocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float HoldingDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ReleaseForce;
};
