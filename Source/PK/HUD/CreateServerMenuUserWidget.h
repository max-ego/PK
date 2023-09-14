// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "HUD/PKBaseUserWidget.h"
#include "CreateServerMenuUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PK_API UCreateServerMenuUserWidget : public UPKBaseUserWidget
{
	GENERATED_BODY()
	
public:
	UCreateServerMenuUserWidget(const FObjectInitializer& ObjectInitializer);
	
protected:

	virtual void OnWidgetRebuilt() override;

	FSlateColor DefaultColor = FLinearColor(1.f, 0.576f, 0.24f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
	FString AvailableMapText_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
	FString AvailableMapText_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
	FString AvailableMapText_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
	FString AvailableMapText_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
	FString AvailableMapText_4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
	FString AvailableMapText_5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor AvailableMapColor_0 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor AvailableMapColor_1 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor AvailableMapColor_2 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor AvailableMapColor_3 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor AvailableMapColor_4 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor AvailableMapColor_5 = DefaultColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
		FString ServerMapText_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
		FString ServerMapText_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
		FString ServerMapText_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
		FString ServerMapText_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
		FString ServerMapText_4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemText")
		FString ServerMapText_5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ServerMapColor_0 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ServerMapColor_1 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ServerMapColor_2 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ServerMapColor_3 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ServerMapColor_4 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ServerMapColor_5 = DefaultColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ControlColor_0 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ControlColor_1 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ControlColor_2 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ControlColor_3 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ControlColor_4 = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor ControlColor_5 = DefaultColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
	TArray<UButton*> AvailableMapButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
	TArray<UButton*> ControlButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
	TArray<UButton*> ServerMapButtons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
	TArray<UButton*> NavigateButtons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor BackButtonColor = DefaultColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemColor")
		FSlateColor StartButtonColor = DefaultColor;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buttons")*/
	float AvailableMapsSliderValue = 1.f;
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buttons")*/
	float ServerMapsSliderValue = 1.f;
	
	TArray<FSlateColor*> AvailableMapColors;
	TArray<FSlateColor*> ControlColors;
	TArray<FSlateColor*> ServerMapColors;
	TArray<FSlateColor*> NavBtnsColors;
	TArray<FString*> AvailableMaps = {};
	TArray<FString*> ServerMaps = {};

	TArray<FString> AvailableMapsTotal;
	TArray<FString> ServerMapsTotal;

	int32 dummy = -1;
	int32 SelectedAvailableMapIdx = -1;
	int32 SelectedServerMapIdx = -1;
	int32 ShiftAvailable = 0;
	int32 ShiftServer = 0;
	int32 LastPressedButtonID;
	
	TScriptDelegate<FWeakObjectPtr> UpButtonDeligate;
	TScriptDelegate<FWeakObjectPtr> AddButtonDeligate;
	TScriptDelegate<FWeakObjectPtr> AddAllButtonDeligate;
	TScriptDelegate<FWeakObjectPtr> RemoveAllButtonDeligate;
	TScriptDelegate<FWeakObjectPtr> RemoveButtonDeligate;
	TScriptDelegate<FWeakObjectPtr> DownButtonDeligate;

	TScriptDelegate<FWeakObjectPtr> BackButtonDeligate;
	TScriptDelegate<FWeakObjectPtr> StartButtonDeligate;
	
	// double ckick detection
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	bool bButtonReleased = false;

	// WORKAROUND:
	// since NativeTick and Tick_Implementation ain't virtual then receiving tick from Blueprint
	UFUNCTION(BlueprintCallable, Category = "Default")
	void TickEvent(const FGeometry& MyGeometry, float InDeltaTime);

	void Init();
	/*returns true if double click detected*/
	bool UpdateItemColors(TArray<UButton*> Buttons, TArray<FSlateColor*> ItemColors, int32 &SelectedItem, int32 Shift);
	float UpdateMapButtonsText(TArray<FString> TotalMaps, TArray<FString*> &Maps, int32 &Shift);
	bool CheckForDoubleClick();
	float CalcSliderValue(int32 TotalLen, int32 FrameLen, int32 Shift);
	UFUNCTION()
	void ClearTimer();
	
	/*Scroll*/
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void ScrollUpAvailableMaps();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void ScrollDownAvailableMaps();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void ScrollUpServerMaps();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void ScrollDownServerMaps();
	
	/*Sliders*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
	ESlateVisibility AvailableMapsSliderVisibility = ESlateVisibility::Hidden;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buttons")
	ESlateVisibility ServerMapsSliderVisibility = ESlateVisibility::Hidden;

	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void UpdateAvailableMapsSlider(); // Calls SetAvailableMapsSlider
	FORCEINLINE void UpdateAvailableMapsSlider_Implementation(){};
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SetAvailableMapsSlider(USlider* InSlider);

	UFUNCTION(BlueprintNativeEvent, Category = "CustomEvents")
	void UpdateServerMapsSlider(); // Calls SetServerMapsSlider
	FORCEINLINE void UpdateServerMapsSlider_Implementation(){};
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SetServerMapsSlider(USlider* InSlider);
	
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void OnAvailableMapsSliderValueChanged(float NewValue);
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void OnServerMapsSliderValueChanged(float NewValue);

	void UpdateServerMapsList();

	/*Navigation deligates*/
	UFUNCTION()
	void Back();
	UFUNCTION()
	void Start();
	
	/*Maps manipulation deligates*/
	UFUNCTION()
	void Up();
	UFUNCTION()
	void Add();
	UFUNCTION()
	void AddAll();
	UFUNCTION()
	void RemoveAll();
	UFUNCTION()
	void Remove();
	UFUNCTION()
	void Down();

	/*UFUNCTION(BlueprintCallable, Category = "Buttons")
	void AvMapBtnClick0();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void AvMapBtnClick1();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void AvMapBtnClick2();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void AvMapBtnClick3();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void AvMapBtnClick4();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void AvMapBtnClick5();

	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SrvMapBtnClick0();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SrvMapBtnClick1();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SrvMapBtnClick2();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SrvMapBtnClick3();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SrvMapBtnClick4();
	UFUNCTION(BlueprintCallable, Category = "Buttons")
	void SrvMapBtnClick5();*/
};
