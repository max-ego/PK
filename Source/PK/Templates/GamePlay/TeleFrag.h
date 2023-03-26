// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PKClasses/PKDamageType.h"
#include "TeleFrag.generated.h"

UCLASS()
class PK_API ATeleFrag : public AActor
{
	GENERATED_BODY()

	/*show transform options in Blueprint*/
	class USceneComponent* Scene;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USphereComponent* CollisionComp;
	
public:	
	// Sets default values for this actor's properties
	ATeleFrag(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	

	UFUNCTION()
	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);
	
	void Activate(ACharacter* Instigator);
	void Deactivate();

protected:

	TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;

	ACharacter* FragInstigator = nullptr;
};
