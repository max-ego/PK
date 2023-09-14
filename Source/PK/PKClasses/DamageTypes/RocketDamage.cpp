// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "RocketDamage.h"

URocketDamage::URocketDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::rocket;
}


