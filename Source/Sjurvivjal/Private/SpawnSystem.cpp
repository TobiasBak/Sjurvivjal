// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
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
    TimeSinceLastSpawn = 0.0f;

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

// Called every frame
void ASpawnSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!SpawnableActor) 
    { 
        static float LastNoActorLogTime = 0.0f;
        if (GetWorld()->GetTimeSeconds() - LastNoActorLogTime > 5.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnSystem (%s): No SpawnableActor set!"), *GetName());
            LastNoActorLogTime = GetWorld()->GetTimeSeconds();
        }
        return; 
    }

    TimeSinceLastSpawn += DeltaTime;

    if (TimeSinceLastSpawn >= SpawnDelay)
    {
        // Check if spawning is allowed based on the day/night cycle
        if (DayNightCycle)
        {
            bool bIsDaytime = DayNightCycle->IsDaytime();
            bool bShouldSpawn = true;

            if (bSpawnOnlyAtNight && bIsDaytime)
            {
                bShouldSpawn = false;
            }

            if (!bShouldSpawn)
            {
                UE_LOG(LogTemp, Log, TEXT("SpawnSystem (%s): Skipping spawn - Day/Night condition not met (NightOnly: %d, IsDay: %d)"), *GetName(), bSpawnOnlyAtNight, bIsDaytime);
                TimeSinceLastSpawn = 0.0f; // Reset timer even if we skipped due to condition
                return;
            }
        }

        // Check if there is room in the collision box
        if (!(IsRoomInCollisionBox())) 
        { 
            // IsRoomInCollisionBox already logs details
            TimeSinceLastSpawn = 0.0f; // Reset timer so we don't spam every frame when full
            return; 
        }

        UE_LOG(LogTemp, Log, TEXT("SpawnSystem (%s): SpawnDelay (%f) reached. Attempting to spawn %d actors of type %s"), *GetName(), SpawnDelay, SpawnCount, *SpawnableActor->GetName());
        TimeSinceLastSpawn = 0.0f;

        for (int32 i = 0; i < SpawnCount; ++i) {
            // Try up to 10 times to find a free spot
            const int32 MaxAttempts = 10;
            bool bSpawned = false;
            
            float ActorRadius = 50.0f;
            float ActorHalfHeight = 50.0f;
            bool bIsPawn = false;

            if (SpawnableActor)
            {
                if (AActor* DefaultActor = SpawnableActor->GetDefaultObject<AActor>())
                {
                    bIsPawn = DefaultActor->IsA<APawn>();
                    
                    // For Characters/Pawns, GetSimpleCollisionCylinder on CDO can return 0.
                    // We'll try to find the RootComponent's shape directly or use default values.
                    float ComponentRadius, ComponentHalfHeight;
                    DefaultActor->GetSimpleCollisionCylinder(ComponentRadius, ComponentHalfHeight);
                    
                    if (ComponentRadius <= 0.1f)
                    {
                        // Fallback: If it's a character, use standard capsule defaults if cylinder query failed
                        if (ACharacter* DefaultChar = Cast<ACharacter>(DefaultActor))
                        {
                            if (UCapsuleComponent* Capsule = DefaultChar->GetCapsuleComponent())
                            {
                                ActorRadius = Capsule->GetScaledCapsuleRadius();
                                ActorHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
                            }
                        }
                        
                        // If still 0, use safe defaults for a humanoid
                        if (ActorRadius <= 0.1f)
                        {
                            ActorRadius = 34.0f;
                            ActorHalfHeight = 88.0f;
                        }
                    }
                    else
                    {
                        ActorRadius = ComponentRadius;
                        ActorHalfHeight = ComponentHalfHeight;
                    }
                }
            }

            UE_LOG(LogTemp, Verbose, TEXT("SpawnSystem (%s): Detected ActorRadius=%f, ActorHalfHeight=%f for %s"), *GetName(), ActorRadius, ActorHalfHeight, *SpawnableActor->GetName());

            for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt)
            {
                FVector SpawnLocation = GetRandomPointInCollisionBox();
                FVector FinalLocation = SpawnLocation;
                AActor* GroundActor = nullptr;
                
                // Trace down to find the ground
                FHitResult GroundHit;
                FVector TraceStart = SpawnLocation;
                FVector TraceEnd = TraceStart - (CollisionBox->GetUpVector() * 10000.0f); // Trace down 100m
                FCollisionQueryParams TraceParams;
                TraceParams.AddIgnoredActor(this);

                if (GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
                {
                    FinalLocation = GroundHit.Location;
                    GroundActor = GroundHit.GetActor();
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("SpawnSystem (%s): Attempt %d failed - No ground hit below %s"), *GetName(), Attempt, *TraceStart.ToString());
                    continue;
                }

                // If it's a Pawn (like an enemy), the pivot is usually at the center.
                // Environment objects (rocks/trees) usually have pivots at the base.
                FVector ActualSpawnLocation = FinalLocation;
                if (bIsPawn)
                {
                    ActualSpawnLocation += CollisionBox->GetUpVector() * (ActorHalfHeight + 2.0f);
                }

                // Check for clearance at the intended location
                float CheckHeight = bIsPawn ? ActorHalfHeight : ActorRadius;
                FVector CheckLocation = FinalLocation + CollisionBox->GetUpVector() * (CheckHeight + 1.0f);

                if (IsSpawnLocationFree(CheckLocation, ActorRadius, GroundActor))
                {
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                    
                    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnableActor, ActualSpawnLocation, FRotator::ZeroRotator, SpawnParams);
                    if (SpawnedActor)
                    {
                        SetActorDifficulty(SpawnedActor);
                        bSpawned = true;
                        UE_LOG(LogTemp, Log, TEXT("SpawnSystem (%s): Successfully spawned %s at %s"), *GetName(), *SpawnableActor->GetName(), *ActualSpawnLocation.ToString());
                        break;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("SpawnSystem (%s): SpawnActor failed for %s even though location was free"), *GetName(), *SpawnableActor->GetName());
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("SpawnSystem (%s): Attempt %d failed - Location %s is blocked"), *GetName(), Attempt, *CheckLocation.ToString());
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

    // Use a larger detection height to include actors spawned on the ground below the box
    float TraceDownDistance = 10000.0f;
    float TotalDetectionHeight = DetectionHeight + TraceDownDistance;
    FVector DetectionExtent = FVector(BoxExtent.X, BoxExtent.Y, TotalDetectionHeight / 2.0f);
    
    // Offset the center so it covers the space from TraceDownDistance below the surface up to DetectionHeight above
    FVector DetectionLocation = BoxLocation + BoxRotation.RotateVector(FVector(0, 0, BoxExtent.Z + DetectionHeight - (TotalDetectionHeight / 2.0f)));

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    GetWorld()->OverlapMultiByChannel(
        Overlaps,
        DetectionLocation,
        BoxRotation,
        ECC_Visibility, 
        FCollisionShape::MakeBox(DetectionExtent),
        Params
    );

    TSet<AActor*> UniqueActors;
    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* OverlapActor = Overlap.GetActor();
        if (OverlapActor && (!SpawnableActor || OverlapActor->IsA(SpawnableActor)))
        {
            UniqueActors.Add(OverlapActor);
        }
    }

    int32 Count = UniqueActors.Num();
    
    if (Count >= MaxActorsInCollisionBox)
    {
        UE_LOG(LogTemp, Log, TEXT("SpawnSystem (%s): Population limit reached. Found %d actors of type %s. Limit is %d."), 
            *GetName(), Count, SpawnableActor ? *SpawnableActor->GetName() : TEXT("None"), MaxActorsInCollisionBox);
        
        // Log the first few actors found to help debug
        int32 LogLimit = 0;
        for (AActor* Actor : UniqueActors)
        {
            UE_LOG(LogTemp, Verbose, TEXT(" - Found: %s at %s"), *Actor->GetName(), *Actor->GetActorLocation().ToString());
            if (++LogLimit >= 3) break;
        }
    }

    return Count < MaxActorsInCollisionBox;
}

// Helper function to check if a location is free of collisions
bool ASpawnSystem::IsSpawnLocationFree(const FVector& Location, float Radius, AActor* IgnoredActor) const
{
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    if (IgnoredActor)
    {
        Params.AddIgnoredActor(IgnoredActor);
    }

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