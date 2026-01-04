// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TobiasLineTrace.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SJURVIVJAL_API UTobiasLineTrace : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTobiasLineTrace();

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

	// Property for setting the object to trace from, to toggle between camera and character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceFromCamera;

	UFUNCTION(BlueprintCallable)
	FHitResult LineTrace(float InTraceDistance = -1.0f);
};
