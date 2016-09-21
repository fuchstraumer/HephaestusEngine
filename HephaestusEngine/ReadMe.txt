========================================================================
    CONSOLE APPLICATION : HephaestusEngine Project Overview
========================================================================
Rudimentary attempt at building a C++ based voxel engine, primarily 
focused on an efficient backend system making interfacing with an 
eventual OpenGL rendering engine easier. PolyVox is providing inspiration
for the larger meta-objects, with smaller objects being based upon
various projects elsewhere.

Noise generation functions will be the secondary goal after the main 
engine structure is built and capable of taking noise generator output.
Libraries like libnoise are relatively fast and offer enough options, 
but are deprecated and feature few examples. AccidentalNoise has a gamut
of features but slow noise generators. FastNoiseSIMD has incredibly fast
generators but very few features beyond generation. The goal is to find
a way to feed noise into AccidentalNoise transformation functions from
FastNoiseSIMD generators. Chunk terrain generation will be a major 
bottleneck without work on this
========================================================================
TODO:
- Implementation of Block class, Chunk class, Region class
- Header files for Chunk and Region classes
- Investigate methods of compressing vertex data using Union structures
- Implement a chunk manager class to maintain chunk lists for 
  updating chunks, drawing chunks, and other managerial functions
- Be able to construct objects like a VAO, VBO, and EBO suitable for
  use with OpenGL rendering functions (but integrate renderer outside
  of Block, Chunk, and Region classes - Chunk Manager should do this)
- Investigate greedy meshing methods and how to work this into tiling
  texture scheme
- Primitive 2D noise generation demonstrations with FastNoiseSIMD
========================================================================
