// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "NetPlayerController.h"
#include "PKClasses/WarmUpView.h"
#include "PKClasses/PickableItem.h"
#include "PKGameState.h"
#include "PKPlayerState.h"
#include "NetPlayer.h"
#include "UnrealNetwork.h"


ANetPlayerController::ANetPlayerController(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	DisableInput(this);

	static ConstructorHelpers::FObjectFinder<UClass>ScoreboardWidget(TEXT("Class'/Game/HUD/ScoreBoard.Scoreboard_C'"));
	ScoreBoardWidgetClass = (UClass*)ScoreboardWidget.Object;
	static ConstructorHelpers::FObjectFinder<UClass>ChatWidget(TEXT("Class'/Game/HUD/Chat.Chat_C'"));
	ChatWidgetClass = (UClass*)ChatWidget.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>killsound(TEXT("SoundWave'/Game/Sounds/multiplayer/Hitsounds/killsoundnew.killsoundnew'"));
	KillSound = killsound.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> hitsound(TEXT("SoundWave'/Game/Sounds/multiplayer/Hitsounds/hitsoundnew.hitsoundnew'"));
	HitSound = hitsound.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> PlayerJoinedSound(TEXT("SoundWave'/Game/Sounds/multiplayer/newplayerjoinedserver.newplayerjoinedserver'"));
	NewPlayerJoinedServer = PlayerJoinedSound.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> PlayerLeftSound(TEXT("SoundWave'/Game/Sounds/hero/hero_gib3.hero_gib3'"));
	PlayerLeft = PlayerLeftSound.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> TickSound(TEXT("SoundWave'/Game/Sounds/multiplayer/clock-tick.clock-tick'"));
	TickSnd = TickSound.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> TockSound(TEXT("SoundWave'/Game/Sounds/multiplayer/clock-tock.clock-tock'"));
	TockSnd = TockSound.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundBase> BellSound(TEXT("SoundWave'/Game/Sounds/multiplayer/clock-bell.clock-bell'"));
	BellSnd = BellSound.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> BellBiggerSound(TEXT("SoundWave'/Game/Sounds/multiplayer/clock-bell-bigger.clock-bell-bigger'"));
	BellBiggerSnd = BellBiggerSound.Object;

	bShowMouseCursor = false;

	OnPawnDestoyedDelegate.BindUFunction(this, FName("RequestRespawn"));

	RespawnTimerDelegate.BindUFunction(this, FName("RespawnPlayer"));

	WarmUpViewTimerDelegate.BindUFunction(this, FName("SetWarmUpView"));

	OnDefaultTimerDelegate.BindUFunction(this, FName("UpdatePacketLoss"));

	OnEndPlayDelegate.BindUFunction(this, FName("HandleEndPlay"));
}

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// WTF ISSUE:
	// On the listening server 'SetInputMode' affects the localplayer when a new client connects.
	// seems like 'IsLocalController' always returns TRUE on the server side.

	if (!IsRunningDedicatedServer())
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(GetWorld()->GetGameInstance());
		if (GI->GetFirstGamePlayer()->PlayerController == this)
		{
			SetInputMode(FInputModeGameOnly());

			UWorld* World = GetWorld();
			if (World != NULL && World->GetGameState()/* && IsLocalController()*/)
			{
				APKGameState* GameState = Cast<APKGameState>(World->GetGameState());
				GameState->OnDefaultTimer.AddUnique(OnDefaultTimerDelegate);
			}

			GetWorldTimerManager().SetTimer(TimerHandle, this, &ANetPlayerController::UnblockOnFire, 2.f, false);

			SetWarmUpView();
		}
	}
}

void ANetPlayerController::UpdatePacketLoss()
{
	UWorld* World = GetWorld();
	if (World && World->NetDriver)
	{
		// see Engine\Source\Runtime\Engine\Private\NetworkDriver.cpp #if STATS
		float RealTime = World->NetDriver->Time - World->NetDriver->StatUpdateTime;
		float InPackets = World->NetDriver->InPackets / RealTime;
		float OutPackets = World->NetDriver->OutPackets / RealTime;

		uint32 OutLoss = FMath::TruncToInt(100.f * World->NetDriver->OutPacketsLost / FMath::Max(/*(float)World->NetDriver->OutPackets*/OutPackets, 1.f));
		uint32 InLoss = FMath::TruncToInt(100.f * World->NetDriver->InPacketsLost / FMath::Max(/*(float)World->NetDriver->InPackets*/InPackets + World->NetDriver->InPacketsLost, 1.f));

		PacketLoss = FMath::Max(OutLoss, InLoss);
#if !STATS
		World->NetDriver->OutPacketsLost = 0;
		World->NetDriver->InPacketsLost = 0;
#endif
	}

	LuciferTimer();
}

void ANetPlayerController::HandleEndPlay()
{
	bLuciferTimer = false;
	UTIL.AddMessage(L"EndPlay", 0, 1, 5);
}

void ANetPlayerController::LuciferTimer()
{
	UWorld* World = GetWorld();
	if (World != NULL && World->GetGameState() && PlayerState)
	{
		APKGameState* GameState = Cast<APKGameState>(World->GetGameState());
		
		if (LastElapsedTime == GameState->ElapsedTime) return;
		LastElapsedTime = GameState->ElapsedTime;

		// NOTE: ElapsedTime replicated COND_InitialOnly
		int32 TimeLeft = (GameState->TimeLimit - GameState->ElapsedTime)/* / 60*/;

		if (TimeLeft == 12)
		{
			Count = 0;
		}
		
		if (FMath::Fmod(GameState->ElapsedTime, 60) != 0 || TimeLeft <= 0) return;
		
		TimeLeft /= 60;

		switch (TimeLeft){
		case 1:
			PlaySnd_Implementation(ESound::Lucifer_time01);
			break;
		case 2:
			PlaySnd_Implementation(ESound::Lucifer_time02);
			break;
		case 3:
			PlaySnd_Implementation(ESound::Lucifer_time03);
			break;
		case 4:
			PlaySnd_Implementation(ESound::Lucifer_time04);
			break;
		case 5:
			PlaySnd_Implementation(ESound::Lucifer_time05);
			break;
		case 6:
			PlaySnd_Implementation(ESound::Lucifer_time06);
			break;
		case 7:
			PlaySnd_Implementation(ESound::Lucifer_time07);
			break;
		case 8:
			PlaySnd_Implementation(ESound::Lucifer_time08);
			break;
		case 9:
			PlaySnd_Implementation(ESound::Lucifer_time09);
			break;
		case 10:
			PlaySnd_Implementation(ESound::Lucifer_time10);
			break;
		case 15:
			PlaySnd_Implementation(ESound::Lucifer_time15);
			break;
		case 20:
			PlaySnd_Implementation(ESound::Lucifer_time20);
			break;
		case 25:
			PlaySnd_Implementation(ESound::Lucifer_time25);
			break;
		default:
			PlaySnd_Implementation(ESound::ClockBellBigger);
		}

		FString msg = FString::Printf(L"Time left: %i minute(s)", TimeLeft);
		Cast<APKPlayerState>(PlayerState)->AddConsoleMessage_Implementation(msg); // locally
	}
}

void ANetPlayerController::EndOfMatchCountdown()
{
	if (/*GameState->ElapsedTime < GameState->TimeLimit*/ Count < 12)
	{
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ANetPlayerController::EndOfMatchCountdown, 1.0f, false);
	}

	/*
	00001001	bellbig tick
	00000010	tock
	00000101	bell tick
	00000010	tock
	*/
	Count++;
	uint8 tick = Count & 0x01;
	uint8 tock = (!tick << 1);
	uint8 bell = ((Count & 0x01) && (Count & 0x02)) << 2;
	uint8 bellbig = (tick ^ (bell >> 2)) << 3;
	uint8 Flags = bellbig | bell | tock | tick;
	
	ClockFlags = ClockFlags ^ Flags;
	OnRep_ClockFlags();
}

/*note: here the pawn is valid only on the server*/
void ANetPlayerController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	SetStartAmmo(InPawn);
}

void ANetPlayerController::SetStartAmmo(APawn* InPawn)
{	
	/*stake = 100;
	grenade = 100;
	shell = 100;
	icebullet = 100;
	bullets = 250;
	return;*/

	ResetAmmo();
	
	uint8 WeaponIndex = Cast<ANetPlayer>(InPawn)->CurWeaponIndex;
	
	switch (WeaponIndex){
	case 1:
		break;
	case 2:
		shell = 20;		// 20
		icebullet = 0;	// 0
		break;
	case 3:
		stake = 10;		// 10
		grenade = 0;	// 0
		break;
	case 4:
		grenade = 5;	// 5
		bullets = 0;	// 0
		break;
		/*case 5:
		shuriken = 25;
		electro = 0;
		break;*/
	default:
		break;
	}
}

void ANetPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	if (/*!IsRunningDedicatedServer() && */IsLocalController())
	{
		/* watch the ragdoll */
		if (Ragdoll != NULL && Camera != NULL) {
			FVector Pos = Ragdoll->GetComponentLocation();
			/* "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h" */
			FRotator Rot = FRotationMatrix::MakeFromX(Pos - Camera->GetActorLocation()).Rotator();
			Camera->SetActorRotation(Rot);

			float dist = FMath::Abs((Pos - Camera->GetActorLocation()).Size());
			float ang = FMath::Clamp(100.f * (500.f / dist), 0.f, 140.f);
			if (FOVBlendVal < 1)
			{
				FOVBlendVal += DeltaTime / FOVTransTime;
				FOVBlendVal = FMath::Clamp(FOVBlendVal, 0.f, 1.f);
			}
			Camera->GetCameraComponent()->FieldOfView = FOV - (FOV - ang) * FOVBlendVal;
		}

		if (SpecificHitSoundTimeOut > 0) SpecificHitSoundTimeOut -= DeltaTime;
		if (bSpecificHitSound && SpecificHitSoundTimeOut <= 0)
		{
			FSlateSound NewSound;
			NewSound.SetResourceObject(HitSound);
			FSlateApplication::Get().PlaySound(NewSound);

			SpecificHitSoundTimeOut = 0.075f;
		}

		UpdateOpposingPawnName();

		UpdateScreenFlash(DeltaTime);
//#if PLATFORM_WINDOWS
		FSlateApplication::Get().QueryCursor();
//#endif
	}
}

void ANetPlayerController::UpdateScreenFlash(float DeltaTime)
{
	if (TickCount >= 0)
	{
		TickCount += DeltaTime * 30;
		PickUpFlashColor.A = (1 - FMath::Cos(TickCount)) * 0.125f;
		if (TickCount > PI * 2) TickCount = -1.f;
	}
	else PickUpFlashColor.A = 0.f;

	if (TickCountDamage >= 0)
	{
		TickCountDamage += DeltaTime * 30;
		DamageFlashColor.A = (1 - FMath::Cos(TickCountDamage)) * 0.125f;
		if (TickCountDamage > PI * 2) TickCountDamage = -1.f;
	}
	else DamageFlashColor.A = 0.f;
}

void ANetPlayerController::UpdateOpposingPawnName()
{
	if (AcknowledgedPawn && AcknowledgedPawn->Controller)
	{
		const FRotator Rotation = GetControlRotation();
		const FVector Start = Cast<ANetPlayer>(AcknowledgedPawn)->GetFirstPersonCameraComponent()->GetComponentLocation();

		FHitResult Hit = FHitResult(1.f);
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), false, AcknowledgedPawn);

		float SightRange = 100.f * 84.f;
		const FVector End = Start + SightRange * Rotation.RotateVector(FVector::ForwardVector);

		GetWorld()->LineTraceSingle(Hit, Start, End, ECollisionChannel::ECC_Visibility, TraceParams);

		if (Hit.bBlockingHit && Hit.GetActor()->IsA(ACharacter::StaticClass()))
		{
			if (Cast<ACharacter>(Hit.GetActor())->PlayerState)
			{
				OpposingPawnName = Cast<ACharacter>(Hit.GetActor())->PlayerState->GetHumanReadableName();
				return;
			}
		}
	}
	
	OpposingPawnName = FString();
}

void ANetPlayerController::AcknowledgePossession(APawn* P)
{
	bAutoManageActiveCameraTarget = true;
	Super::AcknowledgePossession(P);
		
	UCameraComponent* CameraComponent = Cast<ANetPlayer>(AcknowledgedPawn)->GetFirstPersonCameraComponent();
	PlayerCameraManager->DefaultFOV = CameraComponent->FieldOfView;
	
	SetPawnDestoyedDelegate();

	LockedWeapons = 0;
}

void ANetPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	if (/*!IsRunningDedicatedServer() && */IsLocalController()){
		if (PlayerState)
		{
			uint32 in = *((int*)&PlayerState->Score);
			int32 deaths = (0xFFC00 & in) >> 10;

			if (deaths == 0)
			{
				uint8 snd = FMath::RandRange(1, 2);
				snd = snd == 1 ? ESound::lucifer_comemychildren : ESound::Lucifer_fight;
				PlaySnd_Implementation(snd);
			}
			else
			{
				PlaySnd_Implementation(ESound::respawn_m);
			}
		}
	}
}

void ANetPlayerController::UnblockOnFire()
{
	EnableInput(this);
}

void ANetPlayerController::SetWarmUpView()
{
	if (PlayerState){
		if (PlayerState->bOnlySpectator)
		{
			TArray<AActor*> Cameras;
			UGameplayStatics::GetAllActorsOfClass(this, AWarmUpView::StaticClass(), Cameras);
			if (Cameras.Num() > 0)
			{
				ShowStats();
				PlayerCameraManager->SetViewTarget(Cameras[0]);
			}
			else{ // apparently we don't have any 'WarmUpView' camera on the level
				RequestRespawn();
			}
		}
	}
	else{ // if there is no PlayerState (client is lagging), then go to the next cycle
		GetWorldTimerManager().SetTimerForNextTick(WarmUpViewTimerDelegate);
	}
}

void ANetPlayerController::ShowStats()
{
	UPKGameInstance* GI = Cast<UPKGameInstance>(GetWorld()->GetGameInstance());
	if (!GI->WidgetToFocus)
	{
		UTIL.ShowWidgetClassOf(this, ScoreBoardWidgetClass, false);
	}
}

void ANetPlayerController::HideStats()
{
	if (AcknowledgedPawn && UTIL.GetCurrentInputMode(this) != EInputMode::UIOnly)
	{
		UTIL.ShowWidgetClassOf(this, nullptr);
	}
}

void ANetPlayerController::SetPawnDestoyedDelegate()
{
	if (/*!IsRunningDedicatedServer() && */IsLocalController()){
		if(LastPawn != NULL)LastPawn.Get()->OnDestroyed.Clear();
		LastPawn = AcknowledgedPawn;
		AcknowledgedPawn->OnDestroyed.AddUnique(OnPawnDestoyedDelegate);
		bDrawHUD = true;
	}
}

void ANetPlayerController::UnPossess()
{
	Super::UnPossess();

	bDrawHUD = false;

	bSpecificHitSound = false;

	if (Role == ROLE_AutonomousProxy) bLockSpawn = true; // locally on client only
}

void ANetPlayerController::SetCamera()
{
	UWorld* const World = GetWorld();

	FMinimalViewInfo ViewInfo;
	ViewInfo.FOV = PlayerCameraManager->DefaultFOV;
	CalcCamera(0, ViewInfo);

	FOV = ViewInfo.FOV;
	FOVTransTime = 1.0;
	FOVBlendVal = 0.f;

	if (Camera != NULL){
		Camera->SetActorLocationAndRotation(ViewInfo.Location, ViewInfo.Rotation);
	}
	else if (World != NULL){
		Camera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), ViewInfo.Location, ViewInfo.Rotation);
	}

	if (Camera.Get())
	{
		Camera->GetCameraComponent()->bConstrainAspectRatio = ViewInfo.bConstrainAspectRatio;
		Camera->GetCameraComponent()->FieldOfView = ViewInfo.FOV;

		PlayerCameraManager->SetViewTarget(Camera.Get());
		PlayerCameraManager->UnlockFOV();
	}
}

void ANetPlayerController::SetRespawnTimer()
{
	if (GetPawn() && GetPawn() == AcknowledgedPawn) return; // avoid repeated respawn after ragdoll is destroyed
	//GetWorldTimerManager().SetTimerForNextTick(RespawnTimerDelegate);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnTimerDelegate, 0.2f, false);
}

void ANetPlayerController::RespawnPlayer()
{
	UWorld* const World = GetWorld();
	if (World != NULL && World->GetAuthGameMode() && World->GameState)
	{
		if (!World->GameState->HasMatchEnded())
		{
			PlayerState->bOnlySpectator = false;
			this->StartSpot = NULL; // ensure a random spot
			World->GetAuthGameMode()->RestartPlayer(this);
			Health = 100; Armor = 0;
			FrozenArmor = 0;
			GetWorldTimerManager().ClearTimer(FreezeArmorTimerHandle);
		}		
	}
}

void ANetPlayerController::SetupInputComponent(){

	Super::SetupInputComponent();
	
	check(InputComponent);

	// overloads the same binding in NetPlayer
	InputComponent->BindAction("Fire", IE_Pressed, this, &ANetPlayerController::OnFire);
	/*InputComponent->BindAction("NetPlayerIndex", IE_Pressed, this, &ANetPlayerController::Info_Implementation);*/
		
	InputComponent->BindAction("Chat", IE_Pressed, this, &ANetPlayerController::ToggleChat);
	// overloads the same binding in NetPlayer
	InputComponent->BindAction("Disconnect", IE_Pressed, this, &ANetPlayerController::OnEscape);

	InputComponent->BindAction("ShowStats", IE_Pressed, this, &ANetPlayerController::ShowStats);
	InputComponent->BindAction("ShowStats", IE_Released, this, &ANetPlayerController::HideStats);
}

void ANetPlayerController::ToggleChat()
{
	UTIL.ShowWidgetClassOf(this, ChatWidgetClass);
	SetInputMode(FInputModeUIOnly());
}

void ANetPlayerController::OnFire()
{
	if (GetPawn() && GetPawn() == AcknowledgedPawn)
		Cast<ANetPlayer>(AcknowledgedPawn)->OnFire();
	else if (!bLockSpawn){
		UTIL.ShowWidgetClassOf(this, nullptr);
		RequestRespawn();
	}
}

void ANetPlayerController::OnEscape()
{
	UPKGameInstance* gameInstance = Cast<UPKGameInstance>(GetGameInstance());
	gameInstance->OnEscape();
}

void ANetPlayerController::SetupOnRagdollView_Implementation()
{
	/*if we unpossess the pawn on the server first, then 'high ping'(> 500) client gets
	disconnected. So carry out unpossession in the reverse order - server after client.
	NOTE: we need to unpossess on both sides
	server - to be able to respawn
	client - to be able to watch a ragdoll
	*/

	/*Unpossession on owning player*/
	bAutoManageActiveCameraTarget = false;
	UnPossess();
	if (Role == ROLE_AutonomousProxy){
		SeverAbandonPawn(); // Enforce unpossession on server
	}
	
	SetCamera();
	Ragdoll = Cast<ANetPlayer>(AcknowledgedPawn)->GetMesh();
}

void ANetPlayerController::Info_Implementation()
{	
	UWorld* const World = GetWorld();
	if (World/* != NULL && NetConnection*/)
	{
		TArray<ACameraActor*> Cameras;

		UTIL.FindAllObjectsClassOf(Cameras);
		for (auto actor : Cameras)
		{
			UTIL.AddMessage(L"%s", *actor->GetName(), 0, 5);
		}
	}
}

void ANetPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	// we could be on the menu at the end of the match so hide cursor
	bShowMouseCursor = false;

	SetInputMode(FInputModeUIOnly());
	AcknowledgedPawn = nullptr;
	PlayerState->bOnlySpectator = true;
	SetWarmUpView();
	
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
}

void ANetPlayerController::FreezeArmor()
{
	bool bFrozenArmor = GetWorldTimerManager().IsTimerActive(FreezeArmorTimerHandle);
	FrozenArmor = bFrozenArmor ? FrozenArmor : Armor; Armor = 0;
	GetWorldTimerManager().SetTimer(FreezeArmorTimerHandle, this, &ANetPlayerController::UnFreezeArmor, FrozenTime, false);

	SetFrozenAppearance(1);
}

void ANetPlayerController::UnFreezeArmor()
{
	Armor = FrozenArmor;
	FrozenArmor = 0;

	SetFrozenAppearance(0);
}

void ANetPlayerController::SetFrozenAppearance(bool bFrozen)
{
	if (AcknowledgedPawn)
	{
		ANetPlayer* Pawn = Cast<ANetPlayer>(AcknowledgedPawn);
		if (Pawn)
		{
			Pawn->bAppearFrozen = bFrozen;
			Pawn->OnRep_AppearFrozen(); // server side
		}
	}
}

bool ANetPlayerController::OnDamage(float Damage)
{
	// ignore damage if we are already dead, but somehow continue to receive it.
	if (Health <= 0) return false;

	uint8 ArmorType = Armor & 0x03;
	uint8 ArmorAmount = (Armor >> 2) * 4;

	float ArmorRescueFactor[] = { 0.f, 0.33f, 0.6f, 0.8f };
	float rf = ArmorRescueFactor[ArmorType];
	
	Damage = FMath::Abs(Damage);

	uint32 save = FMath::CeilToInt(rf * Damage);
	if (save >= ArmorAmount)
	{
		save = ArmorAmount;
		ArmorType = 0x00;
	}
	ArmorAmount -= save;
	ArmorAmount = ((ArmorAmount / 4) << 2);
	Armor = (ArmorAmount |= ArmorType);

	uint32 Dmg = FMath::CeilToInt(Damage - save);
	Health = (Dmg > Health) ? 0 : Health - Dmg;

	if (Health)
	{
		ClockFlags = ClockFlags ^ DamageFlashBit;
		OnRep_ClockFlags(); // server side
	}

	return Health == 0;
}

bool ANetPlayerController::AmmoAdd(uint8 AmmoType, uint8 AmmoAdd, uint8 AltAmmoAdd) //equipment
{
	// HEALTH
	uint8 LastHealth = Health;

	if (AmmoType == EAmmoType::health && Health < 100)
	{
		Health = Health + AmmoAdd > 100 ? 100 : Health + AmmoAdd;
	}

	if (AmmoType == EAmmoType::megahealth)
	{
		Health = Health + AmmoAdd > 250 ? 250 : Health + AmmoAdd;
	}
	
	if (Health > 100 && LastHealth != Health)
	{
		GetWorldTimerManager().ClearTimer(HealthDecTimerHandle);
		HealthDecTimerHandle.Invalidate();
		HealthDecay();
	}

	if (AmmoType == EAmmoType::health || AmmoType == EAmmoType::megahealth) return CheckPickUp(LastHealth != Health);
	
	// ARMOR
	if (AmmoType == EAmmoType::armor)
	{
		bool bFrozenArmor = GetWorldTimerManager().IsTimerActive(FreezeArmorTimerHandle);
		uint8 _Armor = bFrozenArmor ? FrozenArmor : Armor;

		uint8 LastArmor = _Armor;

		uint8 ArmorType = _Armor & 0x03;
		uint8 ArmorAmount = (_Armor >> 2) * 4;
		uint8 val;
		switch (AmmoAdd)
		{
		case 200:
			val = 0xc8;
			_Armor = (val |= Strong);
			break;
		case 150:
			val = 0x94;
			if ((ArmorType > Medium && ArmorAmount < 110) || ArmorType < Strong) _Armor = (val |= Medium);
			break;
		case 100:
			val = 0x64;
			if ((ArmorType > Weak   && ArmorAmount <  50) || ArmorType < Medium) _Armor = (val |= Weak);
			break;
		}

		if (bFrozenArmor)
			FrozenArmor = _Armor;
		else
			Armor = _Armor;

		return CheckPickUp(LastArmor != _Armor);
	}

	// WEAPON
	ANetPlayer* Player = Cast<ANetPlayer>(AcknowledgedPawn);
	if (AmmoType == EAmmoType::Painkiller)
	{
		if (Player) Player->SelectBestWeapon(0x01);
		return CheckPickUp(true);
	}

	if (AmmoType == EAmmoType::Shotgun)
	{
		shell = shell + AmmoAdd > 100 ? 100 : shell + AmmoAdd;
		icebullet = icebullet + AltAmmoAdd > 100 ? 100 : icebullet + AltAmmoAdd;
		if (Player) Player->SelectBestWeapon(0x02);
		return CheckPickUp(true);
	}

	if (AmmoType == EAmmoType::StakeGunGL)
	{
		stake = stake + AmmoAdd > 100 ? 100 : stake + AmmoAdd;
		grenade = grenade + AltAmmoAdd > 100 ? 100 : grenade + AltAmmoAdd;
		if (Player) Player->SelectBestWeapon(0x04);
		return CheckPickUp(true);
	}

	if (AmmoType == EAmmoType::MiniGunRL)
	{
		grenade = grenade + AmmoAdd > 100 ? 100 : grenade + AmmoAdd;
		bullets = bullets + AltAmmoAdd > 250 ? 250 : bullets + AltAmmoAdd;
		if (Player) Player->SelectBestWeapon(0x08);
		return CheckPickUp(true);
	}

	// AMMO
	int32 LastAmmo = stake + grenade + shell + icebullet + bullets;
	
	if (!AmmoType) // megapack
	{
		uint8 Ammo   = 8;
		stake     = stake     + Ammo > 100 ? 100 : stake     + Ammo;
		Ammo   = 4;
		grenade   = grenade   + Ammo > 100 ? 100 : grenade   + Ammo;
		Ammo   = 10;
		shell     = shell     + Ammo > 100 ? 100 : shell     + Ammo;
		Ammo   = 4;
		icebullet = icebullet + Ammo > 100 ? 100 : icebullet + Ammo;
		Ammo   = 45;
		bullets   = bullets   + Ammo > 250 ? 250 : bullets   + Ammo;
	}

	stake     = (AmmoType & 0x01) ? (stake     + AmmoAdd > 100 ? 100 : stake     + AmmoAdd) : stake;
	grenade   = (AmmoType & 0x02) ? (grenade   + AmmoAdd > 100 ? 100 : grenade   + AmmoAdd) : grenade;
	shell     = (AmmoType & 0x04) ? (shell     + AmmoAdd > 100 ? 100 : shell     + AmmoAdd) : shell;
	icebullet = (AmmoType & 0x08) ? (icebullet + AltAmmoAdd > 100 ? 100 : icebullet + AltAmmoAdd) : icebullet;
	bullets   = (AmmoType & 0x10) ? (bullets   + AltAmmoAdd > 250 ? 250 : bullets   + AltAmmoAdd) : bullets;
	
	if (!AmmoType || (AmmoType & 0x01)) UpdateStakeVisibility(); // server side

	int32 Ammo = stake + grenade + shell + icebullet + bullets;
	return CheckPickUp(Ammo != LastAmmo);
}

void ANetPlayerController::UpdateStakeVisibility()
{
	if (AcknowledgedPawn)
	{
		ANetPlayer* Pawn = Cast<ANetPlayer>(AcknowledgedPawn);
		if (Pawn && Pawn->IsLocallyControlled())
		{
			APKWeapon* Weap = Cast<APKWeapon>(Pawn->Weapons[2]->ChildActor);
			if (Weap) Weap->HideStake(stake == 0);
		}
	}
}

void ANetPlayerController::OnRep_AckStake()
{
	AckAmmo(0x01);
	UpdateStakeVisibility(); // client
}

void ANetPlayerController::OnRep_AckGrenade()
{
	AckAmmo(0x02);
}

void ANetPlayerController::OnRep_AckShell()
{
	AckAmmo(0x04);
}

void ANetPlayerController::OnRep_AckIcebullet()
{
	AckAmmo(0x08);
}

void ANetPlayerController::OnRep_AckBullets()
{
	AckAmmo(0x10);
}

void ANetPlayerController::AckAmmo(uint8 AmmoType)
{
	if (LockedWeapons & AmmoType) LockedWeapons = (LockedWeapons ^ AmmoType);
}

void ANetPlayerController::ConsoleSendMessage_Implementation(const FString& InMsg)
{
	// TODO: distinguish between commands and messages

	// WORKAROUND: $-nick for the message to play sound.
	UWorld* World = GetWorld();
	if (World != NULL && World->GetGameState())
	{
		FString msg = "$" + PlayerState->GetHumanReadableName() + ": " + InMsg;

		APKGameState* GameState = Cast<APKGameState>(World->GetGameState());
		for (int i = 0; i < GameState->PlayerArray.Num(); i++)
		{
			Cast<APKPlayerState>(GameState->PlayerArray[i])->AddConsoleMessage(msg);
		}
	}
}

void ANetPlayerController::PlaySnd_Implementation(uint8 snd)
{
	if (IsRunningDedicatedServer()) return; // otherwise crashes

	FSlateSound NewSound;
	switch (snd)
	{
	case ESound::PLAYER_has_entered_the_arena:
		NewSound.SetResourceObject(NewPlayerJoinedServer);
		break;
	case ESound::PLAYER_chickened_and_left_the_arena:
		NewSound.SetResourceObject(PlayerLeft);
		break;
	case ESound::ClockTick:
		NewSound.SetResourceObject(TickSnd);
		break;
	case ESound::ClockTock:
		NewSound.SetResourceObject(TockSnd);
		break;
	case ESound::ClockBell:
		NewSound.SetResourceObject(BellSnd);
		break;
	case ESound::ClockBellBigger:
		NewSound.SetResourceObject(BellBiggerSnd);
		break;
	case ESound::lucifer_comemychildren:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "lucifer-comemychildren"));
		break;
	case ESound::Lucifer_fight:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_fight"));
		break;
	case ESound::Lucifer_verybad:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", FString::Printf(TEXT("Lucifer_verybad0%d"), FMath::RandRange(1, 2))));
		break;
	case ESound::Lucifer_bad:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", FString::Printf(TEXT("Lucifer_bad0%d"), FMath::RandRange(1, 4))));
		break;
	case ESound::Lucifer_good:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", FString::Printf(TEXT("Lucifer_good0%d"), FMath::RandRange(1, 5))));
		break;
	case ESound::Lucifer_excellent:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", FString::Printf(TEXT("Lucifer_excellent0%d"), FMath::RandRange(1, 2))));
		break;
	case ESound::Lucifer_time01:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time01"));
		break;
	case ESound::Lucifer_time02:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time02"));
		break;
	case ESound::Lucifer_time03:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time03"));
		break;
	case ESound::Lucifer_time04:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time04"));
		break;
	case ESound::Lucifer_time05:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time05"));
		break;
	case ESound::Lucifer_time06:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time06"));
		break;
	case ESound::Lucifer_time07:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time07"));
		break;
	case ESound::Lucifer_time08:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time08"));
		break;
	case ESound::Lucifer_time09:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time09"));
		break;
	case ESound::Lucifer_time10:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time10"));
		break;
	case ESound::Lucifer_time15:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time15"));
		break;
	case ESound::Lucifer_time20:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time20"));
		break;
	case ESound::Lucifer_time25:
		NewSound.SetResourceObject(UTIL.GetSound("multiplayer/lucifer", "Lucifer_time25"));
		break;
	case ESound::wrong_place:
		NewSound.SetResourceObject(UTIL.GetSound("menu/magicboard", "wrong_place"));
		break;
	case ESound::respawn_m:
		NewSound.SetResourceObject(UTIL.GetSound("specials/respawns", FString::Printf(TEXT("respawn_m%d"), FMath::RandRange(1, 6))));
		break;
	}

	FSlateApplication::Get().PlaySound(NewSound);
}

void ANetPlayerController::OnRep_ClockFlags()
{
	if ((LastClockFlags & ClockTickBit) ^ (ClockFlags & ClockTickBit))
	{
		PlaySnd_Implementation(ESound::ClockTick);
	}

	if ((LastClockFlags & ClockTockBit) ^ (ClockFlags & ClockTockBit))
	{
		PlaySnd_Implementation(ESound::ClockTock);
	}

	if ((LastClockFlags & ClockBellBit) ^ (ClockFlags & ClockBellBit))
	{
		PlaySnd_Implementation(ESound::ClockBell);
	}

	if ((LastClockFlags & ClockBellBiggerBit) ^ (ClockFlags & ClockBellBiggerBit))
	{
		PlaySnd_Implementation(ESound::ClockBellBigger);
	}

	if ((LastClockFlags & PickUpFlashBit) ^ (ClockFlags & PickUpFlashBit))
	{
		PickUpFlash();
	}

	if ((LastClockFlags & DamageFlashBit) ^ (ClockFlags & DamageFlashBit))
	{
		DamageFlash();
	}

	LastClockFlags = ClockFlags;
}

/************************************************************
	Replication List
************************************************************/
void ANetPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ANetPlayerController, Health, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, Armor, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, stake, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, grenade, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, shell, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, icebullet, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, bullets, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, bSpecificHitSound, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ANetPlayerController, ClockFlags, COND_OwnerOnly);
}