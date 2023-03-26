// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PK : ModuleRules
{
	public PK(TargetInfo Target)
	{
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore", "OnlineSubsystem", "OnlineSubsystemUtils", "MoviePlayer", "AIModule", "Sockets", "Networking", "OnlineSubsystemNull", "RenderCore" });
        
        //PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	}
}
