// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKProjectile.h"
#include "ExplosionBase.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AExplosionBase : public APKProjectile
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AExplosionBase(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/*virtual void Deactivate_Implementation() override;*/
	
protected:

	void Process();
};
