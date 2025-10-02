#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_DayTimeChange.generated.h"

UINTERFACE(Blueprintable)
class SJURVIVJAL_API UBPI_DayTimeChange : public UInterface
{
    GENERATED_BODY()
};

class SJURVIVJAL_API IBPI_DayTimeChange
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DayNightCycle")
    void OnPeriodChanged(bool bIsDay, float NormalizedTimeOfPeriod);
};