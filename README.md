# Tomb Raider 1 Reverse Engineering Test Version

**Software rendering project (no DirectX/OpenGL).**

Special thanks to LostArtefacts & Marcin Kurczewski for their previous work — this reverse engineering is partial, and the source code is as close as possible to the original Core Design EXE. The project is based on the source code from <a href="https://github.com/LostArtefacts/TRX">https://github.com/LostArtefacts/TRX</a>

## Overview

- Fully software rendering (no DirectX/OpenGL).  
- Original Core Design functions used for triangle rasterization (software triangle rasterization).  
- 8-bit backbuffer using SCITECH Mega Graphics Library from Quake 1.  
- Sound handled via DirectSound 8.  
- Tested on Windows 10. Compiled with Visual Studio 2019.  
- Code is unfinished, but the game is playable.

## Instructions

1. Place level files in the `DATA` directory.  
2. Configuration and entry point can be found in `winmain.cpp`.

## Notes

- The project is primarily for testing and reverse engineering purposes.  
- Expect incomplete features and unfinished implementations.  
