// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "IcebulletDamage.h"

UIcebulletDamage::UIcebulletDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::icebullet;
}


