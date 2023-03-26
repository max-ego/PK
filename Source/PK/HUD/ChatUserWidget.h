// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HUD/PKBaseUserWidget.h"
#include "HUD/PKHUD.h"
#include "Main/NetPlayerController.h"
#include "ChatUserWidget.generated.h"

/**
 * 
 */

UCLASS()
class PK_API UChatUserWidget : public UPKBaseUserWidget
{
	GENERATED_BODY()
	
public:
	UChatUserWidget(const FObjectInitializer& ObjectInitializer);

	FEventReply OnKeyChar_Implementation(FGeometry MyGeometry, FCharacterEvent InCharacterEvent) override;
	FEventReply OnKeyDown_Implementation(FGeometry MyGeometry, FKeyEvent InKeyEvent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString12 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString11 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString10 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString9 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString8 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString7 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString6 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString5 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString4 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString3 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString2 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString1 = FString::FString();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Console")
		FString MsgString0 = FString::FString();
	
	TArray<FString*> MSGs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	FString ConsoleCommandLine = FString::FString("ConsoleCommandLine");	

	void UpdateConsole();
		
	static int32 GetTrimPos(FString Text, UFont* Font);
	static int32 GetTextSize(FString Text, UFont* Font, float Scale);

protected:

	virtual void OnWidgetRebuilt() override;

	APKHUD* Hud;

	float OutHeight = 0;
	float OutWidth = 0;
	float Scale = 1;
	
	UFont* Font;
	void UpdateCommandLine();
	void UpdMessages(FString Text);

	ANetPlayerController* PC;

	void SendMsg(FString &Text);
};
