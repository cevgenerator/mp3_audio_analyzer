# MP3 Audio Analyzer

**Language:** C++  
**Version:** `v0.3.0`

A real-time MP3 audio analyzer (in development) using [mpg123](https://mpg123.de/), [PortAudio](https://portaudio.com/), [FFTW](https://fftw.org/), [GLFW](https://glfw.org/) and [GLAD](https://gen.glad.sh/).

Currently, the project plays back MP3 files while decoding them to raw PCM audio and performing a Fast Fourier Transform. Future versions will further analyze the audio data in real-time and output the results (e.g., for audio-reactive graphics).

*Note: This project is under active development. Playback is functional; further analysis features will be added soon.*

---

## Features

- Uses the `mpg123` library to decode MP3 files
- Dynamically allocates a buffer to store raw PCM data
- Uses the `portaudio` library to play PCM data as audio
- Uses `FFTW` to perform an FFT
- Uses `GLFW` for window management
- Uses `GLAD` for dynamically loading OpenGL functions
- Handles errors and cleanly shuts down

---

## Build Instructions (CMake)

Make sure you have the required development tools:

```bash
sudo apt update
sudo apt install cmake libmpg123-dev portaudio19-dev libfftw3-dev libglfw3-dev libgl1-mesa-dev libglm-dev build-essential
```

Then from the project root:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

This will produce the executable:

```bash
./mp3_analyzer
```

*Note: On some Linux systems, ALSA warnings (or similar messages) may appear at startup. These are expected and caused by the audio backend probing for devices. You can suppress them by redirecting stderr:*

```bash
./mp3_analyzer 2> /dev/null
```

---

## Build Options

By default, the project builds in **Release mode** with optimizations (`-O2`).

To build in **Debug mode** (with debug symbols and no optimizations), use:

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

---

## Dependencies

- CMake ≥ 3.10 (build system)

- FFTW for the Fast Fourier Transform
- GLAD (OpenGL function loader, included in source)
- GLFW for window management
- [GLM](https://github.com/g-truc/glm) (header-only math library)
- libmpg123 for MP3 decoding
- PortAudio for audio playback
- stb_image (for loading the font texture)
- UTF8-CPP (for handling multiple-byte glyphs)
- OpenGL (core graphics API, version 4.1 for macOS compatibility)
- C++17-compatible compiler (e.g., g++, clang++)
- Tested on Linux (Pop!_OS); Windows/macOS support planned

---

## Development Tools
- clang-tidy for static code analysis
- clang-format for consistent formatting

These tools are used during development to help maintain code quality and consistency.

---

## Project Structure

```
project-root/
│
├── assets/
├── cmake/
├── include/
├── shaders/
├── src/
├── tests/
├── third_party_licenses/
├── CHANGELOG.md
├── CMakeLists.txt
├── LICENSE
├── README.md
├── .clang-format
├── .clang-tidy
└── .gitignore
```

---

## To Do

- [x] Fix audio glitches
- [x] Investigate buffer underruns or latency issues
- [x] Refactor cleanup logic
- [x] Refactor error handling into reusable functions
- [x] Refactor audio logic into classes
- [x] Add ring buffer
- [x] Add analysis thread
- [x] Add FFTW 
- [x] Add real-time audio analysis
- [ ] Add visualizer
- [x] Add CMake support
- [ ] Add cross-platform compatibility (Windows/macOS)

---

## Tests

A basic manual test is included to verify `RingBuffer<T>` functionality (single-producer, single-consumer).

### Running the RingBuffer Test

From root, compile and run with:

```bash
g++ -std=c++17 -pthread \
    -Iinclude \
    tests/ring_buffer_test.cpp \
    -o tests/ring_buffer_test
./tests/ring_buffer_test
```

Expected output:

```bash
Test passed.
```

---

## License

This code is released under the GPL-2+ License.
See LICENSE file for details.

### Third-party libraries

- **GLAD** — licensed under the [MIT License](https://mit-license.org/)
- **stb_image** — licensed under the [MIT License](https://github.com/nothings/stb/blob/master/stb_image.h)
- **UTF8-CPP** — licensed under the [BSL-1.0 License](https://github.com/nemtrif/utfcpp/tree/master?tab=BSL-1.0-1-ov-file#readme)

The following libraries are not included in this repository, but are dynamically linked at runtime.

- **FFTW** — licensed under the [GPL-2+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
- **GLFW** — licensed under the [zlib/libpng license](https://www.zlib.net/zlib_license.html)
- **GLM** — licensed under the [MIT License](https://mit-license.org/)
- **mpg123** — licensed under the [LGPL 2.1+](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
- **PortAudio** — licensed under the [MIT License](http://www.portaudio.com/license.html)

You are responsible for complying with their licenses if you reuse or redistribute this project.

### Font

DejaVu Sans Mono (edited)
- Source: https://dejavu-fonts.github.io/
- License: Bitstream Vera Font License (with modifications)
- Used only to generate a static bitmap font atlas (font_atlas.png)

### Audio Source

This project uses a music excerpt from the track:

**"Orbiting A Distant Planet" by Quantum Jazz**
- Original source: [Free Music Archive](https://freemusicarchive.org/music/Quantum_Jazz/End_of_Line/07_-_Quantum_Jazz_-_Orbiting_A_Distant_Planet/)
- License: [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/)
- Changes made: The original track was excerpted (first ~1 minute), volume was adjusted, limitting was applied, silence was added at the beginning and a fade-out was added.
- This adapted version is also licensed under [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/).
