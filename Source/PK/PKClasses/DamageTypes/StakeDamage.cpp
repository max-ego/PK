// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "StakeDamage.h"

UStakeDamage::UStakeDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::stake;
}


