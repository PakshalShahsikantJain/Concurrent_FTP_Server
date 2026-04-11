# Concurrent FTP Server

A high-performance, **multi-device**, concurrent File Transfer Protocol implementation in **C** using **POSIX Sockets** and **`fork()`-based process concurrency**. The server and clients are designed to run on **separate physical machines** across a network — any client machine can connect to the server over TCP/IP using the server's IP address and port. Each client connection is handled by a dedicated **child process** spawned via `fork()`, enabling true simultaneous multi-client support with file listing, reading, stat inspection, uploading, and downloading — all from a clean command-line interface.

---

## Tech Stack

| Layer | Technology |
|---|---|
| Language | C (C11) |
| Networking | POSIX Sockets (TCP/IP) |
| Network Model | Multi-device — server and clients on separate machines |
| Concurrency | `fork()` — child process per client |
| IPC | System V IPC (Shared Memory + Semaphores) |
| OS | Linux / Unix |

---

## Architecture Overview

```
  Machine A (e.g. 192.168.1.10)          Machine B (e.g. 192.168.1.20)
 ┌──────────────────────────────┐        ┌──────────────────────────────┐
 │   SERVER (Parent Process)    │        │          CLIENT              │
 │  bind() → listen()           │◄──────►│  connect(192.168.1.10:9000)  │
 │  accept() → fork()           │  TCP   │  send request (-ls/-cat/...) │
 │  child handles client session│        │  receive & display response  │
 │  System V IPC: shared state  │        └──────────────────────────────┘
 └──────────────────────────────┘
              ▲
              │ TCP/IP (any network: LAN / Wi-Fi / Internet)
              │
 ┌──────────────────────────────┐
 │          CLIENT              │  Machine C (e.g. 192.168.1.30)
 │  connect(192.168.1.10:9000)  │
 │  send request (-upload/...)  │
 └──────────────────────────────┘
```

- **Multi-device design**: The server and clients are completely independent binaries. The server can run on one machine (bare-metal, VM, or cloud instance) while any number of clients on different machines connect to it over the network using the server's **IP address and port**.
- The **server (parent process)** binds to a port and calls `fork()` for every incoming client connection, spawning a **dedicated child process** to handle it — enabling true concurrent multi-client support.
- The **parent process** immediately closes the connected socket and loops back to `accept()`, staying free to handle the next client.
- The **child process** handles the full client session, then exits cleanly.
- **System V IPC** (shared memory + semaphores) is used to safely share state (e.g., active connection count, access logs) across parent and child processes.
- The **client** is a standalone CLI tool that connects to the server using its IP and port, sends a structured request, and processes the response.

---

## Project Structure

```
concurrent-ftp/
├── server.c          # Server: socket setup, thread spawning, request handler
├── client.c          # Client: argument parsing, socket connect, response display
├── common.h          # Shared structs, constants, protocol definitions
├── Makefile          # Build targets for server and client
├── server_files/     # Directory served by the FTP server
└── README.md
```

---

## Network Setup

This is a **multi-device socket programming project**. The server and clients do not need to be on the same machine — they communicate over standard TCP/IP.

| Role | Machine | Requirement |
|---|---|---|
| Server | Any Linux/Unix machine | Needs a reachable IP and open port |
| Client | Any Linux/Unix machine | Needs network access to the server's IP:Port |

**Typical deployment scenarios:**

- **Same machine** (loopback): Use `127.0.0.1` as the IP — useful for local testing.
- **Local network (LAN/Wi-Fi)**: Use the server machine's local IP (e.g. `192.168.1.10`). Find it with `ip addr` or `ifconfig`.
- **Across the internet**: Use the server's public IP. Ensure the firewall/router allows inbound TCP on the chosen port.

**Firewall note:** If clients cannot connect, allow the port on the server machine:
```bash
sudo ufw allow 9000/tcp
```

---

## Build Instructions

### Prerequisites
- GCC or Clang compiler
- Linux / Unix OS
- `pthreads` library (usually pre-installed)

### Compile

```bash
# Compile both server and client
gcc server.c -o Server 
gcc client.c -o Client
```

---

## Usage

### Start the Server

```bash
./server <PORT>

# Example:
./server 9000
```

The server will start listening on the specified port and handle each incoming client in a separate thread.

---

### Client Commands

All client commands follow this pattern:

```bash
./client <IP_ADDRESS> <PORT> [COMMAND] [ARGUMENTS]
```

---

#### 1. `-ls` — List All Files on Server

Lists all files present in the server's shared directory.

```bash
./client <IP>  <PORT>  -ls

# Example:
./client 127.0.0.1  9000  -ls
```

| Argument | Value |
|---|---|
| `argv[0]` | `./client` |
| `argv[1]` | IP Address |
| `argv[2]` | Port Number |
| `argv[3]` | `-ls` |

**argc = 4**

---

#### 2. `-cat` — Display File Contents

Reads the contents of a file on the server and prints it to the console.

```bash
./client <IP>  <PORT>  -cat  <FILENAME>

# Example:
./client 127.0.0.1  9000  -cat  A.txt
```

| Argument | Value |
|---|---|
| `argv[0]` | `./client` |
| `argv[1]` | IP Address |
| `argv[2]` | Port Number |
| `argv[3]` | `-cat` |
| `argv[4]` | Target File Name |

**argc = 5**

---

#### 3. `-stat` — Display File Statistics

Fetches and displays statistical metadata of a file on the server (size, permissions, timestamps, inode, etc.) using the `stat()` syscall.

```bash
./client <IP>  <PORT>  -stat  <FILENAME>

# Example:
./client 127.0.0.1  9000  -stat  A.txt
```

| Argument | Value |
|---|---|
| `argv[0]` | `./client` |
| `argv[1]` | IP Address |
| `argv[2]` | Port Number |
| `argv[3]` | `-stat` |
| `argv[4]` | Target File Name |

**argc = 5**

**Sample Output:**
```
File: A.txt
Size: 2048       Blocks: 8      IO Block: 4096   regular file
Inode: 131072    Links: 1
Access: -rw-r--r--
Modify: 2024-06-01 10:30:00
Change: 2024-06-01 10:30:00
```

---

#### 4. Download a File from Server

Downloads a file from the server and saves it locally with a new name.

```bash
./client <IP>  <PORT>  <SERVER_FILENAME>  <LOCAL_FILENAME>

# Example:
./client 127.0.0.1  9000  Demo.txt  A.txt
```

| Argument | Value |
|---|---|
| `argv[0]` | `./client` |
| `argv[1]` | IP Address |
| `argv[2]` | Port Number |
| `argv[3]` | Server File Name (source) |
| `argv[4]` | Local File Name (destination) |

**argc = 5**

> The file `Demo.txt` from the server will be saved as `A.txt` on the client machine.

---

#### 5. `-upload` — Upload a File to Server

Uploads a local file from the client machine to the server.

```bash
./client <IP>  <PORT>  -upload  <FILENAME>

# Example:
./client 127.0.0.1  9000  -upload  A.txt
```

| Argument | Value |
|---|---|
| `argv[0]` | `./client` |
| `argv[1]` | IP Address |
| `argv[2]` | Port Number |
| `argv[3]` | `-upload` |
| `argv[4]` | File Name to upload |

**argc = 5**

---

## Command Summary

| Command | Description | argc |
|---|---|---|
| `./client <ip> <port> -ls` | List all files on server | 4 |
| `./client <ip> <port> -cat <file>` | Print file contents to console | 5 |
| `./client <ip> <port> -stat <file>` | Show file metadata/statistics | 5 |
| `./client <ip> <port> <src> <dest>` | Download file from server | 5 |
| `./client <ip> <port> -upload <file>` | Upload file to server | 5 |

---

## How Concurrency Works

1. The server (parent process) enters an **accept loop**, waiting for incoming TCP connections.
2. When a client connects, the server calls **`fork()`** to create a **child process** — an exact copy of the parent — to handle that client independently.
3. The **parent process** closes the connected client socket and immediately loops back to `accept()`, ready for the next client.
4. The **child process** closes the listening socket, handles the full client session (request parsing → processing → response), then calls `exit()`.
5. **System V IPC** (shared memory segments + semaphores) coordinates shared state (e.g., active connection counts, access logs) safely across the parent and all child processes.
6. The parent handles `SIGCHLD` to reap zombie child processes and prevent resource leaks.

```
Server (Parent Process)
     │
     ├── accept() → Client 1 → fork() → [Child Process 1: handle -ls    ]
     │                         parent closes connfd, loops back
     │
     ├── accept() → Client 2 → fork() → [Child Process 2: handle -cat   ]
     │                         parent closes connfd, loops back
     │
     └── accept() → Client 3 → fork() → [Child Process 3: handle -upload]
```

> Each child process gets its own **separate memory space**, making the design robust — a crash in one client handler cannot corrupt another client's session.

---

## Key Design Decisions

- **`fork()`-per-client model**: Each client is served by an independent child process, providing strong isolation between sessions. A misbehaving client cannot affect others.
- **Parent stays lean**: After `fork()`, the parent immediately closes the client socket and returns to `accept()` — minimising latency for the next incoming connection.
- **System V IPC**: Used for robust inter-process shared state management (shared memory + semaphore-based mutual exclusion) since child processes do not share memory with the parent after `fork()`.
- **Protocol design**: The client sends a structured request (command + optional filename) over the TCP stream; the server parses it and streams back the response.
- **Binary-safe file transfer**: Both upload and download use raw byte streaming over the socket, ensuring binary files (images, executables) transfer correctly alongside text files.

---

## Error Handling

- Invalid argument count → prints usage and exits.
- File not found on server → server returns an error message to the client.
- Connection refused → client exits with a descriptive error.
- `fork()` failure → server logs the error and continues accepting connections.

---

## Notes

- Ensure the server is running **before** launching any client command.
- Clients and the server **do not need to be on the same machine** — use the server's actual LAN or public IP address instead of `127.0.0.1` when connecting from a remote machine.
- The server's working directory (where files are served from) must have appropriate read/write permissions.
- For `-upload`, the local file must exist and be readable on the **client machine**.
- For download, the file is saved on the **client machine** in the current working directory.
- Tested on **Ubuntu Ubuntu 24.04.4 LTS LTS** with GCC 13.

---

## Author

> Built as a Systems Programming project demonstrating concurrent network server design using low-level C primitives.

---

## License

This project is intended for **educational purposes**. Feel free to use, study, and extend.
