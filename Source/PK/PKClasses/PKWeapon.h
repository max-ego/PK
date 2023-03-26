// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Main/Util.h"

#include "PKWeapon.generated.h"

UENUM()
namespace EWeapons
{
	enum Type{
		PainKiller = 0x01,
		Shotgun = 0x02,
		StakeGunGL = 0x04,
		MiniGunRL = 0x08,
		DriverElectro = 0x10,
		RifleFlameThrower = 0x20,
		BoltGunHeater = 0x40,
		DemonGun = 0x80
	};
}
enum EFireType{
	Fire,
	AltFire
};

UCLASS(config = Game)
class PK_API APKWeapon : public AActor
{
	GENERATED_BODY()

	class USceneComponent* Scene;
	
	

public:	
	// Sets default values for this actor's properties
	APKWeapon(const FObjectInitializer& ObjectInitializer);

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APKProjectile> ProjectileClass;
	
	/** AltFire Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APKProjectile> AltProjectileClass;

	/** Combo Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APKProjectile> ComboProjectileClass;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual bool Fire();

	virtual bool OnFinishFire();

	virtual bool AltFire();

	virtual bool OnFinishAltFire();

	virtual void OnHeadBack();

	virtual void OnOwnerUnPossessed();

	class ANetPlayer* PLAYER;
	
private:

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* WeaponMesh;

	void PLAY(UAnimMontage* montage);

	void PLAYSND(USoundCue* SoundCue);

	bool CheckAmmo(TSubclassOf<class APKProjectile> ProjectileClass);

	bool SpawnProjectile(TSubclassOf<class APKProjectile> ProjectileCls);
	
	UAudioComponent* AudioComponent;

	// see if primary attack is on
	bool bFirePressed;
	// see if alternative attack is on
	bool bAltFirePressed;

	FTimerHandle FireTimerHandle;
	FTimerHandle AltFireTimerHandle;
	float FireTimeout = 0.f;
	float AltFireTimeout = 0.f;

	TEnumAsByte<EWeapons::Type> WeaponType;
	void InitFireTimeOut();
	
	bool CheckFireTimeOut();
	FORCEINLINE void OnFireTimeOut(){ if (bFirePressed) Fire(); }
	
	bool CheckAltFireTimeOut();
	FORCEINLINE void OnAltFireTimeOut(){ if (bAltFirePressed) AltFire(); }
		
	void UpdateFireTimeOut(TEnumAsByte<EFireType> FireType);
	
	USoundBase* OutOfAmmo;
	UAudioComponent* OutOfAmmoAudioComponent;
	USoundCue* OutOf_Ammo;

public:
		
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	class USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; };

	FORCEINLINE virtual void OnChangeWeapon(){ 
		bFirePressed ? OnFinishFire() : NULL;
		bAltFirePressed ? OnFinishAltFire() : NULL;
	}

	// StakeGunGL only
	virtual void HideStake(bool bHide){};
};
