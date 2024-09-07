// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectManager.h"

void UObjectManager::InitializeWorld(UWorld* World) {
    ThisWorld = World;
}


FString UObjectManager::AddObject(TSharedPtr<FJsonObject>& JsonData)
{
    int32 CurrentSize = ObjectDatabase.Num();
    FString CameraIDString = FString::FromInt(CurrentSize);

    // ----- include your code here -----


    // ----- include your code here -----

    return CameraIDString;
}


FString UObjectManager::ModifyObject(TSharedPtr<FJsonObject>& JsonData) {
    // TODO: implement modify object so that we can change the object_id's properties
    return FString();
}

void UObjectManager::DeleteObject(TSharedPtr<FJsonObject>& JsonData)
{
    // TODO: implement delete object while keeping keeping objectID in ObjectDatabase consistent
}

void UObjectManager::SetObjectLocation(TSharedPtr<FJsonObject> JsonData, AMyObject* Object)
{
    if (JsonData->HasField(TEXT("location")))
    {
        FVector Location;
        TSharedPtr<FJsonObject> LocationData = JsonData->GetObjectField(TEXT("location"));
        Location.X = LocationData->GetNumberField(TEXT("x"));
        Location.Y = LocationData->GetNumberField(TEXT("y"));
        Location.Z = LocationData->GetNumberField(TEXT("z"));
        Object->SetActorLocation(Location);
    }
}

void UObjectManager::SetObjectRotation(TSharedPtr<FJsonObject> JsonData, AMyObject* Object)
{
    if (JsonData->HasField(TEXT("rotation")))
    {
        FRotator Rotation;
        TSharedPtr<FJsonObject> RotationData = JsonData->GetObjectField(TEXT("rotation"));
        Rotation.Pitch = RotationData->GetNumberField(TEXT("pitch"));
        Rotation.Yaw = RotationData->GetNumberField(TEXT("yaw"));
        Rotation.Roll = RotationData->GetNumberField(TEXT("roll"));
        Object->SetActorRotation(Rotation);
    }
}

void UObjectManager::SetObjectScale(TSharedPtr<FJsonObject> JsonData, AMyObject* Object) {
    if (JsonData->HasField(TEXT("scale"))) {
        FVector NewScale;
        TSharedPtr<FJsonObject> ScaleData = JsonData->GetObjectField(TEXT("scale"));
        NewScale.X = JsonData->GetNumberField(TEXT("x"));
        NewScale.Y = JsonData->GetNumberField(TEXT("y"));
        NewScale.Z = JsonData->GetNumberField(TEXT("z"));
        Object->SetActorScale3D(NewScale);
        UE_LOG(LogTemp, Log, TEXT("Scale: %d %d %d"), NewScale.X, NewScale.Y, NewScale.Z);
    }
}

void UObjectManager::SetObjectMeshAndMaterial(TSharedPtr<FJsonObject> JsonData, UStaticMeshComponent* MeshComponent)
{
    if (JsonData->HasField(TEXT("mesh_path")))
    {
        FString MeshPath = JsonData->GetStringField(TEXT("mesh_path"));
        if (!MeshPath.IsEmpty())
        {
            UStaticMesh* Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
            MeshComponent->SetStaticMesh(Mesh);
        }
    }

    if (JsonData->HasField(TEXT("material_path")))
    {
        FString MaterialPath = JsonData->GetStringField(TEXT("material_path"));
        if (!MaterialPath.IsEmpty())
        {
            UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *MaterialPath));
            MeshComponent->SetMaterial(0, Material);
        }
    }
}

void UObjectManager::SetObjectPhysics(TSharedPtr<FJsonObject> JsonData, UStaticMeshComponent* MeshComponent)
{
    if (JsonData->HasField(TEXT("physics")))
    {
        if (JsonData->GetBoolField(TEXT("physics")))
        {
            MeshComponent->SetSimulatePhysics(true);
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
        }
    }
}