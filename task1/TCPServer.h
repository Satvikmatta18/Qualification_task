#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectManager.h"
#include "TCPServer.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogServer, Log, All);

UCLASS()
class UNREAL_5_2_FEATURE_API ATCPServer; : public AActor
{
	GENERATED_BODY()

public:
	ATCPServer();
	void Main();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	ISocketSubsystem* SocketSubsystem;
	FSocket* ServerSocket;
	FSocket* ClientSocket;
	int32 port = 12345;

	bool StartTCPListener();
	bool bindSocketSuccess;
	void CheckForConnections();

	FString ReceiveData();

	// prevent garbage collection
	UPROPERTY()
	UObjectManager* ObjectManager;

	FString ParseAndDispatch(FString& Message);

	void SendBackToClient(const FString& Message);

	bool CloseTCPListener();
};