# Cryptographic Client-Server Project

This project implements the _client_ part of the Outsourced Computation Protocol (OCP), based on my master thesis. The _client_ generates and signs data and sends it to the _server_ for computation. The key generation and signing operations are from the RELIC implementation. 

## Features

- **Signing** Signs data points using the MKLHS in RELIC.
- **JSON Serialization:** Uses JSON for structured data exchange.
- **Key Generation:** Secure generation of secret and public keys using RELIC.
- **Base64 Encoding/Decoding:** For safe transmission of binary cryptographic data.
- **HTTP Communication:** Custom HTTP GET/POST requests and response parsing using sockets.
- **Data Handling:** Creation, encoding, and transmission of data points and cryptographic signatures.

## Directory Structure
- core: contains all the core functionallity of the client.
    - crypto: contains cryptographic functions.
        - mklhs: contains the implementation of the MKLHS.
    - message: contains functions for handling messages.
    - request: contains functions for handling requests.
    - send: contains functions for sending requests.
    - utils: contains utility functions for string handling, base64 encoding, and parsing.
- data: data folder for storing data files.
- scripts: contains script for sampling memory.
- testing: contains testing functions. 
```sh
.
├── client.c
├── core
│   ├── crypto
│   │   └── mklhs
│   │       ├── mklhs.c
│   │       └── mklhs.h
│   ├── message
│   │   ├── message.c
│   │   └── message.h
│   ├── request
│   │   ├── json.c
│   │   ├── json.h
│   │   ├── request.c
│   │   └── request.h
│   ├── send
│   │   ├── send.c
│   │   └── send.h
│   └── utils
│       ├── bad_string.c
│       ├── bad_string.h
│       ├── base64.c
│       ├── base64.h
│       ├── utils.c
│       └── utils.h
├── data
├── Makefile
├── README.md
├── scripts
│   └── uss.sh
└── testing
    ├── testing.c
    └── testing.h
```
## Dependencies
**RELIC** is the cryptographic library running the core functionalites and the _client_ needs it to run. 
You can find the RELIC build instructions here [here](https://github.com/relic-toolkit/relic/wiki/Building).
Other than that you need standard gcc and standard C libraries. 

## Configure 
- The _client_ is configured to run 1000 iterations, you can change these values in the `client.c` file (`int iterations_count = 1000;`).
- The _client_ is configured to generate 30 data points in each message, you can change this value in the `messsage.h` with `NUM_DATA_POINTS`.
- The _client_ is configured to use `doubling` as function for evaluation, this can be changed in the `message.h` file to `averaging`.
- The _client_ is pre-configured to send request to the IP defined by `setup_POST(request, sockfd, &req, json.buffer, "/new", SERVER_IP)`, you can change these values in the `send.h` file: 
    - `#define SERVER_IP "your server IP"`
    - `#define LOCAL_SERVER_IP "your local server IP"` 
    - `#define SERVER_PORT "your server port"`

**Note:** You need to change the IP to your correct server configuration if you want it to work. All other is not necessary to change.   

## Compilation
To compile the client, use the provided `Makefile`. Run the following command in `client\` in the terminal:

```sh
make
```
This will compile the `client.c` file and link it with the necessary libraries and h files. 
You can run testing client by running the following command:

```sh
make test
```
Clean up the compiled files by running:

```sh
make clean
```
## Usage
To run the client, execute the compiled binary:

```sh
./client
```
This will start the client, which will generate keys, sign data, and send requests to the _server_ as per the OCP protocol.
