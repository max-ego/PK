// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "LavaDamage.h"


ULavaDamage::ULavaDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::lava;
}

