// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "HUD/PKBaseUserWidget.h"
#include "PlayerSettingsMenuUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UPlayerSettingsMenuUserWidget : public UPKBaseUserWidget
{
	GENERATED_BODY()
	
public:
	UPlayerSettingsMenuUserWidget(const FObjectInitializer& ObjectInitializer);

	FEventReply OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent) override;
	FEventReply OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FString PlayerNickName;
	
protected:
	
	virtual void OnWidgetRebuilt() override;

	void GetNickName();
	void SetNickName();
};
