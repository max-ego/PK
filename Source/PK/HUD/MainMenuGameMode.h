// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "Main/PKGameMode.h"
#include "Main/Util.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS(Config = GameUserSettings)
class PK_API AMainMenuGameMode : public APKGameMode
{
	GENERATED_BODY()	
	
public:
	AMainMenuGameMode(const FObjectInitializer& ObjectInitializer);

protected:
	
public:
	DEPRECATED(1.0, "SaveServerMaps has been deprecated, use UPKGameInstance::StoreCurrentMaps instead.")
	UFUNCTION(BlueprintCallable, Category = "Config")
	void SaveServerMaps(TArray<FString> Maps);
	
	DEPRECATED(1.0, "GetServerMapsString has been deprecated.")
	UFUNCTION(BlueprintCallable, Category = "Config")
	FString GetServerMapsString();

	virtual void StartPlay() override;

protected:

	/** The widget class we will use as our menu when the game starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> StartingWidgetClass;

	/** Called when the game starts. */
	virtual void BeginPlay() override;

	UPROPERTY(Config)
	FString ServerMaps;

protected:
	DEPRECATED(1.0, "SetGameInstanceCurrentMapsList has been deprecated.")
	void SetGameInstanceCurrentMapsList();
};
