// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LineTraceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SJURVIVJAL_API ULineTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULineTraceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

	UPROPERTY(BlueprintReadOnly)
	FHitResult LastTraceResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceDirection;

	UFUNCTION(BlueprintCallable)
	FHitResult LineTrace(const FVector& Start, const FVector& Direction);
};
