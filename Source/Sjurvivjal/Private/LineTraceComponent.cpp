// Fill out your copyright notice in the Description page of Project Settings.


#include "LineTraceComponent.h"

// Sets default values for this component's properties
ULineTraceComponent::ULineTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULineTraceComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}


// Called every frame
void ULineTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	APawn* Owner = Cast<APawn>(GetOwner());
	if (Owner)
	{

	}

	// ...
}



FHitResult ULineTraceComponent::LineTrace(const FVector& Start, const FVector& Direction)
{
	FVector End = ((Direction.GetSafeNormal() * TraceDistance) + Start);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner()); // Ignore the owner actor

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		// Line trace hit something
		return HitResult;
	}

	// No hit
	return FHitResult();
}