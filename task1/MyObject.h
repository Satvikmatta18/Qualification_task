// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyObject.generated.h"

UCLASS()
class UNREAL_5_2_FEATURE_API AMyObject : public AActor
{
	GENERATED_BODY()

public:
	AMyObject();
	USceneComponent* RootComp;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};