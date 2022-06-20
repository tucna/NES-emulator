# NES-emulator
Home made NES emulator in C++ and DirectX 11 backend. *Still work in progress but already useful*.

<p align="center">  
  <img src="doc/sc1.jpg">
</p>

The project is built on [tPixelGameEngine](https://github.com/tucna/tPixelGameEngine) and [Dear ImGui](https://github.com/ocornut/imgui) for debug purposes. The debug windows show internal state of CPU, disassembled program and GPU pattern tables.

# Controls
- `X/Z/S/A` - action buttons.
- `Up/Down/Left/Right` - directional buttons.
- `F1` - debug windows.

# Features
- Full coverage of CPU official instructions.
- CPU unofficial instructions covered partially.
- PPU partially implemented (background handling).

# Additional information
PPU and CPU implementation is based on Javidx9 [NES emulation](https://github.com/OneLoneCoder/olcNES). Functionality passed [nestest.nes](https://wiki.nesdev.com/w/index.php/Emulator_tests) when it comes to official CPU instructions. Some of the unofficial are implemented as well as can be seen in the screenshots below. 

# Screenshots
<p align="center">  
  <img src="doc/sc2.jpg" width="350px">&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; <img src="doc/sc3.jpg" width="350px">
</p>

