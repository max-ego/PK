// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "VideoOptionsUserWidget.h"
#include "Main/Util.h"
#include "Main/NetPlayer.h"


UVideoOptionsUserWidget::UVideoOptionsUserWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	FSlateColor* CtrlClrs[] = {
		&BackButtonColor,
	};
	ControlColors.Append(CtrlClrs, ARRAY_COUNT(CtrlClrs));

	static ConstructorHelpers::FObjectFinder<USoundBase>ScrollerMoveSound(TEXT("SoundWave'/Game/Sounds/menu/menu/scroller-move.scroller-move'"));
	ScrollerMove = ScrollerMoveSound.Object;
}

void UVideoOptionsUserWidget::OnWidgetRebuilt()
{
	if (!IsDesignTime())
	{
		PC = UGameplayStatics::GetPlayerController(this, 0);

		if (GEngine != nullptr)
		{
			Settings = GEngine->GameUserSettings;

			Resolution   = (float)Settings->ScalabilityQuality.ResolutionQuality   / 100;
			ViewDistance = (float)Settings->ScalabilityQuality.ViewDistanceQuality / 3;
			Texture      = (float)Settings->ScalabilityQuality.TextureQuality      / 3;
			AntiAliasing = (float)Settings->ScalabilityQuality.AntiAliasingQuality / 3;
			Shadow       = (float)Settings->ScalabilityQuality.ShadowQuality       / 3;
			PostProcess  = (float)Settings->ScalabilityQuality.PostProcessQuality  / 3;
			Effects      = (float)Settings->ScalabilityQuality.EffectsQuality      / 3;

			/*WORKAROUND: read HardwareTargetingSettings*/
			FURL URL;
			URL.LoadURLConfig(TEXT("/Script/HardwareTargeting.HardwareTargetingSettings"), GEngineIni);
			FString c = URL.GetOption(TEXT("AppliedTargetedHardwareClass="), TEXT("Unspecified"));

			if (c == "Desktop")
			{
				bResolution = false;
				bViewDistance = true;
				bTexture = true;
				bAntiAliasing = true;
				bShadow = true;
				bPostProcess = true;
				bEffects = false;
			}
			else if (c == "Mobile")
			{
				bResolution = false;
				bViewDistance = true;
				bTexture = true;
				bAntiAliasing = false;
				bShadow = true;
				bPostProcess = false;
				bEffects = false;
			}
			else if (c == "Unspecified")
			{
				bResolution = false;
				bViewDistance = false;
				bTexture = false;
				bAntiAliasing = false;
				bShadow = false;
				bPostProcess = false;
				bEffects = false;
			}
		}
	}

	Super::OnWidgetRebuilt();
}

void UVideoOptionsUserWidget::TickEvent(const FGeometry& MyGeometry, float InDeltaTime)
{
	//...
	/*Non-selectable*/
	int32 dummy = -1;
	UpdateItemColors(ControlButtons, ControlColors, dummy, 0);
}

void UVideoOptionsUserWidget::UpdateItemColors(TArray<UButton*> Buttons, TArray<FSlateColor*> ItemColors, int32 &SelectedItem, int32 Shift)
{
	for (int i = 0; i < Buttons.Num(); i++){
		if (SelectedItem - Shift == i){
			*ItemColors[i] = FLinearColor(0.784, 0.784, 0.784);
		}
		else{
			*ItemColors[i] = Buttons[i]->IsHovered() ? FLinearColor(1, 0, 0) : DefaultColor;
		}

		if (Buttons[i]->IsPressed())
		{
			if (Buttons[i]->GetUniqueID() != LastPressedButtonID)
			{
				LastPressedButtonID = Buttons[i]->GetUniqueID();
			}
		}
		else if (Buttons[i]->GetUniqueID() == LastPressedButtonID){
			SelectedItem = Shift + i;
		}
	}
}

void UVideoOptionsUserWidget::ResolutionUp()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.ResolutionQuality = FMath::Clamp(Settings->ScalabilityQuality.ResolutionQuality += 10, 0, 100);
		Settings->ApplySettings(0);
		Resolution = (float)Settings->ScalabilityQuality.ResolutionQuality / 100;
	}
}

void UVideoOptionsUserWidget::ResolutionDown()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.ResolutionQuality = FMath::Clamp(Settings->ScalabilityQuality.ResolutionQuality -= 10, 0, 100);
		Settings->ApplySettings(0);
		Resolution = (float)Settings->ScalabilityQuality.ResolutionQuality / 100;
	}
}

void UVideoOptionsUserWidget::ViewDistanceUp()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.ViewDistanceQuality = FMath::Clamp(++Settings->ScalabilityQuality.ViewDistanceQuality, 0, 3);
		Settings->ApplySettings(0);
		ViewDistance = (float)Settings->ScalabilityQuality.ViewDistanceQuality / 3;
	}
}

void UVideoOptionsUserWidget::ViewDistanceDown()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.ViewDistanceQuality = FMath::Clamp(--Settings->ScalabilityQuality.ViewDistanceQuality, 0, 3);
		Settings->ApplySettings(0);
		ViewDistance = (float)Settings->ScalabilityQuality.ViewDistanceQuality / 3;
	}
}

void UVideoOptionsUserWidget::TextureUp()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.TextureQuality = FMath::Clamp(++Settings->ScalabilityQuality.TextureQuality, 0, 3);
		Settings->ApplySettings(0);
		Texture = (float)Settings->ScalabilityQuality.TextureQuality / 3;
	}
}

void UVideoOptionsUserWidget::TextureDown()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.TextureQuality = FMath::Clamp(--Settings->ScalabilityQuality.TextureQuality, 0, 3);
		Settings->ApplySettings(0);
		Texture = (float)Settings->ScalabilityQuality.TextureQuality / 3;
	}
}

void UVideoOptionsUserWidget::AntiAliasingUp()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.AntiAliasingQuality = FMath::Clamp(++Settings->ScalabilityQuality.AntiAliasingQuality, 0, 3);
		Settings->ApplySettings(0);
		AntiAliasing = (float)Settings->ScalabilityQuality.AntiAliasingQuality / 3;
	}
}

void UVideoOptionsUserWidget::AntiAliasingDown()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.AntiAliasingQuality = FMath::Clamp(--Settings->ScalabilityQuality.AntiAliasingQuality, 0, 3);
		Settings->ApplySettings(0);
		AntiAliasing = (float)Settings->ScalabilityQuality.AntiAliasingQuality / 3;
	}
}

void UVideoOptionsUserWidget::ShadowUp()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.ShadowQuality = FMath::Clamp(++Settings->ScalabilityQuality.ShadowQuality, 0, 3);
		Settings->ApplySettings(0);
		Shadow = (float)Settings->ScalabilityQuality.ShadowQuality / 3;
	}
}

void UVideoOptionsUserWidget::ShadowDown()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.ShadowQuality = FMath::Clamp(--Settings->ScalabilityQuality.ShadowQuality, 0, 3);
		Settings->ApplySettings(0);
		Shadow = (float)Settings->ScalabilityQuality.ShadowQuality / 3;
	}
}

void UVideoOptionsUserWidget::PostProcessUp()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.PostProcessQuality = FMath::Clamp(++Settings->ScalabilityQuality.PostProcessQuality, 0, 3);
		Settings->ApplySettings(0);
		PostProcess = (float)Settings->ScalabilityQuality.PostProcessQuality / 3;
	}
}

void UVideoOptionsUserWidget::PostProcessDown()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.PostProcessQuality = FMath::Clamp(--Settings->ScalabilityQuality.PostProcessQuality, 0, 3);
		Settings->ApplySettings(0);
		PostProcess = (float)Settings->ScalabilityQuality.PostProcessQuality / 3;
	}
}
void UVideoOptionsUserWidget::EffectsUp()
{
	if (Settings && PC)
	{
		Settings->ScalabilityQuality.EffectsQuality = FMath::Clamp(++Settings->ScalabilityQuality.EffectsQuality, 0, 3);
		Settings->ApplySettings(0);
		Effects = (float)Settings->ScalabilityQuality.EffectsQuality / 3;
		ResetPawnAppearance();
	}
}

void UVideoOptionsUserWidget::EffectsDown()
{
	if (Settings)
	{
		Settings->ScalabilityQuality.EffectsQuality = FMath::Clamp(--Settings->ScalabilityQuality.EffectsQuality, 0, 3);
		Settings->ApplySettings(0);
		Effects = (float)Settings->ScalabilityQuality.EffectsQuality / 3;
		ResetPawnAppearance();
	}
}

void UVideoOptionsUserWidget::ResetPawnAppearance()
{
	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> Pawns;
		UGameplayStatics::GetAllActorsOfClass(World, ANetPlayer::StaticClass(), Pawns);

		for (auto pawn : Pawns)
		{
			ANetPlayer* player = Cast<ANetPlayer>(pawn);
			if (!player->IsLocallyControlled())
			{
				player->DisplayCorrectWeapon();
			}
		}
	}
}

void UVideoOptionsUserWidget::OnViewDistanceValueChanged(float NewValue)
{
	if (ViewDistance == NewValue) return;

	float pitch = 0.5 * 1 / 3;
	if (FMath::Abs(NewValue - ViewDistance) >= pitch)
	{
		if (ViewDistance < NewValue) ViewDistanceUp();
		else ViewDistanceDown();

		PlaySound(ScrollerMove);
	}
}

void UVideoOptionsUserWidget::OnTextureValueChanged(float NewValue)
{
	if (Texture == NewValue) return;

	float pitch = 0.5 * 1 / 3;
	if (FMath::Abs(NewValue - Texture) >= pitch)
	{
		if (Texture < NewValue) TextureUp();
		else TextureDown();

		PlaySound(ScrollerMove);
	}
}

void UVideoOptionsUserWidget::OnAntiAliasingValueChanged(float NewValue)
{
	if (AntiAliasing == NewValue) return;

	float pitch = 0.5 * 1 / 3;
	if (FMath::Abs(NewValue - AntiAliasing) >= pitch)
	{
		if (AntiAliasing < NewValue) AntiAliasingUp();
		else AntiAliasingDown();

		PlaySound(ScrollerMove);
	}
}

void UVideoOptionsUserWidget::OnShadowValueChanged(float NewValue)
{
	if (Shadow == NewValue) return;

	float pitch = 0.5 * 1 / 3;
	if (FMath::Abs(NewValue - Shadow) >= pitch)
	{
		if (Shadow < NewValue) ShadowUp();
		else ShadowDown();

		PlaySound(ScrollerMove);
	}
}

void UVideoOptionsUserWidget::OnPostProcessValueChanged(float NewValue)
{
	if (PostProcess == NewValue) return;

	float pitch = 0.5 * 1 / 3;
	if (FMath::Abs(NewValue - PostProcess) >= pitch)
	{
		if (PostProcess < NewValue) PostProcessUp();
		else PostProcessDown();

		PlaySound(ScrollerMove);
	}
}

void UVideoOptionsUserWidget::OnEffectsValueChanged(float NewValue)
{
	if (Effects == NewValue) return;

	float pitch = 0.5 * 1 / 3;
	if (FMath::Abs(NewValue - Effects) >= pitch)
	{
		if (Effects < NewValue) EffectsUp();
		else EffectsDown();

		PlaySound(ScrollerMove);
	}
}