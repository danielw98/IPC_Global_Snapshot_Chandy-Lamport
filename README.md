# IPC Global Snapshot - Chandy-Lamport Algorithm

This project simulates a distributed system using UNIX processes that communicate through pipes in a non-blocking manner. It implements the Chandy-Lamport algorithm for capturing a global snapshot of the system state. The paper is included in `ON DISTRIBUTED SNAPSHOTS.pdf`

## Demo Output

![Chandy-Lamport Output](https://github.com/danielw98/IPC_Global_Snapshot_Chandy-Lamport/blob/main/GlobalSnapshot.png?raw=true)


## Features

- **Distributed System Simulation**: Utilizes UNIX processes to mimic a distributed system's operations.
- **Non-Blocking Communication**: Processes communicate through pipes that are read from only if data is written, ensuring non-blocking operations.
- **Global Snapshot**: Initiates a snapshot across the system with a keypress, preserving the global state as described by the Chandy-Lamport algorithm.

## Getting Started

To get the system up and running:

1. Clone the repository:
    ```bash
    git clone https://github.com/DanielW98/IPC_Global_Snapshot_Chandy-Lamport.git
    ```
2. Navigate to the project directory:
    ```bash
    cd IPC_Global_Snapshot_Chandy-Lamport
    ```
3. Use the provided Makefile to compile the project:
    ```bash
    make
    ```
4. Execute the program:
    ```bash
    ./main
    ```

## Usage

Once the program is running:
- Press **ENTER** or **Space** to initiate a new snapshot.
- Press **'q'** or **'Q'** to command the program to take a final snapshot and quit.

The system will fork processes, simulate message passing, and take snapshots upon the appropriate keypresses.

## Dive Into the Code

The IPC Global Snapshot project implements the Chandy-Lamport algorithm for taking global snapshots of a distributed system. It's designed to work under UNIX, simulating a distributed system using processes that communicate through pipes in a non-blocking fashion. The core files are under `src/` and headers under `includes/`.

### Core Files
- **main.cpp**: Initializes the processes and listens for user input to initiate snapshots or quit the program.
- **process.cpp**: Defines the `Process` class, detailing the behavior of each node in the distributed system, including message handling and local state updates.
- **processcontroller.cpp**: Manages all `Process` instances, orchestrating the snapshot taking and termination of the system.
- **message.cpp**: Manages the construction and communication of messages between processes.
- **snapshot.cpp**: Contains the logic to record the state of a process as part of the global snapshot.

### Headers
- **process.h**: Declares the `Process` class, including methods for the main process loop and inter-process communication.
- **message.h**: Declares the `Message` class for message creation and handling between channels.
- **snapshot.h**: Declares the `Snapshot` class, responsible for snapshot initiation and data collection.
- **enums.h**: Defines enums used across the project to represent message types and process states.

### Interaction and Control
Key interactions are controlled via user input:
- Pressing **Enter** initiates a new snapshot at the given time.
- Pressing **'q'** commands the system to take a final snapshot and terminate all processes.

For detailed usage and to check the specific keys for actions, refer to the source code's comments or the in-code documentation.

The project structure emphasizes modularity and the importance of inter-process communication and synchronization in distributed systems, reflecting real-world scenarios, such as taking a snapshot which saves the global state at the given time.

## Contributing

This is an open-source project, and contributions are welcome. To contribute, please fork the repository, make your changes, and submit a pull request with a clear description of your modifications.

## License

This project is licensed under the MIT License. For full license text, see the [LICENSE](LICENSE) file.