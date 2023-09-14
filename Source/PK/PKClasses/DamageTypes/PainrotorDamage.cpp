// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PainrotorDamage.h"

UPainrotorDamage::UPainrotorDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::painrotor;
}


