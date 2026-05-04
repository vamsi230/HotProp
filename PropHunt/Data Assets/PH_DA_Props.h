// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PH_DA_Props.generated.h"

USTRUCT(BlueprintType)
struct FPropData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* PropMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PropScale = FVector(1.f, 1.f, 1.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PropHealth = 50.f;
};

/**
 * Data asset holding all the available props players can morph into.
 */
UCLASS(BlueprintType)
class PROPHUNT_API UPH_DA_Props : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props")
	TArray<FPropData> PropList;
};
