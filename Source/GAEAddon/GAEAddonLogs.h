// Copyright (C) 2023 owoDra

#pragma once

#include "Logging/LogMacros.h"

GAEADDON_API DECLARE_LOG_CATEGORY_EXTERN(LogGAEA, Log, All);

#if !UE_BUILD_SHIPPING

#define GAEALOG(FormattedText, ...) UE_LOG(LogGAEA, Log, FormattedText, __VA_ARGS__)

#define GAEAENSURE(InExpression) ensure(InExpression)
#define GAEAENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GAEAENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GAEACHECK(InExpression) check(InExpression)
#define GAEACHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GAEALOG(FormattedText, ...)

#define GAEAENSURE(InExpression) InExpression
#define GAEAENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GAEAENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GAEACHECK(InExpression) InExpression
#define GAEACHECK_MSG(InExpression, InFormat, ...) InExpression

#endif