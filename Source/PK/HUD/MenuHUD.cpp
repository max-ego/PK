// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MenuHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"


AMenuHUD::AMenuHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/Textures/HUD/kursor"));
	CrosshairTex = CrosshiarTexObj.Object;
}

void AMenuHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((100.f - (CrosshairTex->GetSurfaceWidth() * 0.5)),
		(100.f - (CrosshairTex->GetSurfaceHeight() * 0.5f)));
	
	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}