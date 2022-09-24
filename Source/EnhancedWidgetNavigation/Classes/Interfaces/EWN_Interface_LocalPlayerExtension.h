// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "EWN_Interface_LocalPlayerExtension.generated.h"

UINTERFACE( meta = ( CannotImplementInterfaceInBlueprint ) )
class ENHANCEDWIDGETNAVIGATION_API UEWN_Interface_LocalPlayerExtension : public UInterface
{
	GENERATED_BODY()
};

class ENHANCEDWIDGETNAVIGATION_API IEWN_Interface_LocalPlayerExtension
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam( FPlayerControllerDelegate, APlayerController* );
	FPlayerControllerDelegate SpawnPlayActorDelegate;
};
