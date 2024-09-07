#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectManager.h"
#include "TCPServer.generated.h"

// Declare a log category for the server
DECLARE_LOG_CATEGORY_EXTERN(LogServer, Log, All);

UCLASS()
class UNREAL_5_2_FEATURE_API ATCPServer : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	ATCPServer();

	// Main function to be called for TCP logic
	void Main();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Socket subsystem for managing sockets
	ISocketSubsystem* SocketSubsystem;

	// Server and client socket
	FSocket* ServerSocket;
	FSocket* ClientSocket;

	// Port for the TCP listener
	int32 port = 12345;

	// Flag to track if socket binding was successful
	bool bindSocketSuccess;

	// Function to start the TCP listener
	bool StartTCPListener();

	// Check for new connections
	void CheckForConnections();

	// Receive data from the client
	FString ReceiveData();

	// Reference to the object manager (prevent garbage collection)
	UPROPERTY()
	UObjectManager* ObjectManager;

	// Parse the received message and dispatch actions
	FString ParseAndDispatch(FString& Message);

	// Send a response back to the client
	void SendBackToClient(const FString& Message);

	// Close the TCP listener
	bool CloseTCPListener();
};
