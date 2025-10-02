// Fill out your copyright notice in the Description page of Project Settings.


#include "DayNightCycle.h"
#include "BPI_DayTimeChange.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADayNightCycle::ADayNightCycle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADayNightCycle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADayNightCycle::Tick(float DeltaTime)
{
		Super::Tick(DeltaTime);
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Current Time of Day: %f"), CurrentTimeOfDay));

		// Advance time
		CurrentTimeOfDay += DeltaTime;
		float CycleLength = DayLength + NightLength;
		if (CurrentTimeOfDay >= CycleLength)
		{
			CurrentTimeOfDay -= CycleLength; // Loop back to start
		}

		// Broadcast normalized time and day/night state at specified frequency
		bool bIsDay = CurrentTimeOfDay < DayLength;
		float NormalizedTime = 0.0f;
		float PeriodLength = bIsDay ? DayLength : NightLength;
		float TimeInPeriod = bIsDay ? CurrentTimeOfDay : (CurrentTimeOfDay - DayLength);
		NormalizedTime = TimeInPeriod / PeriodLength;

		int32 EventIndex = FMath::FloorToInt(NormalizedTime * EventFrequencyPerDay);
		if (EventIndex != LastEventIndex)
		{
			LastEventIndex = EventIndex;
			TArray<AActor*> ActorsWithInterface;
			UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UBPI_DayTimeChange::StaticClass(), ActorsWithInterface);
			for (AActor* Actor : ActorsWithInterface)
			{
				IBPI_DayTimeChange::Execute_OnPeriodChanged(Actor, bIsDay, NormalizedTime);
			}
		}
}

// Returns true if it is currently daytime
bool ADayNightCycle::IsDaytime() const
{
	// Daytime is from 0 to DayLength seconds
	return CurrentTimeOfDay < DayLength;
}

// Returns the current time of day in seconds since the start of the cycle
float ADayNightCycle::GetTimeOfDay() const
{
	return CurrentTimeOfDay;
}

