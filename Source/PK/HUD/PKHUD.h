// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "PKHUD.generated.h"

UCLASS()
class APKHUD : public AHUD
{
	GENERATED_BODY()

public:
	APKHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	class UTexture2D* LeftPanTex;
	class UTexture2D* RightPanTex;
	UMaterial* LeftPanMat;
	UMaterial* RightPanMat;
	class UTexture* LeftPanTexure;
	class UTexture* RightPanTexure;
	bool bSuccess;


	class UTexture2D* NumbersTex;
	class UTexture2D* NumbersRedTex;

	class UTexture2D* HealthTex;

	class UTexture2D* ArmorTex;
	class UTexture2D* ArmorRedTex; // Gold
	class UTexture2D* ArmorYellowTex; // Silver
	class UTexture2D* ArmorGreenTex; // Bronze
	TArray<UTexture2D*> ArmorIcon;

	//ammo
	class UTexture2D* PainKillerOpenTex;
	class UTexture2D* PainKillerCloseTex;
	class UTexture2D* ShotgunShellTex;
	class UTexture2D* ShotgunFeezerTex;
	class UTexture2D* StakesTex;
	class UTexture2D* RocketTex;
	class UTexture2D* MinigunTex;
	class UTexture2D* InfinityTex;

	// console
	class UTexture2D* StripesTex;

	const float SpaceX = 10.f;
	const float SpaceY = 0.f;
	const uint32 DigitWidthSpan = 22;
	const uint32 DigitHeightSpan = 34;
	const uint32 EnergySpan = 36;

	void DrawMat(FVector2D Pos, UMaterial* RenderMaterial, FVector2D Size);
	void DrawItem(FVector2D Pos, UTexture2D* InTex);
	void DrawNumbers(uint8 Val, uint8 WarnThreshold, FVector2D Pos, bool IsTop);

	void DrawConsole();

	UFont* FontTimes;
	UFont* Font;
	void DrawMessages(TArray<FString> Messages, float Scale);
	FFontRenderInfo FontRenderInfo = FFontRenderInfo();
	float LastViewportSizeX = 0;
};
