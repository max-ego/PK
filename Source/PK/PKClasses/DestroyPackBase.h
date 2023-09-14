// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhysicsEngine/DestructibleActor.h"
#include "DestroyPackBase.generated.h"

/**
 * 
 */
UCLASS()
class PK_API ADestroyPackBase : public ADestructibleActor
{
	GENERATED_BODY()
	
public:

	/** The radius to apply the force or impulse in */
	UPROPERTY(interp, EditAnywhere, BlueprintReadWrite, Category = Destruction)
	float Radius;

	/** If > 0.f, will cause damage to destructible meshes as well  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Destruction)
	float DestructibleDamage;

	/** How strong the impulse should be */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Destruction)
	float ImpulseStrength;
	
	// Sets default values for this actor's properties
	ADestroyPackBase(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	FTimerDelegate FireDestructionTimerDelegate;
	UFUNCTION()
	void FireDestruction();
	ARadialForceActor* RadialForceActor;
};
