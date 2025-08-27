# MP3 Audio Analyzer (C++)

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

## Build Instructions

Make sure you have the development libraries installed:

```bash
sudo apt update
sudo apt install libmpg123-dev portaudio19-dev build-essential
```

Then from the project root, run:

```bash
mkdir -p build
g++ src/main.cpp -lmpg123 -lportaudio -o build/mp3_analyzer
```

---

## Usage

```bash
build/mp3_analyzer
```

Make sure the MP3 file is located at:

```bash
assets/gradient_deep_performance_edit.mp3
```

You can adjust the file path inside main.cpp if needed.

---

## Dependencies

- libmpg123 for MP3 decoding
- PortAudio for audio playback
- C++ compiler (e.g., g++)

Currently tested on Linux (Pop!_OS).

---

## Project Structure

```
project-root/
│
├── assets/
├── src/
├── third_party_licenses/
├── LICENSE
├── README.md
├── .clang-format
└── .gitignore
```

---

## To Do

- [ ] Refactor cleanup logic
- [ ] Refactor error handling into reusable functions
- [ ] Refactor audio logic into functions (e.g., InitMp3(), InitPortAudio())
- [ ] Add FFTW 
- [ ] Add real-time audio analysis
- [ ] Add 60 fps update/print loop for analysis
- [ ] Add CMake support
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
