// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PKClasses/DestructibleItemBase.h"
#include "BarrelBig_MP.generated.h"

UCLASS()
class PK_API ABarrelBig_MP : public ADestructibleItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarrelBig_MP(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/*UFUNCTION()
	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);*/
			
	virtual void TakeAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float Health = 201.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float RespawnTime;
	void Activate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particles)
	UParticleSystem* ExplosionParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	USoundBase* ExplosionSnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
	USoundBase* RespawnSound;

protected:

	/*TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;*/

	UPROPERTY(ReplicatedUsing = OnRep_Toggle)
	bool bActive = true;
	UFUNCTION()
	void OnRep_Toggle();

	FTimerHandle TimerHandle;
	FTimerHandle OnRespawnOverlapTimerHandle;
	/*UFUNCTION()*/
	FORCEINLINE void Explode(){ CurrHealth = 0.f; };
	void CheckIfAnyPawnIsInsideMe();

	// timer to make sure AttachedActors(like 'stakes') are registered 
	FTimerDelegate DeactivateTimerDelegate;
	UFUNCTION()
	void Deactivate();
	AController* Instigator;
	
	float RemainingHealth = Health;
	float CurrHealth = Health;
	float LastHealth = Health;
	float BlendTime = 0.5f;
	float BlendValue = 0.f;

	void TraceApplyDmg(AActor* actor, AActor* IgnoreActor);
};
