// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
public class NdNetClient : ModuleRules
{
    public NdNetClient(ReadOnlyTargetRules Target) : base (Target)
    {
        PrivateIncludePaths.AddRange(
          new string[] {
               "NdNetClient/include"
          }
          );


        PublicDependencyModuleNames.AddRange( new string[]{"Core" });

        bEnableExceptions = true;
        bEnableShadowVariableWarnings = false;
		bEnableUndefinedIdentifierWarnings = false;

        PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
        PublicDefinitions.Add("WITHOUT_LOGIC_PARSER");
        
        PublicDefinitions.Add("NDCLIENT_FOR_UE4=1");
        PublicDefinitions.Add("WITHOUT_ICONV=1");
        PublicDefinitions.Add("USING_WIN_PSDK");
        
        PublicDefinitions.Add("ND_COMPILE_AS_DLL=1");
        PrivateDefinitions.Add("ND_CLI_EXPORTS=1");

        //PrivateDefinitions.Add("LOGIC_PARSE_EXPORTS=1");
        //Definitions.Add("_UNNEED_INCLUDE_WINDOWS_SYSTEM=1");
        //Definitions.Add("ND_UNDEF_WIN32_TYPE=1");

        if (Target.Configuration == UnrealTargetConfiguration.Debug)
        {
            PublicDefinitions.Add("ND_DEBUG=1");
        }


        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            //PublicDefinitions.Add("ND_UNIX=1");
            PublicDefinitions.Add("__ND_LINUX__=1");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            //PublicDefinitions.Add("ND_UNIX=1");
            PublicDefinitions.Add("__ND_MAC__=1");
            //PublicFrameworks.Add("iconv");
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {

            //PublicDefinitions.Add("ND_UNIX=1");
            PublicDefinitions.Add("__ND_IOS__=1");
            //PublicDefinitions.Add("__MAC_OS__=1");
            //PublicFrameworks.Add("iconv");
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            //PublicDefinitions.Add("ND_UNIX=1");
            //PublicDefinitions.Add("__LINUX__=1");
			PublicDefinitions.Add("__ND_ANDROID__=1");
			PublicFrameworks.Add("log");
        }
    }
}
