// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PK.h"
#include "PKHUD.h"
#include "Engine/Font.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Main/NetPlayerController.h"
#include "Main/NetPlayer.h"
#include "Main/Util.h"
#include "Main/PKPlayerState.h"
#include "Engine/UserInterfaceSettings.h"
#include "RenderCore.h" /*GWhiteTexture*/
#include "Materials/MaterialInterface.h"

UENUM()
/*enum EInputMode{
	GameAndUI,
	UIOnly,
	GameOnly,
};*/

enum EWeapon{
	Dummy,
	PainKiller,
	Shotgun,
	Stakegun,
	Minigun
};

APKHUD::APKHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("Texture2D'/Game/Textures/HUD/crosshair.crosshair'"));
	CrosshairTex = CrosshiarTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> LeftTexObj(TEXT("Texture2D'/Game/Textures/HUD/hud_left.hud_left'"));
	LeftPanTex = LeftTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> RightTexObj(TEXT("Texture2D'/Game/Textures/HUD/hud_right.hud_right'"));
	RightPanTex = RightTexObj.Object;
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> LeftTexMatObj(TEXT("MaterialInstanceConstant'/Game/Textures/HUD/Mat/hud_left_mat_Inst.hud_left_mat_Inst'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> RightTexMatObj(TEXT("MaterialInstanceConstant'/Game/Textures/HUD/Mat/hud_right_mat_Inst.hud_right_mat_Inst'"));
	
	LeftPanMat = (UMaterial*)LeftTexMatObj.Object;
	RightPanMat = (UMaterial*)RightTexMatObj.Object;

	// digits
	static ConstructorHelpers::FObjectFinder<UTexture2D> NumbersTexObj(TEXT("Texture2D'/Game/Textures/HUD/numbers.numbers'"));
	NumbersTex = NumbersTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> NumbersRedTexObj(TEXT("Texture2D'/Game/Textures/HUD/numbers_red.numbers_red'"));
	NumbersRedTex = NumbersRedTexObj.Object;

	// health (39x35)
	static ConstructorHelpers::FObjectFinder<UTexture2D> eskulapTexObj(TEXT("Texture2D'/Game/Textures/HUD/energia.energia'"));
	HealthTex = eskulapTexObj.Object;

	// armors (39x37)
	static ConstructorHelpers::FObjectFinder<UTexture2D> ArmorTexObj(TEXT("Texture2D'/Game/Textures/HUD/armor.armor'"));
	ArmorTex = ArmorTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> ArmorRedTexObj(TEXT("Texture2D'/Game/Textures/HUD/armor_czerwony.armor_czerwony'"));
	ArmorRedTex = ArmorRedTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> ArmorYellowTexObj(TEXT("Texture2D'/Game/Textures/HUD/armor_zolty.armor_zolty'"));
	ArmorYellowTex = ArmorYellowTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> ArmorGreenTexObj(TEXT("Texture2D'/Game/Textures/HUD/armor_zielony.armor_zielony'"));
	ArmorGreenTex = ArmorGreenTexObj.Object;
	UTexture2D* Armors[] = { ArmorTex, ArmorGreenTex, ArmorYellowTex, ArmorRedTex };	
	ArmorIcon.Append(Armors, ARRAY_COUNT(Armors));

	// ammo
	static ConstructorHelpers::FObjectFinder<UTexture2D> PainKillerOpenTexObj(TEXT("Texture2D'/Game/Textures/HUD/painkiller_open.painkiller_open'"));
	PainKillerOpenTex = PainKillerOpenTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> PainKillerCloseTexObj(TEXT("Texture2D'/Game/Textures/HUD/painkiller_close.painkiller_close'"));
	PainKillerCloseTex = PainKillerCloseTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> ShellTexObj(TEXT("Texture2D'/Game/Textures/HUD/shell.shell'"));
	ShotgunShellTex = ShellTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> FeezerTexObj(TEXT("Texture2D'/Game/Textures/HUD/ikona_freezer.ikona_freezer'"));
	ShotgunFeezerTex = FeezerTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> StakesTexObj(TEXT("Texture2D'/Game/Textures/HUD/kolki.kolki'"));
	StakesTex = StakesTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> RocketTexObj(TEXT("Texture2D'/Game/Textures/HUD/rocket.rocket'"));
	RocketTex = RocketTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> MinigunTexObj(TEXT("Texture2D'/Game/Textures/HUD/minigun.minigun'"));
	MinigunTex = MinigunTexObj.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> InfinityTexObj(TEXT("Texture2D'/Game/Textures/HUD/infinity.infinity'"));
	InfinityTex = InfinityTexObj.Object;

	// console
	static ConstructorHelpers::FObjectFinder<UTexture2D> ConsoleBackgroundTexObj(TEXT("Texture2D'/Game/Textures/HUD/border/tlo_paski.tlo_paski'"));
	StripesTex = ConsoleBackgroundTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UFont> fontTimesObj(TEXT("Font'/Game/Fonts/timesbd.timesbd'"));
	FontTimes = fontTimesObj.Object;
	static ConstructorHelpers::FObjectFinder<UFont> fontObj(TEXT("Font'/Game/Fonts/courbd.courbd'"));
	Font = fontObj.Object;
	FontRenderInfo.bClipText = 1;
}

void APKHUD::BeginPlay()
{
	Super::BeginPlay();

	bSuccess = 
		LeftPanMat->GetTextureParameterValue(FName("MaterialExpressionTextureSampleParameter2D_0"), LeftPanTexure) &&
		RightPanMat->GetTextureParameterValue(FName("MaterialExpressionTextureSampleParameter2D_0"), RightPanTexure);
}

void APKHUD::DrawHUD()
{
	Super::DrawHUD();

	ANetPlayerController* PC = Cast<ANetPlayerController>(GetWorld()->GetGameInstance()->GetFirstLocalPlayerController());
	
	if (PC)
	{
		// FLASHSCREEN
		DrawRect(PC->PickUpFlashColor, 0, 0, Canvas->ClipX, Canvas->ClipY);
		DrawRect(PC->DamageFlashColor, 0, 0, Canvas->ClipX, Canvas->ClipY);

		if (PC->bDrawHUD && /*UTIL.GetCurrentInputMode(PC) == EInputMode::GameOnly*/UTIL.GetCurrentInputMode(PC) != EInputMode::UIOnly)
		{
			const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
			// Crosshair
			const FVector2D CrosshairDrawPosition((Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
				(Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)));
			DrawItem(CrosshairDrawPosition, CrosshairTex);
			
			// Panes
			int32 LeftBase = 0;
			int32 RightBase = 0;
			int32 TopBase = 0;
			
			if (bSuccess)
			{
				LeftBase = SpaceX + (LeftPanTexure->GetSurfaceWidth() - (DigitWidthSpan * 4) - 2);
				RightBase = Canvas->ClipX - SpaceX - RightPanTexure->GetSurfaceWidth() + 17;
				TopBase = Canvas->ClipY - SpaceY - LeftPanTexure->GetSurfaceHeight() + 15;
			
				const FVector2D LeftPanTexDrawPosition(SpaceX, Canvas->ClipY - SpaceY - LeftPanTexure->GetSurfaceHeight());
				const FVector2D RightPanTexDrawPosition(Canvas->ClipX - SpaceX - RightPanTexure->GetSurfaceWidth(), Canvas->ClipY - SpaceY - RightPanTexure->GetSurfaceHeight());
				
				DrawMaterial(LeftPanMat, LeftPanTexDrawPosition.X, LeftPanTexDrawPosition.Y, LeftPanTexure->GetSurfaceWidth(), LeftPanTexure->GetSurfaceHeight(),0,0,1,1);
				DrawMaterial(RightPanMat, RightPanTexDrawPosition.X, RightPanTexDrawPosition.Y, RightPanTexure->GetSurfaceWidth(), RightPanTexure->GetSurfaceHeight(), 0, 0, 1, 1);
			}
			else
			{
				/*const int32*/ LeftBase = SpaceX + (LeftPanTex->GetSurfaceWidth() - (DigitWidthSpan * 4) - 2);
				/*const int32*/ RightBase = Canvas->ClipX - SpaceX - RightPanTex->GetSurfaceWidth() + 17;
				/*const int32*/ TopBase = Canvas->ClipY - SpaceY - LeftPanTex->GetSurfaceHeight() + 15;

				const FVector2D LeftPanTexDrawPosition(SpaceX, Canvas->ClipY - SpaceY - LeftPanTex->GetSurfaceHeight());
				const FVector2D RightPanTexDrawPosition(Canvas->ClipX - SpaceX - RightPanTex->GetSurfaceWidth(), Canvas->ClipY - SpaceY - RightPanTex->GetSurfaceHeight());
				DrawItem(LeftPanTexDrawPosition, LeftPanTex);
				DrawItem(RightPanTexDrawPosition, RightPanTex);
			}

			// Health
			const FVector2D HealthItemPosition(LeftBase - EnergySpan, TopBase);
			DrawItem(HealthItemPosition, HealthTex);
			DrawNumbers(PC->Health, 25, FVector2D(LeftBase, TopBase), true);

			// Armor
			const FVector2D ArmorItemPosition(LeftBase - EnergySpan, TopBase + DigitHeightSpan);
			DrawItem(ArmorItemPosition, ArmorIcon[PC->Armor & 0x03]);
			DrawNumbers((PC->Armor & 0x02 && PC->Armor == 150 ? PC->Armor : (PC->Armor >> 2) * 4), 25, FVector2D(LeftBase, TopBase), false);

			// Ammo
			class UTexture2D* FireTex = PainKillerOpenTex;
			class UTexture2D* AltFireTex = PainKillerCloseTex;
			// Workaround: adjust the position of the icons, as the images in the 
			// original textures are not well centered also textures vary in size.
			FVector2D AdjustF(0, 0);
			FVector2D AdjustAF(0, 0);

			uint8 AmmoPrim = 0;
			uint8 AmmoScnd = 0;

			ANetPlayer* Pawn = Cast<ANetPlayer>(PC->AcknowledgedPawn);
			uint8 Weapon = Pawn ? Pawn->CurWeaponIndex : 0;
			switch (Weapon){
				/*case EWeapon::Dummy:
					FireTex = PainKillerOpenTex;
					AltFireTex = PainKillerCloseTex;
					AmmoPrim = 255;
					AmmoScnd = 255;
					break;*/
			case EWeapon::PainKiller:
				FireTex = PainKillerOpenTex; AdjustF.X = -5; AdjustF.Y = 1;
				AltFireTex = PainKillerCloseTex; AdjustAF.X = -5; AdjustAF.Y = 1;
				AmmoPrim = 255;
				AmmoScnd = 255;
				break;
			case EWeapon::Shotgun:
				FireTex = ShotgunShellTex;
				AltFireTex = ShotgunFeezerTex;
				AmmoPrim = PC->shell;
				AmmoScnd = PC->icebullet;
				break;
			case EWeapon::Stakegun:
				FireTex = StakesTex;
				AltFireTex = RocketTex; AdjustAF.X = 1; AdjustAF.Y = 3;
				AmmoPrim = PC->stake;
				AmmoScnd = PC->grenade;
				break;
			case EWeapon::Minigun:
				FireTex = RocketTex; AdjustF.X = 1; AdjustF.Y = 3;
				AltFireTex = MinigunTex;
				AmmoPrim = PC->grenade;
				AmmoScnd = PC->bullets;
				break;
				/*default:
					FireTex = PainKillerOpenTex;
					AltFireTex = PainKillerCloseTex;
					break;*/
			}

			float PosX = RightBase + (DigitWidthSpan * 3);
			const FVector2D FireItemPosition(PosX + AdjustF.X, TopBase + AdjustF.Y);
			DrawItem(FireItemPosition, FireTex);
			const FVector2D AltFireItemPosition(PosX + AdjustAF.X, TopBase + DigitHeightSpan + AdjustAF.Y);
			DrawItem(AltFireItemPosition, AltFireTex);

			DrawNumbers(AmmoPrim, 5, FVector2D(RightBase, TopBase), true);
			DrawNumbers(AmmoScnd, 5, FVector2D(RightBase, TopBase), false);
		}

		// Console
		if (PC->PlayerState)
		{
			APKPlayerState* PlayerState = Cast<APKPlayerState>(PC->PlayerState);

			const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
			
			if (LastViewportSizeX != ViewportSize.X)
			{
				PlayerState->UpdateConsole();
				PlayerState->UpdateHudMsgs();
				LastViewportSizeX = ViewportSize.X;
			}
			
			float Scale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

			DrawMessages(
				PlayerState->HudMessages, Scale
				);

			if (PC->Role == ROLE_AutonomousProxy && PC->PacketLoss > 0)
			{
				Canvas->DrawColor = FColor(/*0xFFD70017*/0xFFAAAAAA);
				FString sPacketLoss = FString::Printf(L"Packet loss: %i%%", PC->PacketLoss);
				Canvas->DrawText(FontTimes, sPacketLoss, ViewportSize.X - (Scale * 220/*280*/), Scale * 10, Scale, Scale, FontRenderInfo);
			}
						
			// Frag Message
			if (PlayerState->FragMessage.Len())
			{				
				FString White = "You killed:";
				FString Name = PlayerState->FragMessage;
				FString Whole = White + Name + "!";

				float W_White, W_Name, W_Whole, H, _Scale;

				_Scale = Scale * 1.5f;
				
				GetTextSize(White, W_White, H, FontTimes, _Scale);
				GetTextSize(Name, W_Name, H, FontTimes, _Scale);
				GetTextSize(Whole, W_Whole, H, FontTimes, _Scale);
				
				float X = (ViewportSize.X - W_Whole) / 2;
				Canvas->DrawColor = FColor(0xFFFFFFFF);
				Canvas->DrawText(FontTimes, White, X, ViewportSize.Y / 3, _Scale, _Scale, FontRenderInfo);

				X = ((ViewportSize.X - W_Whole) / 2) + W_White;
				Canvas->DrawColor = FColor(0xFFF0E6AA);
				Canvas->DrawText(FontTimes, Name, X, ViewportSize.Y / 3, _Scale, _Scale, FontRenderInfo);

				X = ((ViewportSize.X - W_Whole) / 2) + W_White + W_Name;
				Canvas->DrawColor = FColor(0xFFFFFFFF);
				Canvas->DrawText(FontTimes, "!", X, ViewportSize.Y / 3, _Scale, _Scale, FontRenderInfo);
			}

			// Opposing Pawn Name
			if (PC->OpposingPawnName.Len())
			{
				FString msg = PC->OpposingPawnName;
				float W, H, _Scale;
				_Scale = Scale * 1.5f;
				GetTextSize(msg, W, H, FontTimes, _Scale);
				Canvas->DrawColor = FColor(0xFFC80000);
				Canvas->DrawText(FontTimes, msg, (ViewportSize.X - W) / 2, (ViewportSize.Y / 2) + 50, _Scale, _Scale, FontRenderInfo);
			}
		}
	}
}

void APKHUD::DrawMat(FVector2D Pos, UMaterial* RenderMaterial, FVector2D Size)
{
	FCanvasTileItem TileItem(Pos, RenderMaterial->GetRenderProxy(0), Size);
	Canvas->DrawItem(TileItem);
}

void APKHUD::DrawItem(FVector2D Pos, UTexture2D* InTex)
{
	FCanvasTileItem TileItem(Pos, InTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}

void APKHUD::DrawNumbers(uint8 Val, uint8 WarnThreshold, FVector2D Pos, bool IsTop)
{
	if (Val == 255)
	{
		DrawItem(FVector2D(Pos.X, (IsTop ? Pos.Y : Pos.Y + DigitHeightSpan)), InfinityTex);
		return;
	}

	FTextureResource* Numbers = Val > WarnThreshold ? NumbersTex->Resource : NumbersRedTex->Resource;

	for (int i = 2; i > -1; i--){
		float d = Val % 10; Val /= 10;
		FVector2D ItemPos = FVector2D(Pos.X + (DigitWidthSpan * i), (IsTop ? Pos.Y : Pos.Y + DigitHeightSpan));
		FCanvasTileItem Item = FCanvasTileItem(
			ItemPos, 
			Numbers, 
			FVector2D(Numbers->GetSizeX() / 10, Numbers->GetSizeY()),
			FVector2D(d / 10, 0.0f),
			FVector2D((d / 10) + 0.1f, 1.f),
			FLinearColor::White
			);
		Item.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Item);
	}
}

void APKHUD::DrawConsole()
{
	Canvas->DrawTile(
		StripesTex,
		100, 100, //Position to draw
		Canvas->ClipX - 200, (Canvas->ClipY * 0.5) - 100, //Width/Height of tile.
		0, 0, //Horizontal/Vertical position of the upper left corner of the portion of the texture to be shown(texels).
		StripesTex->GetSizeX(), StripesTex->GetSizeY(), //The width/height of the portion of the texture to be drawn(texels).
		EBlendMode::BLEND_Translucent);
}

void APKHUD::DrawMessages(TArray<FString> Messages, float Scale)
{
	float YExtent = 0;
	Canvas->DrawColor = FColor(0xFFF0E6AA);
	
	for (int32 i = 0; i < Messages.Num(); i++)
	{
		YExtent = Canvas->DrawText(Font, Messages[i], Scale * 40, Scale * 40 + (i * Font->LegacyFontSize * Scale), Scale, Scale, FontRenderInfo);
	}
}