// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ShurikenDamage.h"

UShurikenDamage::UShurikenDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::shuriken;
}


