// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DamageType.h"
#include "PKDamageType.generated.h"

/**
 * 
 */

UENUM()
namespace EDamageCause
{
	enum Type{
		undefined,
		painhead,
		painrotor,
		painbeam,
		stake,
		grenade,
		shell,
		icebullet,
		rocket,
		minigun,
		shuriken,
		electro,
		telefrag,
		outofworld,
		gravity,
		lava,
		explosion
	};
}

UCLASS()
class PK_API UPKDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	UPKDamageType(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageType)
	TEnumAsByte<EDamageCause::Type> CausedBy;
};
