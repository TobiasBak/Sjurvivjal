#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightCycle.generated.h"

UCLASS()
class SJURVIVJAL_API ADayNightCycle : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADayNightCycle();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DayNightCycle")
	float CurrentTimeOfDay = 0.0f; // Seconds since start of cycle

	// Length of the day in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNightCycle")
	float DayLength = 600.0f; // Default: 10 minutes

	// Length of the night in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNightCycle")
	float NightLength = 300.0f; // Default: 5 minutes

	// How many times per day to send the event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNightCycle")
	int32 EventFrequencyPerDay = 100;

	// Returns true if it is currently daytime
	UFUNCTION(BlueprintCallable, Category = "DayNightCycle")
	bool IsDaytime() const;

	// Returns the current time of day in seconds since the start of the cycle
	UFUNCTION(BlueprintCallable, Category = "DayNightCycle")
	float GetTimeOfDay() const;

private:
	int32 LastEventIndex = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
