// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PainbeamDamage.h"

UPainbeamDamage::UPainbeamDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::painbeam;
}


