# Tomb Raider 1 Reverse Engineering Test Version

**Software rendering project (now with optional DirectX 9 support).**

Special thanks to LostArtefacts & Marcin Kurczewski for their previous work — this reverse engineering is partial, and the source code is as close as possible to the original Core Design EXE.  
The project is based on the source code from <a href="https://github.com/LostArtefacts/TRX">https://github.com/LostArtefacts/TRX</a>

## Overview

- Fully software rendering (no OpenGL).  
- Optional DirectX 9 rendering mode (for compilation with DX9 support you need DirectX SDK June 2010).  
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

<imb src="https://github.com/ed-kurlyak/TOMBRAID/blob/main/pics/tomb1.png" alt="Lara Croft Tomb Raider 1 image">
