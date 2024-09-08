import socket
import json
import random
import time

def send_data(data):
    try:
        # Define the server's IP address and port
        SERVER_IP = '-----'  # Replace with your actual server IP
        SERVER_PORT = 12345          # Replace with your actual server port

        # Create a socket object for each object send attempt
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Connect to the server
        client_socket.connect((SERVER_IP, SERVER_PORT))

        # Convert the dictionary to a JSON string
        json_data = json.dumps(data)

        # Send the JSON data
        client_socket.sendall(json_data.encode('utf-8'))

        # Receive the server's response
        response = client_socket.recv(1024)
        print('Response from server:', response.decode('utf-8'))

        # Close the socket after sending each object
        client_socket.close()

    except Exception as e:
        print(f"Error: {e}")


def create_random_object_data():
    return {
        "action": "add_object",
        "location": {
            "x": random.uniform(0, 500),  # Random location within 0-500 range
            "y": random.uniform(0, 500),
            "z": random.uniform(0, 500)
        },
        "rotation": {
            "pitch": random.uniform(0, 360),  # Random rotation
            "yaw": random.uniform(0, 360),
            "roll": random.uniform(0, 360)
        },
        "scale": {  # Random scale
            "x": random.uniform(0.5, 2.0),
            "y": random.uniform(0.5, 2.0),
            "z": random.uniform(0.5, 2.0)
        },
        "material_path": "Material'/Game/StarterContent/Materials/M_Brick_Clay_New.M_Brick_Clay_New'",
        "mesh_path": "StaticMesh'/Game/StarterContent/Shapes/Shape_Pipe_180.Shape_Pipe_180'",
        "physics": False
    }

# Main program to send multiple objects to the server
if __name__ == "__main__":
    for i in range(5):  # Example: sending 5 objects
        data = create_random_object_data()
        print(f"Sending object {i + 1} with data: {data}")
        send_data(data)
        time.sleep(1)  # Optional: Add a delay between sending objects if needed
