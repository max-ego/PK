// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ElectroDamage.h"

UElectroDamage::UElectroDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::electro;
}


