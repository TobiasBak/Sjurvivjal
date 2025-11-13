#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DayNightCycle.h"
#include "SpawnSystem.generated.h"

UCLASS()
class SJURVIVJAL_API ASpawnSystem : public AActor
{
    GENERATED_BODY()
    
public:	
    // Sets default values for this actor's properties
    ASpawnSystem();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void SetActorDifficulty(AActor* SpawnedActor);

    FVector GetRandomPointInCollisionBox() const;
    bool IsSpawnLocationFree(const FVector& Location, float Radius) const;

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // The actor to spawn, editable in the Blueprint interface
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> SpawnableActor;

    // Delay between spawns, editable in the Blueprint interface
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnDelay;

    // Number of actors to spawn, editable in the Blueprint interface
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnCount;

    // Limit the maximum number of spawned actors
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxActorsInCollisionBox;

    // Collision box for detecting overlapping actors
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* CollisionBox;

    // Function to check if there is room in the collision box
    UFUNCTION(BlueprintCallable)
    bool IsRoomInCollisionBox() const;

    // Reference to the DayNightCycle actor
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ADayNightCycle* DayNightCycle;

    // Whether to spawn only during the day or night
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSpawnOnlyAtNight;

    // Difficulty multiplier based on day count
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DifficultyMultiplierPerDay = 0.1f;
};