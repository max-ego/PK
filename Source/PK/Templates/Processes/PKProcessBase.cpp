// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKProcessBase.h"


UPKProcessBase::UPKProcessBase()
{
	bTickable = GIsRunning;
}

void UPKProcessBase::Tick(float DeltaTime)
{
	//Don't invoke Super::Tick()
	//Super::Tick(DeltaTime);
}

bool UPKProcessBase::IsTickable() const
{
	//notify engine to ingore Tick of the object constructed before game running.
	return bTickable;
}
TStatId UPKProcessBase::GetStatId() const
{
	return UObject::GetStatID();
}

