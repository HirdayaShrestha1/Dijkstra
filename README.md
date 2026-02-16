# Dijkstra Visualizer (SDL3)

A small SDL3 app that visualizes Dijkstra's algorithm on a grid with interactive obstacle editing.

## Features
- Step-by-step Dijkstra execution with adjustable speed.
- Random obstacle generation and quick reset.
- Toggle diagonal movement (adds diagonal cost).
- Mouse-driven start/target placement and obstacle editing.

## Controls
- Space: start / run Dijkstra.
- R: reset grid and randomize obstacles.
- C: clear obstacles.
- D: toggle diagonal movement (only when not running).
- Up/Down: decrease/increase step delay.
- Left click: toggle obstacle.
- Right click: set start.
- Shift + Right click: set target.
- Esc: quit.

## Build (Windows, CMake + SDL3)
1. Install SDL3 and note its CMake package path.
2. Update the SDL3 path in [CMakeLists.txt](CMakeLists.txt) if needed:
   - `set(SDL3_DIR "D:/develop/SDL3/x86_64-w64-mingw32/lib/cmake/SDL3")`
3. Configure and build:
   ```bash
   cmake -S . -B build
   cmake --build build
   ```
4. Run the executable from the build output directory.

## Notes
- Grid size is 25x25 with a 1000x1000 window by default.
- Diagonal movement uses a cost of $\sqrt{2}$.
