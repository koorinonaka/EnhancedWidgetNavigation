// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Runtime/Launch/Resources/Version.h"

#define EWN_UE_VERSION_OR_LATER( Major, Minor ) \
	ENGINE_MAJOR_VERSION > Major || ( ENGINE_MAJOR_VERSION == Major && ENGINE_MINOR_VERSION >= Minor )
