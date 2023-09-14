// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ExplosionDamage.h"


UExplosionDamage::UExplosionDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::explosion;
}

