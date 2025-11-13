// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetMathLibrary.h"
#include "SpawnSystem.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "BPI_Difficulty.h"

// Sets default values
ASpawnSystem::ASpawnSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnableActor = nullptr;
	SpawnDelay = 1.0f;
	SpawnCount = 1;
	MaxActorsInCollisionBox = 5;
	bSpawnOnlyAtNight = false; // Default: spawn at any time

	// Create and attach the collision box
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(RootComponent);

    // Set the default size of the collision box
    CollisionBox->SetBoxExtent(FVector(200.0f, 200.0f, 200.0f));
    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void ASpawnSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

float TimeSinceLastSpawn = 0.0f;

// Called every frame
void ASpawnSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!SpawnableActor) { return; }

    TimeSinceLastSpawn += DeltaTime;

    if (TimeSinceLastSpawn >= SpawnDelay)
    {
        TimeSinceLastSpawn = 0.0f;

        // Check if spawning is allowed based on the day/night cycle
        if (DayNightCycle)
        {
            bool bIsDaytime = DayNightCycle->IsDaytime();
            if ((bSpawnOnlyAtNight && bIsDaytime) || (!bSpawnOnlyAtNight && !bIsDaytime))
            {
                return; // Skip spawning if the time doesn't match the spawn condition
            }
        }

        // Check if there is room in the collision box
        if (!(IsRoomInCollisionBox())) { return; }

        for (int32 i = 0; i < SpawnCount; ++i) {
            // Try up to 10 times to find a free spot
            const int32 MaxAttempts = 10;
            bool bSpawned = false;
            for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
            {
                FVector SpawnLocation = GetRandomPointInCollisionBox();

                // You may want to set a reasonable radius for your actor
                float ActorRadius = 50.0f;

                if (IsSpawnLocationFree(SpawnLocation, ActorRadius))
                {
                    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnableActor, SpawnLocation, FRotator::ZeroRotator);
                    if (SpawnedActor)
                    {
                        SetActorDifficulty(SpawnedActor);
                        bSpawned = true;
                        break;
                    }
                }
            }
        }
    }
}

void ASpawnSystem::SetActorDifficulty(AActor* SpawnedActor)
{
    if (!SpawnedActor) return;

    // Calculate the difficulty based on the day count
    float Difficulty = 1.0f;
    if (DayNightCycle)
    {
        int32 CurrentDay = DayNightCycle->GetCurrentDay();
        Difficulty = 1.0f + (CurrentDay * DifficultyMultiplierPerDay);
    }

    // Check if the spawned actor has a difficulty variable
    if (SpawnedActor->Implements<UBPI_Difficulty>())
    {
        IBPI_Difficulty::Execute_UpdateDifficulty(SpawnedActor, Difficulty);
    }
}

bool ASpawnSystem::IsRoomInCollisionBox() const
{
    TArray<AActor*> OverlappingActors;
    CollisionBox->GetOverlappingActors(OverlappingActors);

    // Check if the number of overlapping actors is less than the max allowed
    return OverlappingActors.Num() < MaxActorsInCollisionBox;
}

// Helper function to check if a location is free of collisions
bool ASpawnSystem::IsSpawnLocationFree(const FVector& Location, float Radius) const
{
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // You may want to adjust the collision channel and shape as needed
    return !GetWorld()->OverlapBlockingTestByChannel(
        Location,
        FQuat::Identity,
        ECC_Pawn, // Or ECC_WorldStatic, depending on your needs
        FCollisionShape::MakeSphere(Radius),
        Params
    );
}

FVector ASpawnSystem::GetRandomPointInCollisionBox() const
{
    FVector BoxExtent = CollisionBox->GetScaledBoxExtent();
    FVector BoxOrigin = CollisionBox->GetComponentLocation();

    FVector RandomLocal = UKismetMathLibrary::RandomPointInBoundingBox(FVector::ZeroVector, BoxExtent);
    return BoxOrigin + RandomLocal;
}