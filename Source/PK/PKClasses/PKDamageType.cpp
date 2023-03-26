// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKDamageType.h"

UPKDamageType::UPKDamageType(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::undefined;
}


