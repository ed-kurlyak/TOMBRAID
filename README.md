# Tomb Raider 1 Reverse Engineering Test Version

**Software rendering project (DirectX 6  support).**

Special thanks to LostArtefacts & Marcin Kurczewski for their previous work — this reverse engineering is partial, and the source code is as close as possible to the original Core Design EXE.  
The project is based on the source code from <a href="https://github.com/LostArtefacts/TRX">https://github.com/LostArtefacts/TRX</a>

## Overview

- Fully software rendering (no OpenGL).  
- Optional DirectX 6 rendering mode (requires DirectX 6 SDK). 
- Original Core Design functions used for triangle rasterization (software triangle rasterization).  
- 8-bit backbuffer using SciTech Mega Graphics Library from Quake 1.  
- Sound handled via DirectSound 8.  
- Tested on Windows 10, Compiled with Visual Studio 2019.  

## Instructions

1. Place level files in the `DATA` directory.  
2. Configuration and entry point can be found in `winmain.cpp`.

## Notes

- The project is primarily for testing and reverse engineering purposes.  

## Projects description:  

- TOMBRAID_SW_SciTech&HW_DX6_VS2019 - for software mode used SciTech Mega Graphics Library, for hardware mode DX6, Visual Studio 2019

- TOMBRAID_SW_SCITECH_VS2019 - software mode used SciTech Mega Graphics Library, Visual Studio 2019  


<img src="https://github.com/ed-kurlyak/TOMBRAID/blob/main/pics/tomb1.png" alt="Lara Croft Tomb Raider 1 image">
