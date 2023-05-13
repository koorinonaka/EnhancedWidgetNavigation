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
	virtual ETickableTickType GetTickableTickType() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual TStatId GetStatId() const;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual void Tick( float DeltaTime );
	// FTickableGameObject END

private:
	void SetFocusAndLocking( bool bCaptureMouse );

private:
	friend class UEWN_WidgetNavigation;
	void MarkOnThisFrame( class UEWN_WidgetNavigation* Navigation );

private:
	friend class UEWN_MenuWidget;
	void MenuConstruct( class UEWN_MenuWidget* MenuWidget );
	void MenuDestruct( class UEWN_MenuWidget* MenuWidget );

private:
	TArray<TWeakObjectPtr<class UEWN_WidgetNavigation>> ActiveNavigationsOnLastFrame;

	int32 MenuCounter;
	bool bShowMouseCursor = true;
};
