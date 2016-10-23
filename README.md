========================================================================
    CONSOLE APPLICATION : HephaestusEngine Project Overview
========================================================================
Rudimentary attempt at building a C++ based voxel engine, primarily 
focused on an efficient backend system making interfacing with an 
eventual OpenGL rendering engine easier. PolyVox is providing inspiration
for the larger meta-objects, with smaller objects being based upon
various projects elsewhere.

Terrain generation is currently a bit bland, as it is using FastNoise
2D ValueFractals. 3D noise generation will be integrated eventually, 
providing more interesting terrain features along with better different-
iation of where to place certain types of blocks. The triLerpCube struct
and triLerp function are intended to be used with 3D noise, allowing for
3D noise's advantages without a tremendous amount of computation 
========================================================================
TODO:
- Expanding procedural terrain, generated as "player" object moves throu-
  ogh world.
- chunkManager class for paging chunks in and out of memory based on
  position of player. Also manages building new chunks, and saving data
- Fix rendering issues with textures. Using a texture atlas causes 
  bleeding and blurring with mipmaps, 2D tex array is poorly documented
- Some way to save chunk data
- Compress textures and import using better methods
- Integrate proper world and local lighting scheme, including SSAO
========================================================================
