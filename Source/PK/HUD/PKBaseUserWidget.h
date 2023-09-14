// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PKBaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UPKBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPKBaseUserWidget(const FObjectInitializer& ObjectInitializer);

	FEventReply OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent) override;
	FEventReply OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent) override;
	FEventReply OnKeyUp_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents")
	void OnEscape();

protected:

	virtual void OnWidgetRebuilt() override;

	int32 LastKeyCode = -1;
	
	USoundBase* QuitAccept;
	USoundBase* OptionAccept;
	USoundBase* QuitLightOn;

	TSubclassOf<class UUserWidget> ScoreBoardWidget;
};
