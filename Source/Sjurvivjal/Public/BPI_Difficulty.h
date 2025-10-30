#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_Difficulty.generated.h"

UINTERFACE(MinimalAPI)
class UBPI_Difficulty : public UInterface
{
    GENERATED_BODY()
};

class SJURVIVJAL_API IBPI_Difficulty
{
    GENERATED_BODY()

public:
    // Function to set the difficulty
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void UpdateDifficulty(float Difficulty);
};