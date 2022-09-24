// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Tickable.h"

#include "EWN_WidgetNavigationSubsystem.generated.h"

UCLASS()
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationSubsystem : public ULocalPlayerSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	static UEWN_WidgetNavigationSubsystem* Get( UWidget* Widget );

protected:
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;
	virtual void Deinitialize() override;

	// FTickableGameObject BEGIN
protected:
	virtual TStatId GetStatId() const;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual void Tick( float DeltaTime );
	// FTickableGameObject END

public:
	void MarkOnThisFrame( class UEWN_WidgetNavigation* Navigation );

	void MenuConstruct();
	void MenuDestruct();

private:
	TArray<TWeakObjectPtr<class UEWN_WidgetNavigation>> ActiveNavigationsOnLastFrame;

	int32 MenuCounter;
};
