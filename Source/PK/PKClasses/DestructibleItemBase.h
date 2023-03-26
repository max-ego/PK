// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DestructibleItemBase.generated.h"

UCLASS()
class PK_API ADestructibleItemBase : public AActor
{
	GENERATED_BODY()

	/*show transform options in Blueprint*/
	class USceneComponent* Scene;
	
public:	
	// Sets default values for this actor's properties
	ADestructibleItemBase(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleDefaultsOnly, Category = Actor)
	class UCapsuleComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Actor)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Actor)
	TSubclassOf<class ADestructibleActor> DestroyPackClass;
	/*class ADestructibleActor* DestroyPack;*/

	/** Returns CollisionComp subobject **/
	FORCEINLINE class UCapsuleComponent* GetCollisionComp() const { return CollisionComp; }

	FORCEINLINE class UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComponent; }

	TScriptDelegate<FWeakObjectPtr> OnTakeAnyDamageDeligate;
	UFUNCTION()
	virtual void TakeAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	
	
};
