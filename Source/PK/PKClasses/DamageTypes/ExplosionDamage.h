// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKDamageType.h"
#include "ExplosionDamage.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UExplosionDamage : public UPKDamageType
{
	GENERATED_BODY()
	
public:
	UExplosionDamage(const FObjectInitializer& ObjectInitializer);
	
	
};
