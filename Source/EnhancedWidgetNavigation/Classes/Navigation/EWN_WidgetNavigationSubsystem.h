// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Tickable.h"

#include "EWN_WidgetNavigationSubsystem.generated.h"

class UWidget;

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
	virtual ETickableTickType GetTickableTickType() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual void Tick( float DeltaTime ) override;
	// FTickableGameObject END

private:
	void SetFocusAndLocking( bool bCaptureMouse );

private:
	friend class UEWN_WidgetNavigation;
	void MarkOnThisFrame( UEWN_WidgetNavigation* Navigation );

private:
	friend class UEWN_MenuWidget;
	void MenuConstruct( UEWN_MenuWidget* MenuWidget );
	void MenuDestruct( UEWN_MenuWidget* MenuWidget );

private:
	TArray<TWeakObjectPtr<UEWN_WidgetNavigation>> ActiveNavigationsOnLastFrame;

	int32 MenuCounter;
	bool bShowMouseCursor = true;
};
