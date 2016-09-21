# HephaestusEngine
Attempt 1.1 at building a voxel engine framework.

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
TODO:
========================================================================
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
References:
========================================================================
- The Polyvox engine/mesh processor has provided a tremendous body of
  knowledge to work from. Not giving credit would be uncouth. What follows
  is a link to their main PolyVox engine landing page:
    https://bitbucket.org/volumesoffun/polyvox
- This tutorial series has also proven thoroughly educational:
    https://sites.google.com/site/letsmakeavoxelengine/home
- Inspiration and outright code has come from the Craft project,
  including the rather smart way of building a cubes vertices.
    https://github.com/fogleman/Craft
