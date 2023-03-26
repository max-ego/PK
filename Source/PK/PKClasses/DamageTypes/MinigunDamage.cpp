// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MinigunDamage.h"

UMinigunDamage::UMinigunDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::minigun;
}


