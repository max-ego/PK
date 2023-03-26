// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PKClasses/PKDamageType.h"
#include "GravityDamage.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UGravityDamage : public UPKDamageType
{
	GENERATED_BODY()
	
public:
	UGravityDamage(const FObjectInitializer& ObjectInitializer);
	
	
};
