// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKCache.h"

PKCache::PKCache()
{
	/*UE_LOG(LogClass, Log, TEXT("Create PKCache"), 0);*/
}

PKCache::~PKCache()
{
}


void PKCache::Init(class USoundAttenuation* AttenuationSettings)
{		
	static ConstructorHelpers::FObjectFinder<USoundBase> sound(TEXT("SoundWave'/Game/FirstPerson/Audio/wrong.wrong'"));
	WrongSound = sound.Object;

	FString PathList[] = {
		TEXT("/Game/Sounds/actor/evilmonkv3"),
		TEXT("/Game/Sounds/impacts"),
		TEXT("/Game/Sounds/items"),
		TEXT("/Game/Sounds/misc"),
		TEXT("/Game/Sounds/specials"),
		TEXT("/Game/Sounds/specials/respawns"),
		TEXT("/Game/Sounds/menu/magicboard"),
		TEXT("/Game/Sounds/multiplayer"),
		TEXT("/Game/Sounds/multiplayer/environment"),
		TEXT("/Game/Sounds/multiplayer/lucifer"),
		TEXT("/Game/Sounds/multiplayer/player"),
		TEXT("/Game/Sounds/weapons"),
		TEXT("/Game/Sounds/weapons/assault-rifle"),
		TEXT("/Game/Sounds/weapons/grenadelauncher"),
		TEXT("/Game/Sounds/weapons/machinegun"),
		TEXT("/Game/Sounds/weapons/painkiller"),
		TEXT("/Game/Sounds/weapons/shotgun"),
		TEXT("/Game/Sounds/weapons/stake")
	};

	for (auto Path : PathList){
		Sounds.Add(Path, PrecacheSounds(Path, AttenuationSettings));
	}
}

TMap<FString, USoundBase*> PKCache::PrecacheSounds(FString BasePath, class USoundAttenuation* AttenuationSettings)
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry")).Get();

	// Need to do this if running in the editor with -game 
	// to make sure that the assets in the following path are available
#if WITH_EDITORONLY_DATA
	//if (WITH_EDITOR)
	//{
		TArray<FString> PathsToScan;
		PathsToScan.Add(BasePath);
		AssetRegistry.ScanPathsSynchronous(PathsToScan);
	//}
#endif // WITH_EDITORONLY_DATA

	TArray<FAssetData> AssetList;
	AssetRegistry.GetAssetsByPath(FName(*BasePath), AssetList);
	TMap<FString, USoundBase*> Sounds;
	for (FAssetData Asset : AssetList){
		FString ShortName = FPackageName::GetShortName(Asset.GetFullName());
		FString Left; FString Right;
		ShortName.Split(TEXT("."), &Left, &Right);
		USoundBase* snd = Cast<USoundBase>(Asset.GetAsset());
		snd->AttenuationSettings = AttenuationSettings;
		Sounds.Add(*Left, snd);
	}
	
	return Sounds;
}

PKCache CACHE = PKCache();

