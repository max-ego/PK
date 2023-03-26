// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "Engine/SceneCapture2D.h"
#include "PKLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class PK_API APKLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	APKLevelScriptActor(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
protected:

	TSubclassOf<class AActor> PlayerAppearance;
	TSubclassOf<class ASceneCapture2D> Capture2D;
};
