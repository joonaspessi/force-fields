# force-fields

## Build

Requires CMake 3.11+ and a C compiler. First build fetches Raylib 5.5 (~3 MB).

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/force-fields
```

For debug builds with extra diagnostics:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```
