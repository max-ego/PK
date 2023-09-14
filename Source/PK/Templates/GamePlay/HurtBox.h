// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HurtBox.generated.h"

UCLASS()
class PK_API AHurtBox : public AActor
{
	GENERATED_BODY()

	/*show transform options in Blueprint*/
	class USceneComponent* Scene;
	
public:	
	// Sets default values for this actor's properties
	AHurtBox();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Box collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Collision)
	class UBoxComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GamePlay, meta = (ClampMin = "0", UIMin = "0"))
	float Damage = 10.0f;

	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);

	UFUNCTION()
	void OnEndOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	TScriptDelegate<FWeakObjectPtr> HitDelegate;
	TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;
	TScriptDelegate<FWeakObjectPtr> EndOverlapDelegate;

	class USoundAttenuation* AttenuationSettings;
	USoundBase* snd;	
};
