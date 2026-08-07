# Cache Simulator 

An interactive, desktop-based simulation tool designed to model and visualize hardware memory cache behaviors. This application simulates the three primary cache mapping styles, implements various block replacement policies, and provides cycle-by-cycle execution statistics. Built with a modular C++ backend and a dynamic Qt6 graphical user interface, it serves as a powerful utility for analyzing memory subsystem performance.

---

##  Key Architectural Components

The project is structured with a highly modular object-oriented design, separating the GUI logic from the cycle-accurate cache state machine:

- **`CacheEngine`**: The core simulation coordinator. Manages configuration state, cache memory arrays (`CacheSet`/`CacheLine`), main memory representation, and replacement policies.
- **`AddressDecoder`**: Static decoder utility that parses incoming raw memory addresses into fields depending on the configured cache parameters:
  - **Tag Bits**: High-order bits identifying the block in memory.
  - **Set Index Bits**: Selects the target cache set.
  - **Block Offset Bits**: Specifies the word/byte index within the cache line.
- **`ReplacementPolicy`**: Implements block eviction decision logic when a cache collision occurs and all lines in a set are valid.
- **`MainWindow`**: Qt-based UI containing configuration forms, a live interactive grid visualization of cache lines (dirty, valid, tag, data), and simulation controls.

---

##  Supported Configurations

| Parameter | Supported Options |
| :--- | :--- |
| **Cache Mapping Styles** | • **Direct-Mapped**: One line per set; address maps to a unique line.<br>• **Fully Associative**: One set containing all lines; address can map anywhere.<br>• **K-Way Set Associative**: Cache divided into sets of size *K* (e.g., 2-way, 4-way). |
| **Replacement Policies** | • **LRU** (Least Recently Used): Evicts the line unused for the longest time.<br>• **FIFO** (First-In, First-Out): Evicts the oldest loaded block in the set.<br>• **Random**: Selects a victim line using pseudo-random distribution. |
| **Telemetry & Metrics** | • Total memory accesses<br>• Hit count & Hit Rate (%)<br>• Miss count & Miss Rate (%)<br>• Eviction count |

---

##  Build Requirements

To build and run the simulator locally, ensure you have:
- **C++ Compiler**: A compiler supporting C++17 or later (GCC, Clang, or MSVC)
- **CMake**: Version 3.21 or later
- **Qt SDK**: Qt 6.x (specifically Qt Widgets module)

### Building the Project

```bash
# Clone or navigate to the directory
cd Cache_simulator

# Configure the build files
cmake -S . -B build

# Compile the target executable
cmake --build build
```

*Note: If CMake is unable to locate your Qt installation automatically, pass the prefix path:*
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/path/to/Qt/6.x.x/msvc2019_64"
```

### Running the Simulator

- **macOS / Linux**:
  ```bash
  ./build/CacheSimulatorLab
  ```
- **Windows**:
  The executable will be inside the build directory under the configuration folder, typically:
  ```cmd
  .\build\Debug\CacheSimulatorLab.exe
  ```

---

##  How to Use

1. **Configure the Cache**:
   - Select the Cache Mapping Style.
   - Enter your cache capacity (Bytes), block size (Bytes), and associativity (for Set-Associative mode).
   - Choose the preferred Block Replacement Policy.
   - Click **Apply Configuration**. The visual grid will update to reflect the newly partitioned sets and lines.

2. **Load or Edit Trace Data**:
   - Write memory accesses directly into the trace editor or load a `.txt` file containing trace instructions.

3. **Simulate**:
   - Use **Step** to execute a single access, observing state changes, tag matches, hit/miss updates, and victim evictions.
   - Click **Run Simulation** to execute the entire trace at once and check final performance statistics.
   - Click **Reset** to clear cache data structures and performance metrics while preserving your loaded trace.

---

##  Trace Format

Input traces are parsed line-by-line. Each access must specify a Read/Write transaction type and a hexadecimal memory address:

```text
R 0x1000
W 0x1004
R 0x2000
```
- `R`: Memory Read transaction
- `W`: Memory Write transaction
- `0x...`: Hexadecimal byte address

*Blank lines are ignored. Invalid lines are caught and logged inside the UI execution log without crashing the simulator.*
