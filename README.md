UDP Data Transfer Project

This project implements a cross-platform UDP-based client-server system using Qt for transmitting large arrays of numeric data. The system consists of a UDP server and multiple clients that can work simultaneously and asynchronously.

Server:
- Listens on a configurable UDP port (via `config.ini`)
- Handles requests from multiple clients
- Generates 1,000,000 unique `double` values in the range `[-X, X]`
- Sends data to clients in chunks (5,000 doubles per packet)
- Supports protocol versioning and responds with an error if the client version is outdated
- Writes logs to `logs/server.log`

Client:
- Connects to the server (address and port from `config.ini`)
- Sends a request 3 seconds after startup with a user-defined `double` value
- Receives data in chunks and tracks missing packets
- Sorts received data in descending order
- Saves results to `output/output_<id>.txt` and `output_<id>.bin`
- Generates a random `clientId` automatically
- Logs activity to `logs/client_<id>.log`

Build instructions:
- You must specify the path to Qt using CMAKE_PREFIX_PATH (-DCMAKE_PREFIX_PATH="/path/Qt/6.8.0/gcc_64/lib/cmake")

Use `run_clients.sh` file to test current project
