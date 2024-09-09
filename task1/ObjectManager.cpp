#include "ObjectManager.h"

void UObjectManager::InitializeWorld(UWorld* World) {
    ThisWorld = World;
}

FString UObjectManager::AddObject(TSharedPtr<FJsonObject>& JsonData)
{
    // Get the current size of the ObjectDatabase and generate a new unique ID
    int32 CurrentSize = ObjectDatabase.Num();  // Use the number of objects already in the database
    FString ObjectIDString = FString::FromInt(CurrentSize);  // This ensures unique object IDs
    FName ObjectID = FName(*ObjectIDString);

    // Ensure the World is valid
    if (!ThisWorld)
    {
        UE_LOG(LogTemp, Error, TEXT("World is not valid. Cannot spawn objects."));
        return FString("Error: World is not valid");
    }

    // Create a new object of type AMyObject
    FActorSpawnParameters SpawnParams;
    AMyObject* NewObject = ThisWorld->SpawnActor<AMyObject>(AMyObject::StaticClass(), SpawnParams);

    if (NewObject)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully spawned new object with ID: %s"), *ObjectIDString);

        // Set object properties using helper functions
        SetObjectLocation(JsonData, NewObject);  // Set random location inside 500x500x500
        SetObjectRotation(JsonData, NewObject);
        SetObjectScale(JsonData, NewObject);

        // Retrieve the static mesh component from the newly spawned object
        UStaticMeshComponent* MeshComponent = NewObject->FindComponentByClass<UStaticMeshComponent>();
        if (MeshComponent)
        {
            SetObjectMeshAndMaterial(JsonData, MeshComponent);
            SetObjectPhysics(JsonData, MeshComponent);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to find StaticMeshComponent for object with ID: %s"), *ObjectIDString);
        }

        // Add the new object to the ObjectDatabase with its unique ObjectID
        ObjectDatabase.Add(ObjectID, NewObject);

        return ObjectIDString;  // Return the ObjectID as confirmation
    }

    UE_LOG(LogTemp, Error, TEXT("Failed to spawn object with ID: %s"), *ObjectIDString);
    return FString("Error: Unable to create object");
}

FString UObjectManager::ModifyObject(TSharedPtr<FJsonObject>& JsonData)
{
    // Check if JsonData contains an "object_id" field
    if (!JsonData->HasField(TEXT("object_id")))
    {
        return FString(TEXT("Invalid: No object_id provided"));
    }

    // Retrieve the ObjectID from the JSON data
    FString ObjectIDString = JsonData->GetStringField(TEXT("object_id"));
    FName ObjectID = FName(*ObjectIDString);

    // Find the object in the ObjectDatabase
    AMyObject** FoundObject = ObjectDatabase.Find(ObjectID);
    if (FoundObject && *FoundObject)
    {
        AMyObject* Object = *FoundObject;

        // Modify object properties using helper functions
        SetObjectLocation(JsonData, Object);
        SetObjectRotation(JsonData, Object);
        SetObjectScale(JsonData, Object);

        UStaticMeshComponent* MeshComponent = Object->FindComponentByClass<UStaticMeshComponent>();
        if (MeshComponent)
        {
            SetObjectMeshAndMaterial(JsonData, MeshComponent);
            SetObjectPhysics(JsonData, MeshComponent);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to find StaticMeshComponent for object with ID: %s"), *ObjectIDString);
        }

        return ObjectIDString;  // Return the ObjectID as confirmation
    }

    return FString(TEXT("Invalid: Object not found"));
}

void UObjectManager::DeleteObject(TSharedPtr<FJsonObject>& JsonData)
{
    // Check if JsonData contains an "object_id" field
    if (!JsonData->HasField(TEXT("object_id")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid: No object_id provided"));
        return;
    }

    // Retrieve the ObjectID from the JSON data
    FString ObjectIDString = JsonData->GetStringField(TEXT("object_id"));
    FName ObjectID = FName(*ObjectIDString);

    // Find the object in the ObjectDatabase
    AMyObject** FoundObject = ObjectDatabase.Find(ObjectID);
    if (FoundObject && *FoundObject)
    {
        AMyObject* Object = *FoundObject;

        // Remove the object from the world
        if (Object)
        {
            Object->Destroy();  // Destroy the object from the game world
            UE_LOG(LogTemp, Log, TEXT("Object with ID %s deleted"), *ObjectIDString);
        }

        // Remove the object from the ObjectDatabase
        ObjectDatabase.Remove(ObjectID);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid: Object not found"));
    }
}

void UObjectManager::SetObjectLocation(TSharedPtr<FJsonObject> JsonData, AMyObject* Object)
{
    if (JsonData->HasField(TEXT("location")))
    {
        FVector Location;
        TSharedPtr<FJsonObject> LocationData = JsonData->GetObjectField(TEXT("location"));

        // Ensure the location is within 0 to 500 in all axes
        Location.X = FMath::Clamp(LocationData->GetNumberField(TEXT("x")), 0.0f, 500.0f);
        Location.Y = FMath::Clamp(LocationData->GetNumberField(TEXT("y")), 0.0f, 500.0f);
        Location.Z = FMath::Clamp(LocationData->GetNumberField(TEXT("z")), 0.0f, 500.0f);

        // Set the actor's location within the level
        Object->SetActorLocation(Location);
        UE_LOG(LogTemp, Log, TEXT("Object location set to: %s"), *Location.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Location data missing in JSON"));
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
        UE_LOG(LogTemp, Log, TEXT("Object rotation set to: %s"), *Rotation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Rotation data missing in JSON"));
    }
}

void UObjectManager::SetObjectScale(TSharedPtr<FJsonObject> JsonData, AMyObject* Object)
{
    if (JsonData->HasField(TEXT("scale")))
    {
        FVector NewScale;
        TSharedPtr<FJsonObject> ScaleData = JsonData->GetObjectField(TEXT("scale"));
        NewScale.X = ScaleData->GetNumberField(TEXT("x"));
        NewScale.Y = ScaleData->GetNumberField(TEXT("y"));
        NewScale.Z = ScaleData->GetNumberField(TEXT("z"));
        Object->SetActorScale3D(NewScale);
        UE_LOG(LogTemp, Log, TEXT("Scale set to: %s"), *NewScale.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Scale data missing in JSON"));
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
            if (Mesh)
            {
                MeshComponent->SetStaticMesh(Mesh);
                UE_LOG(LogTemp, Log, TEXT("Mesh set to: %s"), *MeshPath);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load mesh: %s"), *MeshPath);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh path missing in JSON"));
    }

    if (JsonData->HasField(TEXT("material_path")))
    {
        FString MaterialPath = JsonData->GetStringField(TEXT("material_path"));
        if (!MaterialPath.IsEmpty())
        {
            UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *MaterialPath));
            if (Material)
            {
                MeshComponent->SetMaterial(0, Material);
                UE_LOG(LogTemp, Log, TEXT("Material set to: %s"), *MaterialPath);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load material: %s"), *MaterialPath);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Material path missing in JSON"));
    }
}

void UObjectManager::SetObjectPhysics(TSharedPtr<FJsonObject> JsonData, UStaticMeshComponent* MeshComponent)
{
    if (JsonData->HasField(TEXT("physics")))
    {
        bool bEnablePhysics = JsonData->GetBoolField(TEXT("physics"));
        MeshComponent->SetSimulatePhysics(bEnablePhysics);
        MeshComponent->SetCollisionEnabled(bEnablePhysics ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
        MeshComponent->SetCollisionProfileName(bEnablePhysics ? TEXT("BlockAll") : TEXT("NoCollision"));
        UE_LOG(LogTemp, Log, TEXT("Physics set to: %s"), bEnablePhysics ? TEXT("Enabled") : TEXT("Disabled"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Physics data missing in JSON"));
    }
}
