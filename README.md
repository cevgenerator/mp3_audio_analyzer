# MP3 Audio Analyzer

**Language:** C++  
**Version:** `v0.1.0`

A real-time MP3 audio analyzer (in development) using [mpg123](https://www.mpg123.de/) and [PortAudio](http://www.portaudio.com/).

Currently, the project plays back MP3 files while decoding them to raw PCM audio. Future versions will analyze the audio data in real-time and output the results (e.g., for audio-reactive graphics).

*Note: This project is under active development. Playback is functional; analysis features will be added soon.*

---

## Features

- Uses the `mpg123` library to decode MP3 files
- Dynamically allocates buffers to store raw PCM data
- Uses the `portaudio` library to play PCM data as audio
- Handles decoding errors and cleanly shuts down

---

## Build Instructions (CMake)

Make sure you have the required development tools:

```bash
sudo apt update
sudo apt install cmake libmpg123-dev portaudio19-dev build-essential
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

## Dependencies

- CMake ≥ 3.10 (build system)
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
├── src/
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

- [ ] Fix audio glitches
- [ ] Investigate buffer underruns or latency issues
- [ ] Refactor cleanup logic
- [ ] Refactor error handling into reusable functions
- [ ] Refactor audio logic into functions (e.g., InitMp3(), InitPortAudio())
- [ ] Add FFTW 
- [ ] Add real-time audio analysis
- [ ] Add 60 fps update/print loop for analysis
- [x] Add CMake support
- [ ] Add cross-platform compatibility (Windows/macOS)

---

## License

This code is released under the MIT License.
See LICENSE file for details.

### Third-party libraries

- **PortAudio** — licensed under the [MIT License](http://www.portaudio.com/license.html)
- **mpg123** — licensed under the [LGPL 2.1+](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html)

These libraries are not included in this repository, but are dynamically linked at runtime.

You are responsible for complying with their licenses if you reuse or redistribute this project.

### Audio Source

This project uses a music excerpt from the track:

**"Deep Performance" by Gradient**
- Original source: [Free Music Archive](https://freemusicarchive.org/music/Gradient/Seashore/thn113-320-02-gradient_-_deep_perfomance/)
- License: [CC BY-NC 3.0](https://creativecommons.org/licenses/by-nc/3.0/)
- This license allows redistribution and adaptation, provided attribution is given and it's not used for commercial purposes.
