// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Templates/Processes/PKProcessBase.h"
#include "Math/Color.h"
#include "BlendColor.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UBlendColor : public UPKProcessBase
{
	GENERATED_BODY()
	
public:
	UBlendColor();

	virtual void Tick(float DeltaTime) override;

public:

	void Init(FLinearColor& Out, FLinearColor B, float BlendTime);

protected:

	FLinearColor* OutColor;
	FLinearColor A;
	FLinearColor B;
	float BlendValue = 0.f;
	float BlendTime = 0.5f; // timeout before BeginDestroyed if no initialization

	void Blend(float blend);
	FDelegateHandle OnWorldCleanupDelegateHandle;
	void RemoveFromRootSet(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void Destroy();	
	
};
