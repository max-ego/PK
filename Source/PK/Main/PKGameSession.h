// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameSession.h"
#include "PKGameSession.generated.h"

/**
 * 
 */
UCLASS()
class PK_API APKGameSession : public AGameSession
{
	GENERATED_BODY()

	/** Initialize options based on passed in options string */
	virtual void InitOptions(const FString& Options) override;
	
public:

	APKGameSession(const FObjectInitializer& ObjectInitializer);
	//RegisterServer
	
};
