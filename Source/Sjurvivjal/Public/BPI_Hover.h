#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_Hover.generated.h"

UINTERFACE(Blueprintable)
class SJURVIVJAL_API UBPI_Hover : public UInterface
{
    GENERATED_BODY()
};

class SJURVIVJAL_API IBPI_Hover
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hover")
    void OnHover(AActor* HoveredActor);
};