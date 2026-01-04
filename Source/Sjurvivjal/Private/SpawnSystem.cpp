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
                // You may want to set a reasonable radius for your actor
                float ActorRadius = 50.0f;
                FVector SpawnLocation = GetRandomPointInCollisionBox();
                
                // Offset along the local Up axis of the box to correctly handle rotation
                SpawnLocation += CollisionBox->GetUpVector() * ActorRadius;

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
    FVector BoxExtent = CollisionBox->GetScaledBoxExtent();
    FVector BoxLocation = CollisionBox->GetComponentLocation();
    FQuat BoxRotation = CollisionBox->GetComponentQuat();

    // Use the configurable DetectionHeight (defaulting to 1000 for tall actors like trees)
    FVector DetectionExtent = FVector(BoxExtent.X, BoxExtent.Y, DetectionHeight / 2.0f);
    
    // Offset the center of the detection box so it covers the space from the surface upwards
    FVector DetectionLocation = BoxLocation + BoxRotation.RotateVector(FVector(0, 0, BoxExtent.Z + DetectionHeight / 2.0f));

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // Use ECC_Visibility to catch both Pawns and Static/Dynamic objects like trees
    GetWorld()->OverlapMultiByChannel(
        Overlaps,
        DetectionLocation,
        BoxRotation,
        ECC_Visibility, 
        FCollisionShape::MakeBox(DetectionExtent),
        Params
    );

    int32 Count = 0;
    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* OverlapActor = Overlap.GetActor();
        if (OverlapActor && (!SpawnableActor || OverlapActor->IsA(SpawnableActor)))
        {
            Count++;
        }
    }

    return Count < MaxActorsInCollisionBox;
}

// Helper function to check if a location is free of collisions
bool ASpawnSystem::IsSpawnLocationFree(const FVector& Location, float Radius) const
{
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // Use Visibility channel to match the population check
    return !GetWorld()->OverlapBlockingTestByChannel(
        Location,
        FQuat::Identity,
        ECC_Visibility, 
        FCollisionShape::MakeSphere(Radius),
        Params
    );
}

FVector ASpawnSystem::GetRandomPointInCollisionBox() const
{
    // Use unscaled extent to avoid double-scaling when using TransformPosition
    FVector BoxExtent = CollisionBox->GetUnscaledBoxExtent();
    FTransform BoxTransform = CollisionBox->GetComponentTransform();

    FVector RandomLocal = UKismetMathLibrary::RandomPointInBoundingBox(FVector::ZeroVector, BoxExtent);
    RandomLocal.Z = BoxExtent.Z; // Ensure it spawns on the top surface
    
    return BoxTransform.TransformPosition(RandomLocal);
}