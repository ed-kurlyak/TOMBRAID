# Tomb Raider 1 MS-DOS 640x480 Widescreen Mode  

The project is based on the source code from <a href="https://github.com/LostArtefacts/TRX">https://github.com/LostArtefacts/TRX</a>  

This project source code is as close as possible to the original Core Design tomb.exe.  

The project is intended only for the WATCOM 10 environment.

## Recommended Build Environment

It is **not recommended** to use the WATCOM 10 Win32 environment to compile the `tombraid.wpj` project.

### Not Recommended
- WATCOM 10
- Target: `MS-DOS`
- Host: `Win32`

This configuration may produce low FPS problems.

## Recommended Compilation Method

Compile the project in a real MS-DOS environment using the provided `MAKEFILE`
and the `wmake` command.

### Recommended
- WATCOM 10
- Target: `MS-DOS`
- Host: `MS-DOS`

## Build Command

```bat
wmake
```

## Notes

The project was designed specifically for the MS-DOS build environment.
For best compatibility and correct executable behavior, use native MS-DOS tools.
Using the Win32-hosted compiler environment is discouraged.

<img src="https://github.com/ed-kurlyak/TOMBRAID/blob/main/pics/tomb1.png" alt="Lara Croft Tomb Raider 1 image">
