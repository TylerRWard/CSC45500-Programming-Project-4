# CSC45500-Programming-Project-4
---
## Project Details

- **Name**: Tyler Ward  
- **Date**: May 1, 2025  
- **Platform**: macOS 
- **Errors**: None that I know of

---

## Summary
The server uses a  model with pthreads to handle multiple clients. Each client connection spawns a new thread that processes the command ("get", "add", or "clear") and updates a shared accumulator protected by a mutex. The client connects to the server, sends a user-provided command, and displays the response for "get" and "add" commands. The code is based on the code we went over in class, iserver.cpp and simple.cpp, modified to work for this project.
---

# Compilation
```bash
g++ -o server server.cpp -pthread
g++ -o client client.cpp
```
Example: ./proj4server 50000

# Running the Programs
```bash
./proj4server <port_number>
./proj4client <server_address> <port_number>
```
Example: ./proj4client 127.0.0.1 50000

The client will prompt for a command (get, clear, or add <intval>).