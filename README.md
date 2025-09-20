# MP3 Audio Analyzer

**Language:** C++  
**Version:** `v0.1.6`

A real-time MP3 audio analyzer (in development) using [mpg123](https://mpg123.de/), [PortAudio](https://portaudio.com/) and [FFTW](https://fftw.org/).

Currently, the project plays back MP3 files while decoding them to raw PCM audio and performing a Fast Fourier Transform. Future versions will further analyze the audio data in real-time and output the results (e.g., for audio-reactive graphics).

*Note: This project is under active development. Playback is functional; further analysis features will be added soon.*

---

## Features

- Uses the `mpg123` library to decode MP3 files
- Dynamically allocates a buffer to store raw PCM data
- Uses the `portaudio` library to play PCM data as audio
- Uses `FFTW` to perform an FFT
- Handles errors and cleanly shuts down

---

## Build Instructions (CMake)

Make sure you have the required development tools:

```bash
sudo apt update
sudo apt install cmake libmpg123-dev portaudio19-dev libfftw3-dev build-essential
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
- libmpg123 for MP3 decoding
- PortAudio for audio playback
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
- [ ] Add analysis thread
- [ ] Add FFTW 
- [ ] Add real-time audio analysis
- [ ] Add 60 fps update/print loop for analysis
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

- **FFTW** — licensed under the [GPL-2+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
- **mpg123** — licensed under the [LGPL 2.1+](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)
- **PortAudio** — licensed under the [MIT License](http://www.portaudio.com/license.html)

These libraries are not included in this repository, but are dynamically linked at runtime.

You are responsible for complying with their licenses if you reuse or redistribute this project.

### Audio Source

This project uses a music excerpt from the track:

**"Deep Performance" by Gradient**
- Original source: [Free Music Archive](https://freemusicarchive.org/music/Gradient/Seashore/thn113-320-02-gradient_-_deep_perfomance/)
- License: [CC BY-NC 3.0](https://creativecommons.org/licenses/by-nc/3.0/)
- This license allows redistribution and adaptation, provided attribution is given and it's not used for commercial purposes.
