// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"
#include "UObject/Class.h"

#define EWN_UE_VERSION_OR_LATER( Major, Minor ) \
	ENGINE_MAJOR_VERSION > Major || ( ENGINE_MAJOR_VERSION == Major && ENGINE_MINOR_VERSION >= Minor )

namespace EWN::Enum
{
template <typename EnumType>
FORCEINLINE static FString GetValueAsString( EnumType EnumValue )
{
	static_assert( TIsEnum<EnumType>::Value, "Should only call this with enum types" );
	const UEnum* EnumClass = StaticEnum<EnumType>();
	check( EnumClass != nullptr );
	return EnumClass->GetNameStringByValue( static_cast<int64>( EnumValue ) );
}

template <typename EnumType>
FORCEINLINE static EnumType FindValueByName( FName EnumName )
{
	static_assert( TIsEnum<EnumType>::Value, "Should only call this with enum types" );
	const UEnum* EnumClass = StaticEnum<EnumType>();
	check( EnumClass != nullptr );
	int64 FoundValue = EnumClass->GetValueByName( EnumName );
	return FoundValue != INDEX_NONE ? static_cast<EnumType>( FoundValue ) : EnumType::None;
}

template <typename EnumType>
FORCEINLINE static void ForEach( const TFunctionRef<void( EnumType )>& Callback )
{
	static_assert( TIsEnum<EnumType>::Value, "Should only call this with enum types" );
	const UEnum* EnumClass = StaticEnum<EnumType>();
	check( EnumClass != nullptr );
	for ( int i = 0; i < EnumClass->NumEnums() - 1; ++i )
	{
		EnumType Enum = static_cast<EnumType>( EnumClass->GetValueByIndex( i ) );
		Callback( Enum );
	}
}
}	 // namespace EWN::Enum
