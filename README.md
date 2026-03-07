# IOT Project

This is a simple C-based IoT project that implements a custom communication protocol over TCP.

## Description

The project consists of a client and a server that communicate with each other using a custom protocol. The protocol supports different message types like PING, PONG, GET, POST, etc. The project is built using CMake and is designed to be cross-platform.

## Features

- Custom communication protocol
- TCP client and server implementation
- Simple logging mechanism
- Cross-platform build system (CMake)

## Getting Started

### Prerequisites

- C compiler (GCC, Clang, etc.)
- CMake (version 3.10 or higher)
- tmux (for running the test script)

### Building

To build the project, run the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

This will create an executable named `IOT` in the `build` directory.

### Running

The executable takes two arguments: the port to listen on and the port to connect to.

To run a single instance:

```bash
./build/IOT <listen_port> <connect_port>
```

For example:

```bash
./build/IOT 8080 8081
```

## Running Tests

A script is provided to run a pair of instances that connect to each other. This is useful for testing the communication between two nodes.

To run the test script:

```bash
./run_test_pair.sh
```

This will build the project and then start two instances in a `tmux` session. The logs for each instance will be saved in the `logs` directory.

To stop the test, detach from the `tmux` session (`Ctrl-b d`) and then run:

```bash
tmux kill-session -t iot_test_session
```

## Project Structure

```
.
├── CMakeLists.txt
├── include
│   ├── connection.h
│   ├── err_codes.h
│   ├── logger.h
│   └── message.h
├── src
│   ├── connection.c
│   ├── main.c
│   └── message.c
└── tests
```

- `CMakeLists.txt`: The main CMake script for building the project.
- `include`: Contains the header files.
- `src`: Contains the source code.
- `tests`: Contains the test files.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
