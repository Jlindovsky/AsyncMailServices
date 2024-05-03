# Post Office Simulation with Semaphores

This program simulates a post office scenario where there are three types of processes: the main process, postal clerks, and customers. Each customer visits the post office to perform one of three types of tasks: postal services for letters, parcels, or money. The customers are served based on the type of service they require.

## Problem Description

The problem is inspired by Allen B. Downey's book, "The Little Book of Semaphores," specifically the barbershop problem. The main goal is to synchronize the interactions between postal clerks and customers efficiently.

## Features

- Three types of processes: main process, postal clerks, and customers.
- Customers can request postal services for letters, parcels, or money.
- Postal clerks serve customers based on the type of service they require.
- Synchronization using semaphores to manage customer queues and clerk availability.
- Simulation of customer arrival, service requests, and clerk availability.

## How to Run

1. Compile the program using the provided source code.
2. Execute the compiled binary file to start the simulation.
3. The program will simulate customer arrivals, service requests, and clerk availability based on predefined parameters.
4. The simulation will continue until the specified closing time is reached.
5. After closing, the program will print the final state of the customer queues.

## Requirements

- C compiler
- POSIX-compatible operating system
- Standard C libraries
- `semaphore.h` library for semaphore synchronization
- `unistd.h` library for usleep function
- `stdlib.h` library for random number generation
- `sys/types.h`, `sys/ipc.h`, `sys/shm.h` libraries for shared memory management

## Configuration

- `NZ`: Number of customers
- `NU`: Number of postal clerks
- `TZ`: Maximum time for customer service (in microseconds)
- `TU`: Maximum time for clerk breaks (in microseconds)
- `F`: Closing time for the post office (in microseconds)
