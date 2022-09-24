// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_LocalPlayer.h"

bool UEWN_LocalPlayer::SpawnPlayActor( const FString& URL, FString& OutError, UWorld* InWorld )
{
	bool bSucceeded = Super::SpawnPlayActor( URL, OutError, InWorld );
	if ( bSucceeded )
	{
		// EWN_Interface_LocalPlayerExtension implementation
		SpawnPlayActorDelegate.Broadcast( GetPlayerController( InWorld ) );
	}
	return bSucceeded;
}
