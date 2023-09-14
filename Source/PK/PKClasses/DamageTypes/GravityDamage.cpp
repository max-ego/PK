// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "GravityDamage.h"

UGravityDamage::UGravityDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::gravity;
}


