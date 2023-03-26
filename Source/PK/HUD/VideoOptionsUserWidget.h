// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HUD/PKBaseUserWidget.h"
#include "VideoOptionsUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UVideoOptionsUserWidget : public UPKBaseUserWidget
{
	GENERATED_BODY()
	
	
public:
	UVideoOptionsUserWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		float Resolution = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		float ViewDistance = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		float Texture = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		float AntiAliasing = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		float Shadow = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		float PostProcess = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings, meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
		float Effects = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings)
		bool bResolution = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings)
		bool bViewDistance = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings)
		bool bTexture = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings)
		bool bAntiAliasing = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings)
		bool bShadow = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings)
		bool bPostProcess = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Settings)
		bool bEffects = false;

	/*Scroll Quality*/
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void ResolutionUp();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void ResolutionDown();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void ViewDistanceUp();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void ViewDistanceDown();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void TextureUp();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void TextureDown();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void AntiAliasingUp();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void AntiAliasingDown();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void ShadowUp();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void ShadowDown();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void PostProcessUp();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void PostProcessDown();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void EffectsUp();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void EffectsDown();
	

	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void OnViewDistanceValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void OnTextureValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void OnAntiAliasingValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void OnShadowValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void OnPostProcessValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable, Category = "Buttons")
		void OnEffectsValueChanged(float NewValue);
	
protected:

	virtual void OnWidgetRebuilt() override;

	FSlateColor DefaultColor = FLinearColor(1.f, 0.576f, 0.24f);

	// controls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
		TArray<UButton*> ControlButtons;
	// control colors
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor BackButtonColor = DefaultColor;
	TArray<FSlateColor*> ControlColors;

	// TODO: move to PKBaseUserWidget
	int32 LastPressedButtonID;
	UFUNCTION(BlueprintCallable, Category = "Default")
	void TickEvent(const FGeometry& MyGeometry, float InDeltaTime);
	void UpdateItemColors(TArray<UButton*> Buttons, TArray<FSlateColor*> ItemColors, int32 &SelectedItem, int32 Shift);
	
	
	APlayerController* PC;
	UGameUserSettings* Settings;

	void ResetPawnAppearance();

	USoundBase* ScrollerMove;
};
