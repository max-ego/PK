// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "NetPlayer.h"
#include "NetPlayerController.h"
#include "PKPlayerState.h"
#include "UnrealNetwork.h"
#include "PKGameState.h"
#include "Templates/Health/SoulMP.h"

#include "Templates/Weapons/Projectiles/ComboPainHead.h"
#include "Templates/Weapons/Projectiles/Grenade.h"
#include "Templates/Weapons/Projectiles/IceBullet.h"
#include "Templates/Weapons/Projectiles/MiniGunFire.h"
#include "Templates/Weapons/Projectiles/PainHead.h"
#include "Templates/Weapons/Projectiles/PainKillerFire.h"
#include "Templates/Weapons/Projectiles/Rocket.h"
#include "Templates/Weapons/Projectiles/SGShot.h"
#include "Templates/Weapons/Projectiles/Stake.h"

#include "PKClasses/DamageTypes/LavaDamage.h"

#include "Templates/Processes/BlendColor.h"

#include "Misc/DefaultValueHelper.h"

// Sets default values
ANetPlayer::ANetPlayer(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UPKCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f); // radius, half height

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>mesh(TEXT("SkeletalMesh'/Game/Models/Player/mp-model-fallenangel.mp-model-fallenangel'"));
	GetMesh()->SetSkeletalMesh(mesh.Object);
	GetMesh()->RelativeLocation = FVector(0, 0, -95.f);
	GetMesh()->RelativeRotation = FRotator(0, -90, 0);
	static FName CollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionProfileName(CollisionProfileName);

	PKW_head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PKWHead"));
	FName fnWeaponSocket = TEXT("pkheadSocket");
	PKW_head->AttachTo(GetMesh(), fnWeaponSocket, EAttachLocation::SnapToTarget, true);
	GetMesh()->SetOwnerNoSee(true);
	PKW_head->SetOwnerNoSee(true);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(20, 0, 74.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->FieldOfView = 110.f;
	
	CharacterMovement = Cast<UPKCharacterMovementComponent>(GetCharacterMovement());
	if (CharacterMovement)
	{
		CharacterMovement->UpdatedComponent = GetCapsuleComponent();
	}

	FName WeaponBonesInit[] = {
		TEXT("weap1"),
		TEXT("weap2"),
		TEXT("weap3"),
		TEXT("weap4"),
		TEXT("weap5"),
		TEXT("weap6"),
		TEXT("weap7")
	};
	WeaponBones.Append(WeaponBonesInit, ARRAY_COUNT(WeaponBonesInit));
	
	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(5.0f, 0.0f, 0.0f);

	// workaround
	ProjectileClass = APKProjectile::StaticClass();	

	// setup start weapon
	FURL URL;
	URL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);
	const TCHAR* c = URL.GetOption(TEXT("StartupWeapon="), TEXT("3")); // StakeGunGL by default
	int32 val;
	FDefaultValueHelper::ParseInt(c, val);
	CurWeaponIndex = (uint8)val;
	OldCurWeaponIndex = CurWeaponIndex;
	
	if (!PainKiller){
		PainKiller = CreateDefaultSubobject<UChildActorComponent>(TEXT("PainKiller"));
		PainKiller->AttachParent = FirstPersonCameraComponent;
	}
	if (!Shotgun){
		Shotgun = CreateDefaultSubobject<UChildActorComponent>(TEXT("Shotgun"));
		Shotgun->AttachParent = FirstPersonCameraComponent;
	}
	if (!StakeGunGL){
		StakeGunGL = CreateDefaultSubobject<UChildActorComponent>(TEXT("StakeGunGL"));
		StakeGunGL->AttachParent = FirstPersonCameraComponent;
	}
	if (!MiniGunRL){
		MiniGunRL = CreateDefaultSubobject<UChildActorComponent>(TEXT("MiniGunRL"));
		MiniGunRL->AttachParent = FirstPersonCameraComponent;
	}
	if (!DriverElectro){
		DriverElectro = CreateDefaultSubobject<UChildActorComponent>(TEXT("DriverElectro"));
		DriverElectro->AttachParent = FirstPersonCameraComponent;
	}
	if (!RifleFlameThrower){
		RifleFlameThrower = CreateDefaultSubobject<UChildActorComponent>(TEXT("RifleFlameThrower"));
		RifleFlameThrower->AttachParent = FirstPersonCameraComponent;
	}
	if (!BoltGunHeater){
		BoltGunHeater = CreateDefaultSubobject<UChildActorComponent>(TEXT("BoltGunHeater"));
		BoltGunHeater->AttachParent = FirstPersonCameraComponent;
	}
		
	Weapons.Add(PainKiller);
	Weapons.Add(Shotgun);
	Weapons.Add(StakeGunGL);
	Weapons.Add(MiniGunRL);
	Weapons.Add(DriverElectro);
	Weapons.Add(RifleFlameThrower);
	Weapons.Add(BoltGunHeater);
	
	NetUpdateFrequency = 30.f;
	OnTakeAnyDamageDeligate.BindUFunction(this, FName("TakeAnyDamage"));
	OnTakeAnyDamage.Add(OnTakeAnyDamageDeligate);
	OnTakePointDamageDeligate.BindUFunction(this, FName("TakePointDamage"));
	OnTakePointDamage.Add(OnTakePointDamageDeligate);
	OnEndPlayDeligate.BindUFunction(this, FName("CustomEndPlay"));
	OnEndPlay.Add(OnEndPlayDeligate);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/Particles/shotgunhitwall.shotgunhitwall"));
	ShotgunHitWallParticle = Particle.Object;

	PlayerHurtAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlayerHurtAudioComp"));
	PlayerHurtAudioComponent->AttachParent = GetRootComponent();
	PlayerHurtAudioComponent->bAutoActivate = false;
	
	static ConstructorHelpers::FObjectFinder<USoundCue>HurtSmallCue(TEXT("SoundCue'/Game/Sounds/multiplayer/player/Cue/HurtSmall.HurtSmall'"));
	HurtSmallSound = HurtSmallCue.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue>HurtMediumCue(TEXT("SoundCue'/Game/Sounds/multiplayer/player/Cue/HurtMedium.HurtMedium'"));
	HurtMediumSound = HurtMediumCue.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue>HurtBigCue(TEXT("SoundCue'/Game/Sounds/multiplayer/player/Cue/HurtBig.HurtBig'"));
	HurtBigSound = HurtBigCue.Object;

	LavaHurtSnd = UTIL.GetSound("actor/evilmonkv3", "evil-fire-hit");
}


// Called when the game starts or when spawned
void ANetPlayer::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	// client hears other client or server spawn
	if (Role == ROLE_SimulatedProxy)
	{
		UGameplayStatics::PlaySoundAtLocation(this, UTIL.GetSound("specials/respawns", FString::Printf(TEXT("respawn_m%d"), FMath::RandRange(1, 6))), GetActorLocation());
	}	

	SelectBestWeapon(0x01 << (CurWeaponIndex - 1));
	
	if (HasAuthority())
	{
		FURL URL;
		URL.LoadURLConfig(TEXT("GameSettings"), GGameIni);
		const TCHAR* c = URL.GetOption(TEXT("FallingDamage="), TEXT("0")); // off by default
		FDefaultValueHelper::ParseInt(c, bFallingDamage);
	}
}

// Called every frame
void ANetPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/************************************
		OTHER STUFF
	************************************/
	if (!PC && Controller)
	{
		PC = Cast<ANetPlayerController>(Controller);

		// listen server hears client spawn
		if (!IsLocallyControlled())
		{
			UGameplayStatics::PlaySoundAtLocation(this, UTIL.GetSound("specials/respawns", FString::Printf(TEXT("respawn_m%d"), FMath::RandRange(1, 6))), GetActorLocation());
		}
	}

	/*Demon*/
	CalcWarp(DeltaTime);

	if (GroundSndDelay > 0) GroundSndDelay -= DeltaTime;

	if (JumpSndDelay > 0) JumpSndDelay -= DeltaTime;

	if (HurtSoundTimeOut > 0) HurtSoundTimeOut -= DeltaTime;

	// LAVA
	if (!GetMesh()->IsAnySimulatingPhysics() && LavaDamage){
		if (LavaHurtDelay > 0) LavaHurtDelay -= DeltaTime;
		else{
			if (Role == ROLE_Authority && Controller) UGameplayStatics::ApplyDamage(this, LavaDamage, Controller, this, ULavaDamage::StaticClass());
			if (LavaHurtSnd) UGameplayStatics::PlaySoundAttached(LavaHurtSnd, GetRootComponent());
			LavaHurtDelay = 0.33f;
		}
	}
	else LavaHurtDelay = 0.f;
}

FLinearColor ANetPlayer::BlendColor(FLinearColor A, FLinearColor B, float blend)
{
	blend = FMath::Clamp(blend, 0.f, 1.f);

	return FLinearColor
		(
		A.R - (A.R - B.R) * blend,
		A.G - (A.G - B.G) * blend,
		A.B - (A.B - B.B) * blend,
		A.A - (A.A - B.A) * blend
		);
}

// Called to bind functionality to input
void ANetPlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent){

	Super::SetupPlayerInputComponent(InputComponent);

	check(InputComponent);
	
	InputComponent->BindAxis("MoveForward", this, &ANetPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ANetPlayer::MoveRight);

	InputComponent->BindAxis("Turn", this, &ANetPlayer::Turn);
	InputComponent->BindAxis("LookUp", this, &ANetPlayer::LookUp);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ANetPlayer::OnFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ANetPlayer::OnFireRelease);

	InputComponent->BindAction("AltFire", IE_Pressed, this, &ANetPlayer::OnAltFire);
	InputComponent->BindAction("AltFire", IE_Released, this, &ANetPlayer::OnAltFireRelease);

	InputComponent->BindAction("Jump", IE_Pressed, CharacterMovement, &UPKCharacterMovementComponent::Jump);

	InputComponent->BindAction("Pause", IE_Pressed, this, &ANetPlayer::OnPause);
	InputComponent->BindAction("Disconnect", IE_Pressed, this, &ANetPlayer::OnDisconnect);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ANetPlayer::NextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ANetPlayer::PrevWeapon);

	InputComponent->BindAction("BestWeapon", IE_Pressed, this, &ANetPlayer::BestWeaponFire);
	InputComponent->BindAction("BestWeapon", IE_Released, this, &ANetPlayer::BestWeaponStopFire);

	InputComponent->BindAction("NetPlayerIndex", IE_Pressed, this, &ANetPlayer::OnGetNetPlayerIndex_Implementation);

	InputComponent->BindAction("ShowStats", IE_Pressed, this, &ANetPlayer::ShowStats);
	InputComponent->BindAction("ShowStats", IE_Released, this, &ANetPlayer::HideStats);

	InputComponent->BindAction("ServerTravel", IE_Pressed, this, &ANetPlayer::ServerTravel);

	InputComponent->BindAction("Weapon1", IE_Pressed, this, &ANetPlayer::SelectWeap1);
	InputComponent->BindAction("Weapon2", IE_Pressed, this, &ANetPlayer::SelectWeap2);
	InputComponent->BindAction("Weapon3", IE_Pressed, this, &ANetPlayer::SelectWeap3);
	InputComponent->BindAction("Weapon4", IE_Pressed, this, &ANetPlayer::SelectWeap4);
}

void ANetPlayer::ServerTravel()
{
	if (!HasAuthority())return;

	UWorld* World = GetWorld();

	if (World != NULL)
	{
		Cast<UPKGameInstance>(World->GetGameInstance())->LoadNextMap();
	}
}

void ANetPlayer::ShowStats()
{	
	if(PC) PC->ShowStats();
}

void ANetPlayer::HideStats()
{	
	if (PC && UTIL.GetCurrentInputMode(PC) != EInputMode::UIOnly)
	{
		UTIL.ShowWidgetClassOf(this, nullptr);
	}
}

void ANetPlayer::DisplayCorrectWeapon()
{
	if (bBlockDisplayCorrectWeapon) return;

	if (CurWeaponIndex > Weapons.Num())	CurWeaponIndex = Weapons.Num();
	if (CurWeaponIndex < 1) CurWeaponIndex = 1;

	// 3rd person view
	USkeletalMeshComponent* mesh = GetMesh();	
	if (mesh){
		for (auto name : WeaponBones){
			mesh->HideBoneByName(name, PBO_None);
		}
		if (!GetMesh()->IsAnySimulatingPhysics()) // if not a ragdoll
		{
			mesh->UnHideBoneByName(WeaponBones[CurWeaponIndex - 1]);
			PKW_head->SetVisibility(CurWeaponIndex == 1 && !bPKHeadHide);
		}
	}
	
	// 1st person view
	if (Weapons.Num() < 1){
		if (dummyweap) cw = dummyweap;
		return;
	}

	AActor* child;
	for (auto weap : Weapons){
		if (weap && weap->ChildActor){
			child = weap->ChildActor;
			child->SetActorHiddenInGame(true);			
			if (child->IsA(APKWeapon::StaticClass()))
				(Cast<APKWeapon>(child))->GetWeaponMesh()->SetVisibility(false);
		}
	}	
	
	if (Weapons[CurWeaponIndex - 1] && Weapons[CurWeaponIndex - 1]->ChildActor){
		child = Weapons[CurWeaponIndex - 1]->ChildActor;
		child->SetActorHiddenInGame(false);
		if (child->IsA(APKWeapon::StaticClass())){
			cw = Cast<APKWeapon>(child);
			cw->GetWeaponMesh()->SetVisibility(true);
		}
		else
		{
			if (dummyweap){
				cw = dummyweap;
				UTIL.AddMessage(L"wrong class at weapon slot: %d", CurWeaponIndex);
			}
		}

	}
	else{
		if (dummyweap){
			cw = dummyweap;
			UTIL.AddMessage(L"empty weapon slot: %d", CurWeaponIndex);
		}
	}

	UGameplayStatics::PlaySoundAttached(UTIL.GetSound("misc", "weapon-hide"), RootComponent);
	UGameplayStatics::PlaySoundAttached(UTIL.GetSound("misc", "weapon-show"), RootComponent);
}

void ANetPlayer::ReceiveDestroyed()
{
	
	if (dummyweap != nullptr
		&& dummyweap->IsValidLowLevel()
		&& !dummyweap->IsPendingKill()
		)
	{
		dummyweap->Destroy();
	}
}

void ANetPlayer::UnPossessed()
{
	Super::UnPossessed();

	// force stop firing
	cw->OnChangeWeapon()/*OnOwnerUnPossessed()*/;

	if (PainHead != NULL)
	{
		PainHead->Deactivate();
	}

	if (MinigunAltFire != NULL)
	{
		MinigunAltFire->Deactivate();
	}

	if (PainKillerFire != NULL)
	{
		PainKillerFire->Deactivate();
	}
}

void ANetPlayer::CustomEndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PainHead != NULL)
	{
		PainHead->Destroy();
	}

	if (MinigunAltFire != NULL)
	{
		MinigunAltFire->Destroy();
	}

	if (PainKillerFire != NULL)
	{
		PainKillerFire->Destroy();
	}
}

void ANetPlayer::NextPrevWeapon(bool next)
{
	if (GetWorldTimerManager().IsTimerActive(WeapChangeTimerHandle) || LastWeaponIndex >= 0) return;
	GetWorldTimerManager().SetTimer(WeapChangeTimerHandle, 0.25f, false);

	uint8 WeapSlot = CurWeaponIndex;
	if (next){
		while (true){
			WeapSlot++;
			if (WeapSlot > Weapons.Num())WeapSlot = 1;
			if (AvailableWeapons & (0x01 << (WeapSlot-1))) break;
		}
	}
	else{
		while (true){
			WeapSlot--;
			if (WeapSlot < 1)WeapSlot = Weapons.Num();
			if (AvailableWeapons & (0x01 << (WeapSlot-1))) break;
		}
	}

	ChangeWeapon(WeapSlot);
}

void ANetPlayer::ChangeWeapon(uint8 NewSlot)
{
	if (CurWeaponIndex != NewSlot)
	{
		cw->OnChangeWeapon();

		// lagging simulated proxy & server
		SlotUpdate(NewSlot);

		// instant autonomous proxy
		if (Role == ROLE_AutonomousProxy)
		{
			CurWeaponIndex = NewSlot;
			DisplayCorrectWeapon();
		}
	}
}

void ANetPlayer::OnPause()
{
	UPKGameInstance* gameInstance = Cast<UPKGameInstance>(GetGameInstance());
	gameInstance->LeaveGame();
}

void ANetPlayer::OnDisconnect()
{
	UPKGameInstance* gameInstance = Cast<UPKGameInstance>(GetGameInstance());
	gameInstance->/*GoToMainMenu*/OnEscape();
}

void ANetPlayer::CheckJumpInput(float DeltaTime)
{
	if (bPressedJump)
	{
		// Increment our timer first so calls to IsJumpProvidingForce() will return true
		/*JumpKeyHoldTime += DeltaTime;*/
		const bool bDidJump = CanJump() && CharacterMovement && CharacterMovement->DoJump(bClientUpdating);
		if (/*!bWasJumping && */bDidJump)
		{
			OnJumped();
			if (HasAuthority() && IsLocallyControlled()) bPressedJump = false;
		}
	}	
}

void ANetPlayer::Restart()
{
	Super::Restart();
}

void ANetPlayer::ClearJumpInput()
{
	/*Super::ClearJumpInput();*/
}

void ANetPlayer::MoveForward(float Value)
{
	CharacterMovement->MoveForward(Value);
}

void ANetPlayer::MoveRight(float Value)
{
	CharacterMovement->MoveRight(Value);
}

void ANetPlayer::PendingJump()
{
	CharacterMovement->Jump();
}

void ANetPlayer::SpawnProjectile_Implementation(TSubclassOf<class APKProjectile>  ProjectileCls)
{
	int8 AmmoType = -1;
	float ProjectileSpeed = 0;

	if (ProjectileCls != NULL)
	{
		AmmoType = ProjectileCls.GetDefaultObject()->AmmoType;
		PC->ConsumeAmmo(AmmoType);

		if (AmmoType & 0x10 && MinigunAltFire != NULL && MinigunAltFire->bIsActive) MinigunAltFire->Deactivate();
		if (AmmoType & 0x80 && PainKillerFire != NULL && PainKillerFire->bIsActive) PainKillerFire->Deactivate();
	}

	FVector _GunOffset = GunOffset;
	FRotator SpawnRotation = GetControlRotation();
	FVector Loc = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector FV = SpawnRotation.RotateVector(FVector::ForwardVector);
	
	if (CurWeaponIndex == 3/* || (AmmoType & 0x80)*/) {
		Loc.Z -= 16.8f; // PK - 0.2
		_GunOffset *= 12.0;
		if (AmmoType & 0x01){ // stake
			SpawnRotation = ((FV + FVector(0, 0, 0.05)).GetSafeNormal()).Rotation();
			FV = SpawnRotation.RotateVector(FVector::ForwardVector);
		}
	}
	
	if (AmmoType & 0x08) Loc.Z -= 16.8f; // icebullet

	const FVector SpawnLocation = Loc + SpawnRotation.RotateVector(_GunOffset);
	
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		// spawn the projectile at the muzzle
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Instigator = this;
		SpawnParameters.Owner = this;
		
		APKProjectile* projectile = Cast<APKProjectile>(
			World->SpawnActorDeferred<APKProjectile>(ProjectileCls, SpawnLocation, SpawnRotation, this, this)
		);
		switch (ProjectileCls.GetDefaultObject()->AmmoType){
		case 0x00:
			PainHead = projectile;
			HideThirdPersonPainHead(true);
			break;
		case 0x02: // grenade
			if (CurWeaponIndex == 3){
				
			}
			break;
		case 0x10:
			MinigunAltFire = projectile/*Cast<APKProjectile>(projectile)*/;
			break;
		case 0x80:
			PainKillerFire = projectile/*Cast<APKProjectile>(projectile)*/;
			break;
		}

		FTransform Transform = FTransform(SpawnRotation, SpawnLocation);
		UGameplayStatics::FinishSpawningActor(projectile, Transform);
	}
}

uint16 ANetPlayer::GetPlayerFlag()
{
	uint16 FLAG = 0x0000;
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		APKGameState* GameState = Cast<APKGameState>(World->GetGameState());
		if (GameState && PC && PC->PlayerState) // check for PC->PlayerState to avoid crash on client disconnect
		{
			FLAG = 0x0001 << GameState->GetIdxFromPlayerStates(PC->PlayerState);
		}
	}	
	return FLAG;
}

void ANetPlayer::MinigunHitScan()
{
	const FRotator SpawnRotation = GetControlRotation();
	FVector Loc = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector SpawnLocation = Loc + SpawnRotation.RotateVector(GunOffset);

	float MinigunFireRange = 100.f * 84.f;
	const FVector End = SpawnLocation + MinigunFireRange * SpawnRotation.RotateVector(FVector::ForwardVector);

	FHitResult Hit = FHitResult(1.f);
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	
	GetWorld()->LineTraceSingle(Hit, SpawnLocation, End, ECollisionChannel::ECC_Visibility, TraceParams);

	if (Hit.bBlockingHit)
	{
		if (Hit.GetActor()->IsA(ANetPlayer::StaticClass()))
		{
			if (MinigunLastHitEnemy != Hit.Actor)
			{
				ANetPlayer* Enemy = Cast<ANetPlayer>(Hit.GetActor());
				Enemy->MinigunAttackingPlayerFlags |= ClearMinigunLastHitEnemy();
				MinigunLastHitEnemy = Hit.Actor;
			}		
		}
		else ClearMinigunLastHitEnemy();
	}
	else ClearMinigunLastHitEnemy();
}

uint16 ANetPlayer::ClearMinigunLastHitEnemy()
{
	uint16 Flag = GetPlayerFlag();
	ANetPlayer* LastEnemy = Cast<ANetPlayer>(MinigunLastHitEnemy.Get());
	if (LastEnemy)
	{
		LastEnemy->MinigunAttackingPlayerFlags = (LastEnemy->MinigunAttackingPlayerFlags ^ Flag);
		MinigunLastHitEnemy = NULL;
	}
	return Flag;
}

void ANetPlayer::ShotgunFire_Implementation()
{
	ShotgunFireFX();
}

void ANetPlayer::ShotgunFireFX_Implementation()
{
	const FRotator SpawnRotation = PC ? GetControlRotation() : GetBaseAimRotation();
	FVector Loc = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector SpawnLocation = Loc + SpawnRotation.RotateVector(GunOffset);

	float FireRange = 50.f * 84.f;
	uint8 HowManyPellets = 24;
	float Y; float Z; float FireRadius = 4.f * 84.f;
	TMap<uint16, ANetPlayer*> Enemies;

	for (int i = 0; i < HowManyPellets; i++)
	{
		UTIL.RadiusRandom2D(FireRadius, Y, Z);
		FVector ForwardVector = FVector(1.f * FireRange, Y, Z);
		FVector End = SpawnLocation + FireRange * SpawnRotation.RotateVector(ForwardVector);

		FHitResult Hit = FHitResult(1.f);
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);

		GetWorld()->LineTraceSingle(Hit, SpawnLocation, End, ECollisionChannel::ECC_Visibility, TraceParams);

		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor()->IsA(ANetPlayer::StaticClass()))
			{
				if (Role == ROLE_Authority)
				{					
					ANetPlayer* Enemy = Cast<ANetPlayer>(Hit.GetActor());
					uint16 Flag = Enemy->GetPlayerFlag();
					
					if (Enemies.Contains(Flag))
					{
						(*(Enemies.Find(Flag)))->Pellets++;
					}
					else{
						Enemy->Pellets++;
						Enemies.Add(Flag, Enemy);
					}
				}
			}
			else{
				UGameplayStatics::SpawnEmitterAtLocation(this, ShotgunHitWallParticle, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), true);
			}
		}
	}
	
	if (Role == ROLE_Authority)
	{
		for (uint8 idx = 0; idx < 16; idx++) // 16 players max
		{
			uint16 Flag = 0x0001 << idx;
			if (Enemies.Contains(Flag))
			{
				ANetPlayer* Enemy = *Enemies.Find(Flag);
				UPKCharacterMovementComponent* CharacterMovement = Cast<UPKCharacterMovementComponent>(Enemy->GetCharacterMovement());
				CharacterMovement->ReceiveImpactRPC(Enemy->Pellets * 100.f * GetControlRotation().RotateVector(FVector::ForwardVector));
				Enemy->Pellets = 0;
			}
		}
	}
}

/*********************************************
	Damage
*********************************************/

void ANetPlayer::TakeAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// TODO: move message handling to GameState
	
	if (DamageType->IsA(UPKDamageType::StaticClass()))
	{

		const class UPKDamageType* DmgType = Cast<UPKDamageType>(DamageType);

		if (PC)
		{
			if (DmgType->CausedBy == EDamageCause::icebullet) PC->FreezeArmor();

			ANetPlayerController* Instigator = Cast<ANetPlayerController>(InstigatedBy);

			if (Instigator && Instigator == PC)
			{
				bool bEnvironmentalDamage = 
					DmgType->CausedBy == EDamageCause::outofworld ||
					DmgType->CausedBy == EDamageCause::gravity ||
					DmgType->CausedBy == EDamageCause::lava;

				if (!bEnvironmentalDamage)	Damage *= 0.5f; // reduce damage to the owner
			}

			if (PC->OnDamage(Damage))
			{
				/*update score*/
				if (InstigatedBy && InstigatedBy != PC && InstigatedBy->PlayerState) 
					Cast<APKPlayerState>(InstigatedBy->PlayerState)->FragUp();
				if (PC->PlayerState)
					Cast<APKPlayerState>(PC->PlayerState)->FragDown(InstigatedBy == PC);

				UWorld* const World = GetWorld();
				if (World != NULL)
				{
					/*Spawn Soul*/
					FVector Loc = GetCapsuleComponent()->GetComponentLocation();
					FRotator Rot = FRotator(0, GetControlRotation().Yaw, 0);

					FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
					SpawnParameters.Owner = this;

					ASoulMP* SoulMP = World->SpawnActor<ASoulMP>(ASoulMP::StaticClass(), Loc, Rot, SpawnParameters);

					/*Whose frag message*/
					Cast<APKGameState>(World->GetGameState())->SayToAllWhoseFrag(DamageType, PC, InstigatedBy, DamageCauser);
				}

				if (Instigator && Instigator != PC) Instigator->PlayKillSound();

				/*Death cry*/				
				HurtSoundFlags = HurtSoundFlags ^ DeathFlag; // notify clients
				OnRep_SoundFlags(); // server side execution

				bLocallyControlled = IsLocallyControlled();
				PC->SetupOnRagdollView();
				GetWorldTimerManager().SetTimer(TimerHandle, this, &ANetPlayer::DestroyAfterRagdoll, RagdollLifeSpan, false);

				FVector BackOffset = DamageCauser->GetActorRotation().RotateVector(FVector(-200.0f, 0.0f, 0.0f));

				FVector ShotFromDirection = (GetActorLocation() - (DamageCauser->GetActorLocation() + BackOffset)).GetSafeNormal();
				DamageImpulse = ShotFromDirection.IsZero() ? FVector(0, 0, -500 * CharacterMovement->Mass) : 1000.f * ShotFromDirection * CharacterMovement->Mass;
				OnRep_DamageImpulse(); // server side
			}
			else if (DmgType->CausedBy != EDamageCause::gravity && DmgType->CausedBy != EDamageCause::icebullet){

				if (Instigator) // controller
				{
					bool bSpecificHit =
						DmgType->CausedBy == EDamageCause::minigun ||
						DmgType->CausedBy == EDamageCause::painbeam ||
						DmgType->CausedBy == EDamageCause::painrotor;

					if (!(bSpecificHit || Instigator == PC)) Instigator->PlayHitSound();
				}

				if (HurtSoundTimeOut <= 0)
				{
					if (Damage >= 50)
					{
						HurtSoundFlags = HurtSoundFlags ^ HurtBig;		// notify clients
						OnRep_SoundFlags();								// server side execution
					}
					else if (Damage >= 20)
					{
						HurtSoundFlags = HurtSoundFlags ^ HurtMedium;	// notify clients
						OnRep_SoundFlags();								// server side execution
					}
					else if (Damage > 0)
					{
						HurtSoundFlags = HurtSoundFlags ^ HurtSmall;	// notify clients
						OnRep_SoundFlags();								// server side execution
					}
					HurtSoundTimeOut = 0.25f;
				}

			}
		}
	}
}

void ANetPlayer::TakePointDamage(
	float Damage,
	class AController* InstigatedBy, 
	FVector HitLocation, 
	class UPrimitiveComponent* FHitComponent, 
	FName BoneName,
	FVector ShotFromDirection, 
	const class UDamageType* DamageType, 
	AActor* DamageCauser
	)
{
	
}

void ANetPlayer::RagdollDisappear()
{
	UBlendColor* BlendColorObj = NewObject<UBlendColor>(this);
	BlendColorObj->Init(PlayerColor, FLinearColor(0.f, 0.05, 0.1, 0.f), RagdollLifeSpan * 0.15f);
}

void ANetPlayer::OnRep_DamageImpulse()
{
	UBlendColor* BlendColorObj = NewObject<UBlendColor>(this);
	BlendColorObj->Init(PlayerColor, FLinearColor(0.f, 0.05, 0.1, 1.f), 2.f);

	GetWorldTimerManager().SetTimer(DisappearTimerHandle, this, &ANetPlayer::RagdollDisappear, RagdollLifeSpan * 0.8f , false);
	
	/**
	ISSUE: Server ignores third person pawn velocity on SetSimulatePhysics.
	WORKAROUND: Forcibly apply velocity
	*/
	FVector Impulse = DamageImpulse;
	FVector Vel = CharacterMovement->Velocity * CharacterMovement->Mass;
	if (HasAuthority() && !bLocallyControlled) Impulse += Vel;

	CharacterMovement->Deactivate();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PKW_head->SetVisibility(false);
	GetMesh()->HideBoneByName(WeaponBones[CurWeaponIndex - 1], PBO_None);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->SetSimulatePhysics(true); // check source
	
	GetMesh()->AddImpulse(Impulse);
}

void ANetPlayer::Landed(const FHitResult& Hit)
{
	// NOTE: fired up on Authority & Autonomous

	Super::Landed(Hit);

	float speed = GetVelocity().Z;

	if (speed < -collisionMinSpeed && bFallingDamage)
	{
		speed = FMath::Abs(speed);
		float d = 100.f;

		if (speed < collisionMaxSpeed)
		{
			d = ((speed - collisionMinSpeed) / (collisionMaxSpeed - collisionMinSpeed)) * d * 0.75f;
		}

		if (Role == ROLE_Authority)
		{
			UDamageType const* const DmgType = GetDefault<UGravityDamage>();
			UGameplayStatics::ApplyDamage(this, d, PC, this, DmgType->GetClass());
		}

		// flip the bit
		JumpSoundFlags = JumpSoundFlags ^ JumpEndHurt;
		OnRep_SoundFlags(); // locally
	}
	else{
		float minspd = -12.f * 84;
		if (GroundSndDelay <= 0 && speed < minspd)
		{
			// flip the bit
			JumpSoundFlags = JumpSoundFlags ^ JumpEnd;
			OnRep_SoundFlags(); // locally

			GroundSndDelay = 0.7f;
		}
	}
}

void ANetPlayer::OnStartJumpAnim()
{
	UWorld* const World = GetWorld();

	if (World && JumpSndDelay <= 0.f)
	{
		UGameplayStatics::PlaySoundAttached(
			UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_jump_%d"), FMath::RandRange(1, 2)))
			, RootComponent);
		JumpSndDelay = CharacterMovement->JumpZVelocity / -World->GetGravityZ();
	}
}

void ANetPlayer::OnRep_HurtSmall()
{
	UGameplayStatics::PlaySoundAttached(
		UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_hurt_small%d"), FMath::RandRange(1, 5)))
		, RootComponent);
}


void ANetPlayer::OnRep_HurtMedium()
{
	UGameplayStatics::PlaySoundAttached(
		UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_hurt_medium%d"), FMath::RandRange(1, 4)))
		, RootComponent);
}

void ANetPlayer::OnRep_HurtBig()
{
	UGameplayStatics::PlaySoundAttached(
		UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_hurt_big%d"), FMath::RandRange(1, 3)))
		, RootComponent);
}

void ANetPlayer::OnRep_Death()
{
	USceneComponent* comp = GetMesh() ? GetMesh() : RootComponent;

	UGameplayStatics::PlaySoundAttached(
		UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_death%d"), FMath::RandRange(1, 7)))
		, comp);
}

void ANetPlayer::OnRep_SoundFlags()
{
	// JUMP
	if ((LastJumpSoundFlags & JumpEnd) ^ (JumpSoundFlags & JumpEnd))
	{
		UGameplayStatics::PlaySoundAttached(
			UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer-jump_end%d"), FMath::RandRange(1, 2)))
			, RootComponent);
	}

	if ((LastJumpSoundFlags & JumpEndHurt) ^ (JumpSoundFlags & JumpEndHurt))
	{
		float pitch = 0.01f * FMath::RandRange(95, 100);
		UGameplayStatics::PlaySoundAttached(
			UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_jump_end-hurt%d"), FMath::RandRange(1, 1)))
			, RootComponent, NAME_None, ((FVector)(ForceInit)), EAttachLocation::KeepRelativeOffset, false, 1.0f, pitch);
	}

	LastJumpSoundFlags = JumpSoundFlags;
	
	// HURT / DEATH
	if ((LastHurtSoundFlags & HurtSmall) ^ (HurtSoundFlags & HurtSmall))
	{
		UGameplayStatics::PlaySoundAttached(
			UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_hurt_small%d"), FMath::RandRange(1, 5)))
			, RootComponent);
	}

	if ((LastHurtSoundFlags & HurtMedium) ^ (HurtSoundFlags & HurtMedium))
	{
		UGameplayStatics::PlaySoundAttached(
			UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_hurt_medium%d"), FMath::RandRange(1, 4)))
			, RootComponent);
	}

	if ((LastHurtSoundFlags & HurtBig) ^ (HurtSoundFlags & HurtBig))
	{
		UGameplayStatics::PlaySoundAttached(
			UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_hurt_big%d"), FMath::RandRange(1, 3)))
			, RootComponent);
	}

	if ((LastHurtSoundFlags & DeathFlag) ^ (HurtSoundFlags & DeathFlag))
	{
		USceneComponent* comp = GetMesh() ? GetMesh() : RootComponent;

		UGameplayStatics::PlaySoundAttached(
			UTIL.GetSound("multiplayer/player", FString::Printf(TEXT("manplayer_death%d"), FMath::RandRange(1, 7)))
			, comp);
	}

	LastHurtSoundFlags = HurtSoundFlags;
}

void ANetPlayer::OnRep_AppearFrozen()
{
	if (bAppearFrozen)
	{
		UBlendColor* BlendColorObj = NewObject<UBlendColor>(this);
		BlendColorObj->Init(PlayerColor, FrozenColor, 0.25f);
	}
	else if (!GetMesh()->IsAnySimulatingPhysics()){ // change appearance only if alive
		UBlendColor* BlendColorObj = NewObject<UBlendColor>(this);
		BlendColorObj->Init(PlayerColor, DefaultColor, 0.25f);
	}
}

void ANetPlayer::QuickWeaponSelection(uint8 WeapSlot)
{
	if (AvailableWeapons & (0x01 << (WeapSlot - 1)))
	{
		if (GetWorldTimerManager().IsTimerActive(WeapChangeTimerHandle)) return;
		if (CurWeaponIndex != WeapSlot) GetWorldTimerManager().SetTimer(WeapChangeTimerHandle, 0.25f, false);

		ChangeWeapon(WeapSlot);
	}
}

void ANetPlayer::BestWeaponFire()
{
	GetWorldTimerManager().ClearTimer(BestWeaponTimerHandle);
	uint8 WeapSlot = 3; // hardcoded stakegun
	if (AvailableWeapons & (0x01 << (WeapSlot - 1)))
	{
		if (LastWeaponIndex < 0){
			LastWeaponIndex = CurWeaponIndex;
			ChangeWeapon(WeapSlot);
		}
		cw->AltFire();
	}
}

void ANetPlayer::BestWeaponStopFire()
{
	cw->OnFinishAltFire();
	GetWorldTimerManager().SetTimer(BestWeaponTimerHandle, this, &ANetPlayer::RestoreAfterBestWeapon, 0.25, false);
}

void ANetPlayer::RestoreAfterBestWeapon()
{
	ChangeWeapon(LastWeaponIndex);
	LastWeaponIndex = -1;
}

void ANetPlayer::SelectBestWeapon_Implementation(uint8 weap)
{
	AvailableWeapons |= weap;
	uint8 WeapSlot = 0;
	while (weap)
	{
		weap = weap >> 1; WeapSlot++;
	}
	if (WeapSlot > CurWeaponIndex) ChangeWeapon(WeapSlot);
}

void ANetPlayer::CalcWarp(float DeltaTime)
{
	if (WarpTime < 4.0f){
		float k = 16;
		DemonWarp = FMath::Sin(k * WarpTime) / (1 + k * FMath::Pow(WarpTime, 6));
		WarpTime += DeltaTime;
	}
}

void ANetPlayer::FellOutOfWorld(const class UDamageType& dmgType)
{
	// Avoid repeated damage
	if (bFellOutOfWorld) return;
	bFellOutOfWorld = true;
	
	UDamageType const* const DmgType = GetDefault<UOutOfWorldDamage>();

	UGameplayStatics::ApplyDamage(this, 1000, PC, this, DmgType->GetClass());
}

void ANetPlayer::OnGetNetPlayerIndex_Implementation()
{	
	TArray<ANetPlayer*> Pawns;

	UTIL.FindAllObjectsClassOf(Pawns);
	for (auto pawn : Pawns)
	{
		if (pawn->PlayerState)
			UTIL.AddMessage(L"Player %s", *pawn->GetName(), 0, 5);
	}
}

void ANetPlayer::OnRep_SetCurWeaponIndex()
{
	// simulated proxy (COND_SimulatedOnly)
	CurWeaponIndex = slot;
	DisplayCorrectWeapon();
}

void ANetPlayer::OnRep_PainHeadShot()
{
	if (GetMesh()->IsAnySimulatingPhysics()) return;
	PKW_head->SetVisibility(CurWeaponIndex == 1 && !bPKHeadHide);
}

/************************************************************
		Replication List
************************************************************/
void ANetPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Conditional replication //ELifetimeCondition
	DOREPLIFETIME_CONDITION(ANetPlayer, slot, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ANetPlayer, bPKHeadHide, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ANetPlayer, JumpSoundFlags, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ANetPlayer, bAppearFrozen, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ANetPlayer, MinigunAttackingPlayerFlags, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayer, bFallingDamage, COND_OwnerOnly);
	
	// Replicate to everyone
	DOREPLIFETIME(ANetPlayer, bPKHeadOpen);
	DOREPLIFETIME(ANetPlayer, DamageImpulse);
	DOREPLIFETIME(ANetPlayer, HurtSoundFlags);

	DOREPLIFETIME(ANetPlayer, bHurtSmall);
	DOREPLIFETIME(ANetPlayer, bHurtMedium);
	DOREPLIFETIME(ANetPlayer, bHurtBig);
	DOREPLIFETIME(ANetPlayer, bDeath);
}