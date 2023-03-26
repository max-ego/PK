// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine.h"
#include "EngineUtils.h"
#include "AssetRegistryModule.h"
#include "Util.h"

/**
 * 
 */
class PK_API PKCache
{
public:
	PKCache();
	~PKCache();

	// replacement for missing sound
	USoundBase* WrongSound;

	TMap<FString, TMap<FString, USoundBase*>> Sounds;

	TMap<FString, USoundBase*> PrecacheSounds(FString BasePath, class USoundAttenuation* AttenuationSettings);

	TArray<FString> AssetNames; // debug

	void Init(class USoundAttenuation* AttenuationSettings);
};


/**/
extern PKCache CACHE;