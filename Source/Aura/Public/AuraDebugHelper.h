﻿// YanWei.

#pragma once

#include "CoreMinimal.h"

namespace AuraDebug
{
	static void Print(const FString& Msg, const FColor& Color = FColor::White, int32 Inkey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Inkey, 7.f, Color, Msg);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}

	}

	static void Print(const FString& FloatTitle, float FloatValueToPrint, int32 InKey = -1, const FColor& Color = FColor::White)
	{
		const FString FinalMsg = FloatTitle + TEXT(":") + FString::SanitizeFloat(FloatValueToPrint);

		GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, FinalMsg);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalMsg);

	}

}
