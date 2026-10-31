# ForgeOS – Linux Dynamic Memory Allocation and Memory Monitoring System

<p align="center">

### A Linux-Based Memory Allocation Simulator with an Interactive Monitoring Dashboard

**[🌐 Live Dashboard](https://tech-titans-ossp.vercel.app)** | **[📂 GitHub Repository](https://github.com/tallurihimabindu342-design/TechTitans_OSSP)**

</p>

---

## 1. Project Overview

The **Linux Dynamic Memory Allocation and Memory Monitoring System** is a C-based user-space memory allocation simulator developed as part of the Operating System and System Programming (OSSP) project.

The system demonstrates dynamic memory allocation and deallocation using three allocation strategies: First Fit, Best Fit, and Worst Fit.

It also provides memory utilization monitoring, allocation tracking, fragmentation analysis, performance benchmarking, and an interactive web dashboard.

The project integrates a C-based memory allocator with a Node.js backend and React frontend to provide a visual representation of memory management operations.

## 2. Problem Statement

Memory management is one of the important responsibilities of an operating system. Inefficient memory allocation can result in memory wastage, fragmentation, and poor memory utilization.

Different allocation strategies have different effects on memory usage and allocation success.

This project addresses these challenges by implementing and comparing memory allocation strategies and providing tools to monitor memory utilization and analyze fragmentation.

## 3. Project Objectives

- Implement dynamic memory allocation and deallocation.
- Develop First Fit, Best Fit, and Worst Fit allocation algorithms.
- Track allocated and released memory blocks.
- Monitor memory utilization and available memory.
- Identify and analyze internal and external fragmentation.
- Compare allocation strategies using workload-based benchmarks.
- Monitor actual Linux system memory information.
- Visualize memory allocation and monitoring results through a web dashboard.

## 4. Key Features

### 4.1 Dynamic Memory Allocation

- Allocate memory dynamically.
- Free previously allocated memory.
- Reallocate existing memory.
- Assign unique allocation IDs.
- Track active memory allocations.

### 4.2 Memory Allocation Algorithms

The system implements three allocation strategies:

- First Fit (FF)
- Best Fit (BF)
- Worst Fit (WF)

### 4.3 Allocation Modes

- **Contiguous Allocation:** Allocates a request within a suitable continuous memory block.
- **Non-Contiguous Allocation:** Allows a request to be distributed across multiple available memory blocks.

### 4.4 Fragmentation Analysis

- Internal fragmentation calculation.
- External fragmentation calculation.
- Total free memory analysis.
- Largest available free block identification.

### 4.5 Memory Monitoring

- Total memory.
- Used memory.
- Available memory.
- Memory utilization percentage.
- Active allocation statistics.
- Free block information.
- Linux system memory monitoring using `/proc/meminfo`.

### 4.6 Performance Benchmarking

- Execute allocation workloads.
- Compare allocation strategies.
- Count successful and failed allocation requests.
- Display benchmark comparisons.

### 4.7 Interactive Web Dashboard

- Memory allocation visualization.
- Active allocation information.
- Memory utilization statistics.
- Fragmentation analysis.
- Benchmark comparison graphs.
- System memory monitoring.
- Backend-connected memory information.

## 5. Technologies Used

| Component | Technologies |
|---|---|
| Programming Language | C |
| Operating System | Linux |
| Memory Functions | malloc(), free(), realloc() |
| System Monitoring | `/proc/meminfo` |
| Backend | Node.js, Express.js |
| Frontend | React.js, JavaScript |
| Development Tool | Vite |
| Communication | REST APIs |
| Data Exchange | JSON |
| Version Control | Git, GitHub |
| Deployment | Vercel |

## 6. System Architecture

The project follows a layered architecture connecting the memory allocator to the web dashboard.

```text
                USER
                  |
                  v
          REACT WEB DASHBOARD
                  |
                  v
           NODE.JS BACKEND
                  |
                  v
             REST APIs
                  |
                  v
          MEMORY STATUS JSON
                  |
                  v
          C MEMORY ALLOCATOR
                  |
          -------------------
          |        |        |
        FIRST     BEST    WORST
         FIT      FIT      FIT
          |        |        |
          -------------------
                  |
                  v
         MEMORY ALLOCATION
          AND DE-ALLOCATION
                  |
                  v
        FRAGMENTATION ANALYSIS
                  |
                  v
         PERFORMANCE TESTING
                  |
                  v
          DASHBOARD UPDATES
```

The C application performs memory allocation operations and exports simulator information into a JSON file.

The Node.js backend reads the generated information and provides it through API endpoints.

The React dashboard communicates with the backend and displays the received memory information.

Actual Linux system memory information is collected separately through `/proc/meminfo`.

## 7. Memory Allocation Algorithms

### 7.1 First Fit

First Fit searches the available memory blocks from the beginning and selects the first block that is large enough to satisfy the allocation request.

**Advantages:**
- Simple to implement.
- Faster search in many cases.

**Limitation:**
- Can leave small unused memory blocks.

### 7.2 Best Fit

Best Fit searches for the smallest available memory block that can accommodate the requested memory.

**Advantages:**
- Attempts to minimize leftover space in the selected block.

**Limitation:**
- May require searching through more available blocks.

### 7.3 Worst Fit

Worst Fit selects the largest available memory block for an allocation request.

**Advantages:**
- Leaves a relatively large remaining free block after allocation.

**Limitation:**
- Can consume large blocks that may be useful for future requests.

## 8. Fragmentation Analysis

### 8.1 Internal Fragmentation

Internal fragmentation occurs when the allocated memory is greater than the requested memory.

**Example:**

| Description | Size |
|---|---:|
| Requested Memory | 23 KB |
| Allocated Memory | 30 KB |
| Internal Fragmentation | 7 KB |

The simulator uses a 10 KB allocation unit for this calculation.

Formula:

`Internal Fragmentation = Allocated Memory - Requested Memory`

### 8.2 External Fragmentation

External fragmentation occurs when free memory is available but distributed across separate blocks.

**Example:**

| Description | Size |
|---|---:|
| Free Block 1 | 30 KB |
| Free Block 2 | 30 KB |
| Total Free Memory | 60 KB |
| Largest Free Block | 30 KB |
| External Fragmentation | 30 KB |

Formula:

`External Fragmentation = Total Free Memory - Largest Free Block`

## 9. Performance Benchmarking

The system includes a benchmark feature to compare First Fit, Best Fit, and Worst Fit using allocation workloads.

The benchmark evaluates:

- Successful allocation requests.
- Failed allocation requests.
- Allocation behavior under different memory configurations.
- Differences between allocation strategies.

Example workload-specific results:

| Algorithm | Successful Requests | Failed Requests |
|---|---:|---:|
| First Fit | 8 | 1 |
| Best Fit | 9 | 0 |
| Worst Fit | 8 | 1 |

*Note: These are example results from a particular contiguous allocation workload. Results may change depending on memory size, allocation sequence, and configuration.*

## 10. Memory Monitoring

The project monitors two different types of memory information.

**Simulator Memory Monitoring**

Tracks the configured memory pool, including:
- Allocated memory.
- Free memory.
- Active allocations.
- Largest free block.
- Fragmentation statistics.

**Linux System Memory Monitoring**

Uses the Linux `/proc/meminfo` interface to obtain actual system memory information, including:
- Total system memory.
- Used memory.
- Available memory.
- Memory utilization.

The simulator memory pool and actual Linux system memory are separate measurements.

## 11. Project Directory Structure

```text
TechTitans_OSSP/
|
└── ForgeOS/
    |
    ├── README.md
    ├── OSSP ABSTARCT.pdf
    |
    ├── Shellforge/
    |   └── README.md
    |
    ├── NanoKernel/
    |   └── README.md
    |
    ├── linux-memory-project/
    |   |
    |   ├── main.c
    |   ├── allocator.c
    |   ├── allocator.h
    |   └── .gitignore
    |
    └── linux-memory-dashboard/
        |
        ├── src/
        |   └── App.jsx
        |
        ├── server/
        |   └── server.js
        |
        ├── index.html
        ├── package.json
        ├── package-lock.json
        └── .gitignore
```

## 12. Installation and Setup

### Prerequisites

Make sure the following tools are installed:

- Linux or Ubuntu environment.
- GCC compiler.
- Node.js.
- npm.
- Git.

### Step 1: Clone the Repository

```bash
git clone https://github.com/tallurihimabindu342-design/TechTitans_OSSP.git
```

Navigate to the project directory:

```bash
cd TechTitans_OSSP/ForgeOS
```

### Step 2: Compile the C Program

```bash
cd linux-memory-project
```

Compile the source files:

```bash
gcc -Wall -Wextra -std=c11 -o allocator main.c allocator.c
```

### Step 3: Run the Memory Allocator

```bash
./allocator
```

The program asks the user to select:

- Total memory pool size.
- Allocation strategy.
- Allocation mode.

### Step 4: Install Dashboard Dependencies

Open another terminal:

```bash
cd ~/LinuxLab/TechTitans_OSSP/ForgeOS/linux-memory-dashboard
```

Install dependencies:

```bash
npm install
```

### Step 5: Start the Backend

Open another terminal:

```bash
cd ~/LinuxLab/TechTitans_OSSP/ForgeOS/linux-memory-dashboard/server
```

Run the backend:

```bash
node server.js
```

The backend is configured to run on port `5000` in the local setup.

### Step 6: Start the Frontend

Open another terminal:

```bash
cd ~/LinuxLab/TechTitans_OSSP/ForgeOS/linux-memory-dashboard
```

Start the development server:

```bash
npm run dev
```

Open the local URL displayed in the terminal, normally:

`http://localhost:5173`

## 13. C Application Menu

The memory allocator provides the following interactive menu:

| Option | Operation |
|---|---|
| 1 | Allocate Memory |
| 2 | Free Memory |
| 3 | Display Memory Status |
| 4 | Fragmentation Analysis |
| 5 | Reallocate Memory |
| 6 | Visualize Memory |
| 7 | Memory Monitoring |
| 8 | Exit |
| 9 | Run Allocation Benchmark |

## 14. Backend API Endpoints

The Node.js backend provides the following REST API endpoints:

| Endpoint | Purpose |
|---|---|
| `/api/health` | Checks backend availability |
| `/api/memory` | Retrieves allocator memory information |
| `/api/system-memory` | Retrieves Linux system memory information |

The frontend uses these endpoints to retrieve information and display it on the dashboard.

## 15. Testing and Validation

The system can be tested using different memory configurations and allocation workloads.

Important test cases include:

1. Allocate memory within available capacity.
2. Request memory greater than available capacity.
3. Free an allocated memory block.
4. Reallocate an existing memory block.
5. Compare First Fit, Best Fit, and Worst Fit.
6. Test contiguous allocation.
7. Test non-contiguous allocation.
8. Analyze internal fragmentation.
9. Analyze external fragmentation.
10. Run performance benchmarks.
11. Monitor memory utilization.
12. Verify that allocation changes are reflected on the dashboard.

## 16. Deployment

The React/Vite frontend is deployed using Vercel.

<details>
<summary>Live Project Information</summary>

**Live Dashboard:**

https://tech-titans-ossp.vercel.app

**GitHub Repository:**

https://github.com/tallurihimabindu342-design/TechTitans_OSSP

</details>

The deployed dashboard depends on the configured backend and data source for live memory information.

## 17. Team Members and Contributions

**Team Name:** Team 4

| S. No. | Team Member | Responsibilities |
|---|---|---|
| 1 | **B. Siri** | Memory allocator implementation, First Fit, Best Fit and Worst Fit algorithms, memory tracking and fragmentation analysis. |
| 2 | **K. Bhavana** | Linux `/proc` monitoring, memory utilization analysis, peak memory monitoring and system memory statistics. |
| 3 | **T. Himabindu** | Performance analysis, workload-based testing, algorithm comparison, fragmentation evaluation, report generation, integration and validation. |

## 18. Expected Outcomes

The project aims to provide:

- A functional C-based memory allocation simulator.
- Implementation of three memory allocation algorithms.
- Dynamic allocation and deallocation.
- Memory block tracking.
- Internal and external fragmentation analysis.
- Linux system memory monitoring.
- Workload-based performance comparison.
- An interactive web-based memory monitoring dashboard.

## 19. Limitations

- The allocator is a user-space memory allocation simulator, not a modification to the Linux kernel memory allocator.
- The configured simulator memory pool is separate from actual system memory.
- System memory information is collected separately through `/proc/meminfo`.
- Benchmark results depend on the selected workload and configuration.
- Live dashboard updates depend on the availability of the configured backend and data source.
- Automatic memory leak detection and kernel-level memory allocation are not claimed as implemented features.

## 20. Future Enhancements

- Additional memory allocation strategies.
- More advanced performance benchmarking.
- Enhanced memory leak detection.
- Historical memory usage reports.
- Advanced memory visualization.
- Automated allocation workload testing.
- Further integration with Linux memory monitoring interfaces.

## 21. References

- Linux Manual Pages: https://man7.org/linux/man-pages/
- GCC Documentation: https://gcc.gnu.org/documentation.html
- Node.js Documentation: https://nodejs.org/docs/
- React Documentation: https://react.dev/
- Vite Documentation: https://vite.dev/
- Vercel Documentation: https://vercel.com/docs

---

## Project Information

| Detail | Information |
|---|---|
| Project Title | Linux Dynamic Memory Allocation and Memory Monitoring System |
| Subject | Operating System and System Programming (OSSP) |
| Course Code | 25CS2104E |
| Academic Year | 2026–2027 |
| Term | I |
| Team | Team 4 |

---

<p align="center">

**Developed as part of the OSSP Academic Project – 2026–2027**

</p>
