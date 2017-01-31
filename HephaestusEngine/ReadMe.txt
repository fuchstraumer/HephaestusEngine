========================================================================
    CONSOLE APPLICATION : HephaestusEngine Project Overview
========================================================================
Rudimentary attempt at building a C++ based voxel engine, primarily 
focused on an efficient backend system making interfacing with an 
eventual OpenGL rendering engine easier. PolyVox is providing inspiration
for the larger meta-objects, with smaller objects being based upon
various projects elsewhere.

Terrain generation is currently a bit bland, as it is using rather vanilla
simplex terrain generation with some basic modulation and octave iteration.

Swiss generator is pretty broken, but creates interesting results.
========================================================================
TODO:
- Expanding procedural terrain, generated as "player" object moves throu-
  ogh world.
- ChunkManager class for paging chunks in and out of memory based on
  position of player. Also manages building new chunks, and saving data. 
  Outline of this is complete, needs actual implementation.
- Fix rendering issues on chunk borders and see about how to get SSAO
  working on chunk edges (querying data here can be risky)
- Some way to save chunk data
- Make textures into a fileformat that works better with OpenGL and 
  loads onto the GPU faster. Not a bottleneck currently, but could be.
- Add a skybox using the method I applied to DiamondDogs.
- Integrate proper world and local lighting scheme, including SSAO. Some 
  sort of fog would be nice. Use previous skybox and a timer to shift
  time and create day/night cycle.
  NOTE: There was some sort of SSAO implemented, but a botched pull+commit
  overwrote the files I had for this. May be able to rescue these, somehow.
- Better investigation into the performance of linearly-encoded vs Z-order
  curve encoded chunks. Currently using linearly encoded chunks for their
  simplicity, and because I don't query blocks after initialization. 
  Frequent block queries during runtime would probably make Z-order enc-
  -oding worth any initial setup cost.
========================================================================
