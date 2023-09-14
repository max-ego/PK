// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GrenadeCombo.generated.h"

UCLASS()
class PK_API AGrenadeCombo : public AActor
{
	GENERATED_BODY()

	class USceneComponent* Scene;

	class UStaticMeshComponent* StaticMeshComponent;
	
public:	
	// Sets default values for this actor's properties
	AGrenadeCombo();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FORCEINLINE class UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComponent; }
	
};
