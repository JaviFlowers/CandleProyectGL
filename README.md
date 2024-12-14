
# Flame Rendering Simulation Using OpenGL - ReadMe

## Overview
This project demonstrates realistic flame rendering using a layered particle system and advanced shading techniques inspired by SIGGRAPH papers. The application uses OpenGL to render a scene with a candle and a dynamic flame, illuminating the room with realistic lighting effects.

## Features
- Multi-layered particle system for flame rendering.
- Gradient-based particle shading to simulate temperature variations in the flame.
- Physically inspired lighting with dynamic attenuation.
- Normal mapping for enhanced surface detail on the candle and room.

---

## Requirements
To build and run the project, ensure the following are installed:

### Software Dependencies
1. **C++ Compiler**:
   - GCC (MinGW for Windows)
   - Clang
   - Visual Studio (with C++ support)

2. **Libraries**:
   - OpenGL (Version 3.3 or later)
   - GLFW (Window and input handling)
   - GLAD (OpenGL function loader)
   - stb_image (Image loading for textures)

3. **Development Environment**:
   - Code editor/IDE (e.g., Visual Studio Code, CLion, or Visual Studio)

### Hardware Requirements
- A GPU capable of OpenGL 3.3 or higher.

---

## Project Structure
```
project/
|-- include/           # Header files
|   |-- ParticleEmitter.h
|   |-- CandleModel.h
|   |-- RoomModel.h
|   |-- Shader.h
|-- src/               # Source files
|   |-- ParticleEmitter.cpp
|   |-- CandleModel.cpp
|   |-- RoomModel.cpp
|   |-- Shader.cpp
|   |-- main.cpp       # Entry point
|-- shaders/           # GLSL shaders
|   |-- room.vert
|   |-- room.frag
|   |-- candle.vert
|   |-- candle.frag
|   |-- particle.vert
|   |-- particle.frag
|-- textures/          # Texture files
|   |-- wood_albedo.jpg
|   |-- wood_normal.jpg
|-- CMakeLists.txt     # (Optional) Build configuration for CMake
```

---

## Build Instructions

### Step 1: Clone the Repository
```
git clone <repository_url>
cd project
```

### Step 2: Install Dependencies
Ensure GLFW and GLAD are set up:
- GLFW: [Download GLFW](https://www.glfw.org/)
- GLAD: Use the OpenGL loader from [https://glad.dav1d.de/](https://glad.dav1d.de/)

Ensure the `glad` files are in the following structure:
```
project/
|-- glad/
|   |-- include/
|   |   |-- glad/
|   |       |-- glad.h
|   |-- src/
|       |-- glad.c
```

### Step 3: Build the Project
#### Option A: Using a Makefile
1. Create a `build/` directory:
   ```
   mkdir build
   cd build
   ```
2. Compile the project using a Makefile (if provided):
   ```
   make
   ```

#### Option B: Using CMake
1. Generate build files:
   ```
   cmake ..
   ```
2. Build the project:
   ```
   cmake --build .
   ```

#### Option C: Manual Compilation (G++ Command)
Use the following command to compile:
```
g++ src/main.cpp src/Shader.cpp src/CandleModel.cpp src/ParticleEmitter.cpp src/RoomModel.cpp glad/src/glad.c -o CandleWithFlame -Iinclude -Iglad/include -I"C:/msys64/mingw64/include" -L"C:/msys64/mingw64/lib" -lglfw3 -lopengl32 -lgdi32 -lglew32
```

### Step 4: Run the Application
After building, run the executable:
```
./CandleWithFlame
```

---

## Usage Instructions
1. Use the arrow keys to move around the room.
2. Observe how the flame dynamically lights the room and the candle.
3. The particles simulate realistic fire behavior with layered effects.

---

## Troubleshooting
1. **Missing GLFW/GLAD libraries**:
   - Ensure GLFW and GLAD are installed and correctly linked.
2. **Edit Configurations**
   - Modifica c_cpp_properties.json para incluir la ruta de la librería
```
   {
    "configurations": [
        {
            "name": "MinGW",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/msys64/mingw64/include" // just add this
            ],
            "defines": [],
            "compilerPath": "C:/Program Files/mingw64/bin/g++.exe",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```
3. **Textures Not Loading**:
   - Verify the `textures/` folder exists with the required image files.
4. **Shader Compilation Errors**:
   - Check for typos in the `.vert` and `.frag` shader files.
5. **Black Screen**:
   - Ensure your GPU supports OpenGL 3.3 or later.

---

## Credits
- Inspired by techniques from SIGGRAPH papers on realistic flame rendering.
- Developed using OpenGL and C++.

---

## License
This project is licensed under [MIT License].
