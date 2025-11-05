import requests

SERVER_URL = "http://localhost:8080"

def send_request(method, url, data=None):
    try:
        if method == "GET":
            response = requests.get(url)
        elif method == "POST":
            response = requests.post(url, json=data)
        elif method == "DELETE":
            response = requests.delete(url)
        else:
            print("Unknown request method.")
            return

        print(response.status_code, response.text)
        
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

while True:
    try:
        command_input = input("Enter command (p | c key val | r key | d key | q): ").split()
        
        if not command_input:
            continue

        command = command_input[0].lower()

        if command == 'p':
            send_request("GET", f"{SERVER_URL}/ping")

        elif command == 'c':
            if len(command_input) == 3:
                key = command_input[1]
                val = command_input[2]
                payload = {"key": key, "value": val}
                send_request("POST", f"{SERVER_URL}/kv", payload)
            else:
                print("Usage: c key val")

        elif command == 'r':
            if len(command_input) == 2:
                key = command_input[1]
                send_request("GET", f"{SERVER_URL}/kv/{key}")
            else:
                print("Usage: r key")

        elif command == 'd':
            if len(command_input) == 2:
                key = command_input[1]
                send_request("DELETE", f"{SERVER_URL}/kv/{key}")
            else:
                print("Usage: d key")
                
        elif command == 'q':
            print("Exiting client.")
            break

        else:
            print(f"Unknown command: {command}")

    except Exception as e:
        print(f"Error: {e}")