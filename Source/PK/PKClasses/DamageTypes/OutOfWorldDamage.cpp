// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "OutOfWorldDamage.h"

UOutOfWorldDamage::UOutOfWorldDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::outofworld;
}


