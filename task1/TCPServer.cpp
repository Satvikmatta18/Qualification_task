#include "TCPServer.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"

DEFINE_LOG_CATEGORY(LogServer);

ATCPServer::ATCPServer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATCPServer::BeginPlay()
{
    Super::BeginPlay();
    StartTCPListener();
    ObjectManager = NewObject<UObjectManager>();

    UWorld* World = GetWorld();
    ObjectManager->InitializeWorld(World);
}

void ATCPServer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    Main();
}

FString ATCPServer::ParseAndDispatch(FString& Message) {
    FString result;

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        FString Action;
        if (JsonObject->TryGetStringField(TEXT("action"), Action)) {
            if (Action == TEXT("add_object")) {
                result = ObjectManager->AddObject(JsonObject);
            }
        }
    }
    return result;
}

void ATCPServer::Main()
{
    if (!ClientSocket) {
        CheckForConnections();
    }
    else {
        if (ClientSocket->GetConnectionState() == SCS_Connected)
        {
            FString ReceivedMessage = ReceiveData();
            UE_LOG(LogServer, Log, TEXT("%s"), *ReceivedMessage);

            if (!ReceivedMessage.IsEmpty()) {
                FString result = ParseAndDispatch(ReceivedMessage);
                SendBackToClient(result);
            }

            // After responding to the client, close the connection and reset the socket to allow new connections
            CloseClientSocket();
        }
        else {
            CloseClientSocket();
            UE_LOG(LogServer, Log, TEXT("Client disconnected"));
        }
    }
}


bool ATCPServer::StartTCPListener()
{
    // Obtain the socket subsystem
    SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem)
    {
        UE_LOG(LogServer, Error, TEXT("Unable to get SocketSubsystem"));
        return false;
    }
    else {
        UE_LOG(LogServer, Log, TEXT("Got SocketSubsystem"));
    }

    // Create the TCP listener socket
    ServerSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TCP Server"), false);
    if (!ServerSocket)
    {
        UE_LOG(LogServer, Error, TEXT("Unable to create TCP Socket"));
        return false;
    }
    else {
        UE_LOG(LogServer, Log, TEXT("Created TCP Socket"));
    }

    // Set up the server address
    TSharedRef<FInternetAddr> ServerAddr = SocketSubsystem->CreateInternetAddr();
    ServerAddr->SetIp(TEXT("192.168.1.192"), bindSocketSuccess);
    ServerAddr->SetPort(port);

    // Bind the socket to the address
    if (!ServerSocket->Bind(*ServerAddr))
    {
        UE_LOG(LogServer, Error, TEXT("Unable to bind socket to address"));
        return false;
    }
    else {
        UE_LOG(LogServer, Log, TEXT("Socket successfully bound to address"));
    }

    // Start listening on the socket
    if (!ServerSocket->Listen(8))
    {
        UE_LOG(LogServer, Error, TEXT("Unable to start listening on socket"));
        return false;
    }
    else {
        UE_LOG(LogServer, Log, TEXT("Server started listening on port %d"), port);
    }
    return true;
}

void ATCPServer::CheckForConnections()
{
    bool Pending;
    if (ServerSocket->HasPendingConnection(Pending) && Pending)
    {
        ClientSocket = ServerSocket->Accept(TEXT("Accepted Client Connection"));
        UE_LOG(LogServer, Log, TEXT("Client connected"));
    }
}

FString ATCPServer::ReceiveData()
{
    uint32 Size;
    while (ClientSocket->HasPendingData(Size))
    {
        TArray<uint8> ReceivedData;
        ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));

        int32 Read = 0;
        ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

        // Ensure the data is null-terminated
        ReceivedData.Add(0);

        FString ReceivedString = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
        UE_LOG(LogServer, Log, TEXT("Received Data: %s"), *ReceivedString);  // Add log here to confirm data reception
        return ReceivedString;
    }
    return FString();
}

void ATCPServer::SendBackToClient(const FString& Message)
{
    if (ClientSocket && ClientSocket->GetConnectionState() == SCS_Connected)
    {
        FTCHARToUTF8 Convert(*Message);
        int32 BytesSent = 0;
        bool bSuccessful = ClientSocket->Send((uint8*)Convert.Get(), Convert.Length(), BytesSent);

        if (bSuccessful)
        {
            UE_LOG(LogServer, Log, TEXT("Sent message to client: %s"), *Message);
        }
        else
        {
            UE_LOG(LogServer, Error, TEXT("Failed to send message to client"));
        }
    }
    else
    {
        UE_LOG(LogServer, Error, TEXT("No client connected or client disconnected"));
    }
}

void ATCPServer::CloseClientSocket()
{
    if (ClientSocket)
    {
        ClientSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
        ClientSocket = nullptr;
        UE_LOG(LogServer, Log, TEXT("Client socket closed and reset"));
    }
}

void ATCPServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    CloseTCPListener();
}

bool ATCPServer::CloseTCPListener()
{
    if (ServerSocket)
    {
        ServerSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);
        ServerSocket = nullptr;
    }
    if (ClientSocket)
    {
        ClientSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
        ClientSocket = nullptr;
    }

    UE_LOG(LogServer, Log, TEXT("Sockets cleaned up"));
    return true;
}
