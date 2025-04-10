# SimOS – Operating System Simulation in C++

**SimOS** is a simplified simulation of a multitasking operating system, developed in C++ for educational purposes. It emulates core OS functionalities such as process scheduling, disk I/O, memory paging, and system call handling — all within a command-line environment.

This project was developed as a final assignment for a systems programming course and aimed to replicate internal OS behavior using object-oriented design and manual memory control.

---

## 🧠 Features

- **Process Scheduling**:  
  Round-robin CPU scheduling using a first-come-first-serve ready queue with support for forking, waiting, and termination (including zombie and cascading termination logic).

- **Disk I/O Simulation**:  
  Supports multiple disks with I/O queues. Simulates file read requests and tracks currently active jobs.

- **Memory Paging**:  
  Simulates a RAM system with paging using Least Recently Used (LRU) replacement strategy. Manages frames and tracks per-process memory usage.

- **System Calls & Process Lifecycle**:  
  Implements commands such as `NewProcess`, `SimFork`, `SimExit`, `SimWait`, and `TimerInterrupt`.

- **CPU & Queue Management APIs**:  
  Includes getter methods to inspect system state: current CPU process, ready queue, memory layout, and disk queues.

---

## 🛠 Technologies Used

- **C++17**
- STL containers: `std::deque`, `std::vector`, `std::map`
- Command-line compilation via `g++`
- Linux-compatible build (tested on university lab machines)

---

## 🚀 Build Instructions

```bash
g++ -std=c++17 *.cpp -o runme

Run the resulting binary with:
./runme
```
📂 File Structure
SimOS.h – Public class interface used by the test driver

SimOS.cpp – Class implementation (process control, memory, disks)

main.cpp – Optional driver for testing functionality

README.md – You’re here!

⚠️ Note
This project was designed to simulate OS behavior — no actual memory, hardware, or kernel code is used. While the final implementation encountered limitations and did not pass all autograder test cases, it served as a valuable deep-dive into systems-level programming, process control logic, and C++ memory handling.

📬 Contact:
Jeremy Almonte
📧 jeremya1029@gmail.com
