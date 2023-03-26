// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PKProcessBase.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UPKProcessBase : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
	

public:
	UPKProcessBase();

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

private:
	bool bTickable = false;	
	
};
