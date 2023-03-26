// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "CreateServerMenuUserWidget.h"
#include "Main/PKGameInstance.h"
#include "Main/Util.h"


UCreateServerMenuUserWidget::UCreateServerMenuUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	FSlateColor* AMColors[] = {
		&AvailableMapColor_0,
		&AvailableMapColor_1,
		&AvailableMapColor_2,
		&AvailableMapColor_3,
		&AvailableMapColor_4,
		&AvailableMapColor_5,		
	};
	AvailableMapColors.Append(AMColors, ARRAY_COUNT(AMColors));

	FSlateColor* SMColors[] = {
		&ServerMapColor_0,
		&ServerMapColor_1,
		&ServerMapColor_2,
		&ServerMapColor_3,
		&ServerMapColor_4,
		&ServerMapColor_5
	};
	ServerMapColors.Append(SMColors, ARRAY_COUNT(SMColors));

	FSlateColor* CtrlColors[] = {
		&ControlColor_0,
		&ControlColor_1,
		&ControlColor_2,
		&ControlColor_3,
		&ControlColor_4,
		&ControlColor_5
	};
	ControlColors.Append(CtrlColors, ARRAY_COUNT(CtrlColors));

	FSlateColor* NavBtnsClrs[] = {
		&BackButtonColor,
		&StartButtonColor
	};
	NavBtnsColors.Append(NavBtnsClrs, ARRAY_COUNT(NavBtnsClrs));

	FString* AMText[] = {
		&AvailableMapText_0,
		&AvailableMapText_1,
		&AvailableMapText_2,
		&AvailableMapText_3,
		&AvailableMapText_4,
		&AvailableMapText_5
	};
	AvailableMaps.Append(AMText, ARRAY_COUNT(AMText));

	FString* SMText[] = {
		&ServerMapText_0,
		&ServerMapText_1,
		&ServerMapText_2,
		&ServerMapText_3,
		&ServerMapText_4,
		&ServerMapText_5
	};
	ServerMaps.Append(SMText, ARRAY_COUNT(SMText));
	
	UpButtonDeligate.BindUFunction(this, FName("Up"));
	AddButtonDeligate.BindUFunction(this, FName("Add"));
	AddAllButtonDeligate.BindUFunction(this, FName("AddAll"));
	RemoveAllButtonDeligate.BindUFunction(this, FName("RemoveAll"));
	RemoveButtonDeligate.BindUFunction(this, FName("Remove"));
	DownButtonDeligate.BindUFunction(this, FName("Down"));

	BackButtonDeligate.BindUFunction(this, FName("Back"));
	StartButtonDeligate.BindUFunction(this, FName("Start"));

	TimerDelegate.BindUFunction(this, FName("ClearTimer"));
}

void UCreateServerMenuUserWidget::TickEvent(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (UpdateItemColors(AvailableMapButtons, AvailableMapColors, SelectedAvailableMapIdx, ShiftAvailable))
	{
		Add();
	}
	if (UpdateItemColors(ServerMapButtons, ServerMapColors, SelectedServerMapIdx, ShiftServer))
	{
		Remove();
	}

	if (SelectedServerMapIdx >= ServerMapsTotal.Num()) SelectedServerMapIdx = - 1;
	
	/*Non-selectable*/
	dummy = -1;
	UpdateItemColors(ControlButtons, ControlColors, dummy, 0);
	dummy = -1;
	UpdateItemColors(NavigateButtons, NavBtnsColors, dummy, 0);
}

bool UCreateServerMenuUserWidget::UpdateItemColors(TArray<UButton*> Buttons, TArray<FSlateColor*> ItemColors, int32 &SelectedItem, int32 Shift)
{
	bool bDoubleClick = false;

	for (int i = 0; i < Buttons.Num(); i++){
		if (SelectedItem - Shift == i){
			*ItemColors[i] = FLinearColor(0.784, 0.784, 0.784);
		}
		else{
			*ItemColors[i] = Buttons[i]->IsHovered() ? FLinearColor(1, 0, 0) : DefaultColor;
		}

		/*check for double click*/
		if (Buttons[i]->IsPressed())
		{
			if (Buttons[i]->GetUniqueID() != LastPressedButtonID)
			{
				LastPressedButtonID = Buttons[i]->GetUniqueID();
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.3f, false);
			}
			else if (TimerHandle.IsValid() && bButtonReleased) //GetWorldTimerManager().IsTimerActive
			{
				bDoubleClick = true;
			}
			bButtonReleased = false;
		}
		else if (Buttons[i]->GetUniqueID() == LastPressedButtonID){
			SelectedItem = Shift + i;
			bButtonReleased = true;
		}
	}

	return bDoubleClick;
}

void UCreateServerMenuUserWidget::ClearTimer()
{
	TimerHandle.Invalidate();
	LastPressedButtonID = -1;
}

void UCreateServerMenuUserWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	if (!IsDesignTime())
	{
		Init();
	}
}

void UCreateServerMenuUserWidget::Init()
{
	SelectedAvailableMapIdx = -1;
	SelectedServerMapIdx = -1;
	
	if (ControlButtons.Num() >= 6){
		ControlButtons[0]->OnClicked.AddUnique(UpButtonDeligate);
		ControlButtons[1]->OnClicked.AddUnique(AddButtonDeligate);
		ControlButtons[2]->OnClicked.AddUnique(AddAllButtonDeligate);
		ControlButtons[3]->OnClicked.AddUnique(RemoveAllButtonDeligate);
		ControlButtons[4]->OnClicked.AddUnique(RemoveButtonDeligate);
		ControlButtons[5]->OnClicked.AddUnique(DownButtonDeligate);
	}

	if (NavigateButtons.Num() >= 2){
		NavigateButtons[0]->OnClicked.AddUnique(BackButtonDeligate);
		NavigateButtons[1]->OnClicked.AddUnique(StartButtonDeligate);
	}

	UWorld* World = GetWorld();
	if (World != NULL)
	{
		AvailableMapsTotal = Cast<UPKGameInstance>(World->GetGameInstance())->DMLevels;		

		AvailableMapsSliderVisibility = AvailableMapsTotal.Num() > AvailableMaps.Num() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;				
		UpdateMapButtonsText(AvailableMapsTotal, AvailableMaps, ShiftAvailable);

		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
		ServerMapsTotal.Empty();
		for (auto item : GI->CurrentMapsList){
			ServerMapsTotal.Add(item);
		}
		
		ServerMapsSliderVisibility = ServerMapsTotal.Num() > ServerMaps.Num() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		UpdateMapButtonsText(ServerMapsTotal, ServerMaps, ShiftServer);
	}
}

float UCreateServerMenuUserWidget::UpdateMapButtonsText(TArray<FString> TotalMaps, TArray<FString*> &Maps, int32 &Shift)
{
	/*validate 'Shift' value*/
	Shift = (Shift < 0 || TotalMaps.Num() <= Maps.Num()) ? 0 :
		Shift > (TotalMaps.Num() - Maps.Num()) ? (TotalMaps.Num() - Maps.Num()) : Shift;

	for (int32 i = 0; i < Maps.Num(); i++){
		*Maps[i] = TotalMaps.Num() > i + Shift ? TotalMaps[i + Shift] : FString();
	}

	return CalcSliderValue(TotalMaps.Num(), Maps.Num(), Shift);
}

void UCreateServerMenuUserWidget::ScrollUpAvailableMaps()
{
	AvailableMapsSliderValue = UpdateMapButtonsText(AvailableMapsTotal, AvailableMaps, --ShiftAvailable);
	UpdateAvailableMapsSlider();
}
void UCreateServerMenuUserWidget::ScrollDownAvailableMaps()
{
	AvailableMapsSliderValue = UpdateMapButtonsText(AvailableMapsTotal, AvailableMaps, ++ShiftAvailable);
	UpdateAvailableMapsSlider();
}
void UCreateServerMenuUserWidget::ScrollUpServerMaps()
{
	ServerMapsSliderValue = UpdateMapButtonsText(ServerMapsTotal, ServerMaps, --ShiftServer);
	UpdateServerMapsSlider();
}
void UCreateServerMenuUserWidget::ScrollDownServerMaps()
{
	ServerMapsSliderValue = UpdateMapButtonsText(ServerMapsTotal, ServerMaps, ++ShiftServer);
	UpdateServerMapsSlider();
}

void UCreateServerMenuUserWidget::SetAvailableMapsSlider(USlider* InSlider)
{
	InSlider->SetValue(AvailableMapsSliderValue);
}

void UCreateServerMenuUserWidget::SetServerMapsSlider(USlider* InSlider)
{
	InSlider->SetValue(ServerMapsSliderValue);
}

void UCreateServerMenuUserWidget::OnAvailableMapsSliderValueChanged(float NewValue)
{
	if (NewValue != AvailableMapsSliderValue && AvailableMapsTotal.Num() > AvailableMaps.Num())
	{
		float pitch = 1.f / (AvailableMapsTotal.Num() - AvailableMaps.Num());
		if (FMath::Abs(NewValue - AvailableMapsSliderValue) >= pitch)
		{
			if (NewValue > AvailableMapsSliderValue) ScrollUpAvailableMaps();
			else ScrollDownAvailableMaps();
		}
	}
}

void UCreateServerMenuUserWidget::OnServerMapsSliderValueChanged(float NewValue)
{
	if (NewValue != ServerMapsSliderValue && ServerMapsTotal.Num() > ServerMaps.Num())
	{
		float pitch = 1.f / (ServerMapsTotal.Num() - ServerMaps.Num());
		if (FMath::Abs(NewValue - ServerMapsSliderValue) >= pitch)
		{
			if (NewValue > ServerMapsSliderValue) ScrollUpServerMaps();
			else ScrollDownServerMaps();
		}
	}
}

float UCreateServerMenuUserWidget::CalcSliderValue(int32 TotalLen, int32 FrameLen, int32 Shift)
{
	float pitches = TotalLen - FrameLen;
	return pitches > 0.f ? (pitches - Shift) / pitches : 1.f;
}

void UCreateServerMenuUserWidget::UpdateServerMapsList()
{
	ServerMapsSliderVisibility = ServerMapsTotal.Num() > ServerMaps.Num() ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	ServerMapsSliderValue = UpdateMapButtonsText(ServerMapsTotal, ServerMaps, ShiftServer);
	UpdateServerMapsSlider();

	UWorld* World = GetWorld();
	if (World != NULL)
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
		GI->StoreCurrentMaps(ServerMapsTotal);
	}
}

void UCreateServerMenuUserWidget::Up()
{
	if (SelectedServerMapIdx > 0)
	{
		ServerMapsTotal.Swap(SelectedServerMapIdx, SelectedServerMapIdx - 1);
		SelectedServerMapIdx--;
		
		if (SelectedServerMapIdx < ShiftServer) ShiftServer--;
		
		UpdateServerMapsList();
	}
}
void UCreateServerMenuUserWidget::Add()
{
	if (SelectedAvailableMapIdx >= 0 && SelectedAvailableMapIdx < AvailableMapsTotal.Num()){
		ServerMapsTotal.AddUnique(AvailableMapsTotal[SelectedAvailableMapIdx]);
		UpdateServerMapsList();
	}
}
void UCreateServerMenuUserWidget::AddAll()
{
	ServerMapsTotal.Empty(); ShiftServer = 0; SelectedServerMapIdx = -1;
	for (auto item : AvailableMapsTotal) {
		ServerMapsTotal.AddUnique(item);
	}
	UpdateServerMapsList();
}
void UCreateServerMenuUserWidget::RemoveAll()
{
	ServerMapsTotal.Empty(); ShiftServer = 0; SelectedServerMapIdx = -1;
	UpdateServerMapsList();
}
void UCreateServerMenuUserWidget::Remove()
{
	if (SelectedServerMapIdx >= 0 && SelectedServerMapIdx < ServerMapsTotal.Num()) {
		ServerMapsTotal.RemoveAt(SelectedServerMapIdx, true);
		int32 offset = SelectedServerMapIdx - ShiftServer; // store offset
		UpdateServerMapsList();
		SelectedServerMapIdx = ShiftServer + offset; // restore MapIdx after 'ShiftServer' is updated
	}
}
void UCreateServerMenuUserWidget::Down()
{	
	if (SelectedServerMapIdx >= 0 && SelectedServerMapIdx + 1 < ServerMapsTotal.Num())
	{
		ServerMapsTotal.Swap(SelectedServerMapIdx, SelectedServerMapIdx + 1);
		SelectedServerMapIdx++;
		
		if (SelectedServerMapIdx >= ShiftServer + ServerMaps.Num()) ShiftServer++;
		
		UpdateServerMapsList();
	}
}

void UCreateServerMenuUserWidget::Back()
{
	UWorld* World = GetWorld();
	if (World != NULL)
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
		GI->ShowWidgetClassOf(GI->MultiplayerMenuWidgetClass);
	}
}

void UCreateServerMenuUserWidget::Start()
{
	UWorld* World = GetWorld();
	if (World != NULL)
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());
		GI->OnStartButtonClick();
	}
}

/**
void UCreateServerMenuUserWidget::AvMapBtnClick0()
{
}

void UCreateServerMenuUserWidget::AvMapBtnClick1()
{
}

void UCreateServerMenuUserWidget::AvMapBtnClick2()
{
}

void UCreateServerMenuUserWidget::AvMapBtnClick3()
{
}

void UCreateServerMenuUserWidget::AvMapBtnClick4()
{
}

void UCreateServerMenuUserWidget::AvMapBtnClick5()
{
}

void UCreateServerMenuUserWidget::SrvMapBtnClick0()
{
}

void UCreateServerMenuUserWidget::SrvMapBtnClick1()
{
}

void UCreateServerMenuUserWidget::SrvMapBtnClick2()
{
}

void UCreateServerMenuUserWidget::SrvMapBtnClick3()
{
}

void UCreateServerMenuUserWidget::SrvMapBtnClick4()
{
}

void UCreateServerMenuUserWidget::SrvMapBtnClick5()
{
}
*/