// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Templates/Health/Health.h"
#include "MegaHealth.generated.h"

/**
 * 
 */
UCLASS()
class PK_API AMegaHealth : public AHealth
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AMegaHealth(const FObjectInitializer& ObjectInitializer);
	
	
protected:

	virtual void HideItem(bool bHide) override;
	virtual void SpawnParticles() override;

	UParticleSystem* energy;
	UParticleSystem* menergy;

};
