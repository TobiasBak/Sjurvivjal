// Fill out your copyright notice in the Description page of Project Settings.


#include "TobiasLineTrace.h"
#include "Camera/CameraComponent.h"
#include "BPI_Hover.h"

// Sets default values for this component's properties
UTobiasLineTrace::UTobiasLineTrace()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 500.0f;
	bTraceFromCamera = true;
}


// Called when the game starts
void UTobiasLineTrace::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTobiasLineTrace::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FHitResult Hit = LineTrace();

	if (Hit.bBlockingHit)
	{
		LastTraceResult = Hit;
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *Hit.GetActor()->GetName()));
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<UBPI_Hover>())
		{
			IBPI_Hover::Execute_OnHover(HitActor, HitActor);
		}
	}
	else
	{
		LastTraceResult = FHitResult();
	}
}

FHitResult UTobiasLineTrace::LineTrace(float InTraceDistance)
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if (!Owner)
	{
		return FHitResult();
	}
	FVector Start = Owner->GetController()->GetPawn()->GetActorLocation();
	if (bTraceFromCamera)
	{
		if (Owner->FindComponentByClass<UCameraComponent>())
		{
			UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
			Start = Camera->GetComponentLocation();
		}
	}
	FRotator Direction = Owner->GetBaseAimRotation();
	
	float ActualDistance = (InTraceDistance >= 0.0f) ? InTraceDistance : TraceDistance;
	FVector End = ((Direction.Vector().GetSafeNormal() * ActualDistance) + Start);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner()); // Ignore the owner actor

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		// Line trace hit something
		return HitResult;

		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Green,    // Line color
			false,            // Persistent lines (false = duration, true = until level unload)
			5.0f,             // Duration in seconds
			0,                // Depth priority
			2.0f              // Line thickness
		);

	}

	// No hit
	return FHitResult();
}

