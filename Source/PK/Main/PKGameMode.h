// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "Engine.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Util.h"
#include "PKGameMode.generated.h"

UCLASS()
class APKGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	APKGameMode(const FObjectInitializer& ObjectInitializer);

	/** @return GameSession class to use for this game  */
	virtual TSubclassOf<class AGameSession> GetGameSessionClass() const override;

	/** Remove the current menu widget and create a new one from the specified class, if provided. */
	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void ChangeMenuWidget(TSubclassOf<class UUserWidget> NewWidgetClass);

	virtual void AbortMatch() override;

	virtual APlayerController* Login(class UPlayer* NewPlayer, const FString& Portal, const FString& Options, const TSharedPtr<class FUniqueNetId>& UniqueId, FString& ErrorMessage) override;
	
protected:

	virtual void HandleMatchAborted() override;

	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
};



