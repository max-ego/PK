// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PainheadDamage.h"

UPainheadDamage::UPainheadDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::painhead;
}

