// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PickableItem.h"
#include "Health.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AHealth : public APickableItem
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AHealth(const FObjectInitializer& ObjectInitializer);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

protected:

	virtual void HideItem(bool bHide) override;
	virtual void SpawnParticles();

	UAnimationAsset* AnimAsset;

	UParticleSystem* energyhealth;
	UParticleSystem* energyhealth1;

	UParticleSystemComponent* root;
	UParticleSystemComponent* e1;
	UParticleSystemComponent* e2;
	UParticleSystemComponent* e3;
	UParticleSystemComponent* e4;
	UParticleSystemComponent* e5;
	UParticleSystemComponent* e6;
};
