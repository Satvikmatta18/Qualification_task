// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyObject.h"
#include "ObjectManager.generated.h"

/**
 *
 */
UCLASS()
class UNREAL_5_2_FEATURE_API UObjectManager : public UObject
{
	GENERATED_BODY()

public:
	void InitializeWorld(UWorld* World);
	FString AddObject(TSharedPtr<FJsonObject>& JsonData);
	FString ModifyObject(TSharedPtr<FJsonObject>& JsonData);
	void DeleteObject(TSharedPtr<FJsonObject>& JsonData);

private:
	UWorld* ThisWorld;
	TMap<FName, AMyObject*> ObjectDatabase;

	void SetObjectLocation(TSharedPtr<FJsonObject> JsonData, AMyObject* Object);
	void SetObjectRotation(TSharedPtr<FJsonObject> JsonData, AMyObject* Object);
	void SetObjectScale(TSharedPtr<FJsonObject> JsonData, AMyObject* Object);
	void SetObjectMeshAndMaterial(TSharedPtr<FJsonObject> JsonData, UStaticMeshComponent* MeshComponent);
	void SetObjectPhysics(TSharedPtr<FJsonObject> JsonData, UStaticMeshComponent* MeshComponent);
};