// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "TelefragDamage.h"

UTelefragDamage::UTelefragDamage(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CausedBy = EDamageCause::telefrag;
}


