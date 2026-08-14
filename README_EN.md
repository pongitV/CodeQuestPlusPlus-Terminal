# CodeQuestPlusPlus-Terminal

A terminal RPG implemented in C++ with two perspectives: a raycaster-style renderer for gameplay and an "IDE Mode" that presents, at runtime, how parts of the code are functioning. The project is a learning exercise in C++ without external dependencies.

Language / Idioma: [English](README_EN.md) | [Português](README.md)

Main file: [src/Main.cpp](src/Main.cpp)

Pre-compiled executable: [bin/CodeQuestPlusPlus-Terminal.exe](bin/CodeQuestPlusPlus-Terminal.exe)

## Build with CMake Presets (Recommended)

### Prerequisites

- CMake 3.10+
- C++ Toolchain (MSVC, MinGW, or equivalent)
- Windows 10/11 (the project uses Win32 APIs in `src/Main.cpp`)

### Quick compilation

```bash
# Configure using preset (if supported)
cmake --preset default

# Compile
cmake --build --preset default
```

### Manual commands

```bash
# Configure (generates build folder)
cmake -S . -B build

# Compile
cmake --build build
```

*(Alternatively, use the scripts in `scripts/compile_start.bat` or `scripts/compilar_inicio.bat` for clean build and `scripts/compile_changes.bat` or `scripts/compilar_mudancas.bat` for incremental build on Windows).*

### Note about toolchains

The code uses Win32 calls (`ShellExecuteEx`, `IsUserAnAdmin`, etc.), therefore it is targeted at Windows. Compiling with MSVC (Visual Studio) is the most straightforward option; MinGW/MSYS2 can work depending on the environment.

## Pre-compiled executable

To run the binary available in `bin/`:

PowerShell / Windows Terminal:

```powershell
.\bin\CodeQuestPlusPlus-Terminal.exe
```

cmd.exe (ensure UTF-8):

```bat
chcp 65001
bin\CodeQuestPlusPlus-Terminal.exe
```

The game may request privilege elevation (UAC). The `ensureAdmin()` call in [src/Main.cpp](src/Main.cpp) attempts to launch an elevated instance and close the current instance.

## Project structure

```
CodeQuestPlusPlus-Terminal/
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── README_EN.md
├── scripts/
│   ├── compile_start.bat     # Clean build (EN)
│   ├── compile_changes.bat   # Incremental build (EN)
│   ├── compilar_inicio.bat   # Clean build (PT)
│   └── compilar_mudancas.bat # Incremental build (PT)
├── src/
│   ├── Main.cpp
│   ├── Core/       # Engine, Terminal, and Utilities
│   ├── Domain/     # Entities, Items, Classes, and Races
│   ├── Systems/    # Systems (Combat, Inventory, Progression)
│   ├── UI/         # Interfaces and Renderers (2D/3D Raycaster)
│   └── World/      # Maps and Scenario Transitions
├── docs/           # Documentation and Reports
└── bin/
	└── CodeQuestPlusPlus-Terminal.exe
```

### Raycaster View
- 3D Combat
- Attributes
- Inventory
- Diary
- Bestiary
- Map
- Victory/Defeat

### Terminal IDE View (SUSPENDED - IN DEVELOPMENT)
- Screens styled like a code editor with syntax highlighting (terminal colors)
- Visualization of class structures and execution flow

## Project Retrospective (Post-Mortem)

The development of this terminal version served as an in-depth exercise in C++ and software architecture, operating under extreme environment constraints. The project required overcoming significant technical hurdles:

- 3D projection engine (Raycaster) operating purely in the text terminal, rendering visuals without hardware acceleration.
- Object-oriented design with heavy use of polymorphism and dependency injection to decouple game logic from the rendering layer.
- Persistent state management (State Pattern) to transition between real-time exploration, inventory management, and turn-based combat without compromising console stability.

The decision to suspend this version and migrate the base architecture to a more flexible and efficient Framework (in a dedicated repository) is based on the technical limitations of the Windows console (CPU-bound, severe screen tearing on fast updates, framerate constraints, and buffer manipulation limitations). Moving to a true graphics API will allow leveraging the robust architectural patterns built here, with the direct benefit of control over graphics hardware.

## How to add new screens

1. Create the new screen in `src/UI/Screens/`
2. Register the new screen in the perspective manager (`src/UI/PerspectiveManager.h`/`.cpp`)
3. Compile the project

## How to move the project

The project uses `CMakePresets.json` for automatic configuration. When moving to another directory:

```bash
cmake --preset default
cmake --build --preset default
```

No need to adjust paths manually — CMake should detect project files.

## Contribution

Personal project focused on learning. For contributions, open issues and submit pull requests with small, well-described changes.

## License

This project is licensed under the GNU GPLv3 license - see the [LICENSE](LICENSE) file for details.
