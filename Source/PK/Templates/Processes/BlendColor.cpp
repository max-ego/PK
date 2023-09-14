// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "BlendColor.h"


UBlendColor::UBlendColor()
:Super()
{
	AddToRoot();

	OnWorldCleanupDelegateHandle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &UBlendColor::RemoveFromRootSet);
}

void UBlendColor::RemoveFromRootSet(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	RemoveFromRoot();
}

void UBlendColor::Init(FLinearColor& Out, FLinearColor NewColor, float InBlendTime)
{
	OutColor = &Out;
	A = Out;
	B = NewColor;
	BlendTime = FMath::Abs(InBlendTime);
	BlendValue = 0.f;
}

void UBlendColor::Destroy()
{
	RemoveFromRoot();
	FWorldDelegates::OnWorldCleanup.Remove(OnWorldCleanupDelegateHandle);
	ConditionalBeginDestroy();
}

void UBlendColor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BlendValue < 1.f)
	{
		BlendValue = BlendTime > 0 ? BlendValue + DeltaTime / BlendTime : 1.f;
		Blend(BlendValue);
	}
}

void UBlendColor::Blend(float blend)
{
	blend = FMath::Clamp(blend, 0.f, 1.f);

	if (OutColor != nullptr)
	{
		*OutColor = FLinearColor
			(
			A.R - (A.R - B.R) * blend,
			A.G - (A.G - B.G) * blend,
			A.B - (A.B - B.B) * blend,
			A.A - (A.A - B.A) * blend
			);
	}

	if (blend == 1)
	{
		Destroy();
	}
}

