// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "GrenadeDamage.h"

UGrenadeDamage::UGrenadeDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::grenade;
}


