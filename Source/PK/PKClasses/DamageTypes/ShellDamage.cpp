// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "ShellDamage.h"

UShellDamage::UShellDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::shell;
}


