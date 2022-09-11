// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//
#include "EWN_WidgetTypes.h"

#include "EWN_WidgetNavigationSwitcher.generated.h"

UCLASS( Blueprintable )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationSwitcher : public UObject
{
	GENERATED_BODY()

	struct FWidgetWithNavigation
	{
		class UEWN_WidgetNavigation* Navigation = nullptr;
		int32 Index = INDEX_NONE;
		FVector2D Position;
	};

	struct FWidgetNavigationOverride
	{
		TObjectKey<class UEWN_WidgetNavigation> Navigation;
		EEWN_WidgetCursor WidgetCursor = EEWN_WidgetCursor::None;

		FORCEINLINE bool operator==( const FWidgetNavigationOverride& Other ) const
		{
			return Navigation == Other.Navigation && WidgetCursor == Other.WidgetCursor;
		}

		friend uint32 GetTypeHash( const FWidgetNavigationOverride& Row )
		{
			return GetTypeHash( Row.Navigation ) ^ GetTypeHash( (uint8) Row.WidgetCursor );
		}
	};

public:
	UFUNCTION( BlueprintCallable )
	class UEWN_WidgetNavigation* TickNavigation( float DeltaTime, EEWN_WidgetInputType& OutInputType );

	UFUNCTION( BlueprintCallable )
	void Register( class UEWN_WidgetNavigation* WidgetNavigation );

	UFUNCTION( BlueprintCallable )
	void Unregister( class UEWN_WidgetNavigation* WidgetNavigation );

	UFUNCTION( BlueprintCallable )
	void AddRoute( EEWN_WidgetCursor WidgetCursor, class UEWN_WidgetNavigation* Source, class UEWN_WidgetNavigation* Destination );

	UFUNCTION( BlueprintCallable )
	void RemoveRoute(
		EEWN_WidgetCursor WidgetCursor, class UEWN_WidgetNavigation* Source, class UEWN_WidgetNavigation* Destination );

public:
	UFUNCTION( BlueprintCallable )
	void SetLoopNavigation( bool bNewFlag ) { bLoopNavigation = bNewFlag; }

private:
	bool MoveFocusOverride( class UEWN_WidgetNavigation* Navigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool MoveFocusFallback( class UEWN_WidgetNavigation* Navigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );

	UWidget* FindFocusToNearest( UWidget* CurrentWidget, EEWN_WidgetCursor WidgetCursor,
		const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation ) const;
	UWidget* FindFocusToFarthest( UWidget* CurrentWidget, EEWN_WidgetCursor WidgetCursor,
		const TMap<UWidget*, FWidgetWithNavigation>& WidgetsWithNavigation ) const;

	UFUNCTION()
	void OnNavigationFocusUpdated( class UEWN_WidgetNavigation* Navigation, int32 OldIndex, int32 NewIndex, bool bFromOperation );

private:
	TWeakObjectPtr<class UEWN_WidgetNavigation> LastActiveNavigation;
	TArray<TWeakObjectPtr<class UEWN_WidgetNavigation>> WidgetNavigations;
	TMultiMap<FWidgetNavigationOverride, TWeakObjectPtr<class UEWN_WidgetNavigation>> WidgetNavigationOverrides;

	bool bLoopNavigation = false;
};
