// Fill out your copyright notice in the Description page of Project Settings.

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
