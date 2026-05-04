// Fill out your copyright notice in the Description page of Project Settings.


#include "PH_MorphableObject.h"

#include "PhysicsEngine/PhysicsHandleComponent.h"


// Sets default values
APH_MorphableObject::APH_MorphableObject()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true); 

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	
	Mesh->SetSimulatePhysics(true);
	Mesh->SetIsReplicated(true); // 🟡 optional but safer
}
