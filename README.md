HephaestusEngine Project Overview:
========================================================================

Rudimentary attempt at building a C++ based voxel engine, primarily 
focused on an efficient backend system making interfacing with an 
eventual OpenGL rendering engine easier. PolyVox is providing inspiration
for the larger meta-objects, with smaller objects being based upon
various projects elsewhere.

Terrain generation is currently a bit bland, as it is using rather vanilla
simplex terrain generation with some basic modulation and octave iteration.

Swiss noise generator is pretty broken, but creates interesting results. 
Just how broken it is what makes it so fascinating, it seems. Jordan
noise is unimplemented entirely: don't try to call this.

This project draws inspiration and code from several sources. I try to 
avoid directly copy-and-pasting code, and usually prefer to (at least)
re-type it in my own words. I've drawn heavily from the following projects:

https://github.com/minetest/minetest

https://bitbucket.org/volumesoffun/polyvox/

https://github.com/fogleman/Craft

I have been trying to get better at giving specific links to the exact
source of code snippets/sections/files, but some of my older code isn't
as good about this. LearnOpenGL.com provided the majority of my rendering code,
especially the shader class. PolyVox is the origin of my mesh code, although
its diverged considerably from their implementation at this point in time.

TODO:
========================================================================

- Primary task right now: Rewriting the vast majority of the core code using minetest. Most credit for my project has to go to them! I try to embed links to the relevant sections in my code, but have only been doing this recently. I may have missed it in spots. Sorry! :c
- Expanding procedural terrain, generated as "player" object moves through world.
- ChunkManager class for paging chunks in and out of memory based on position of player. Also manages building new chunks, and saving data. Outline of this is complete, needs actual implementation.
- Fix rendering issues on chunk borders and see about how to get SSAO working on chunk edges (querying data here can be risky)
- Some way to save chunk data
- Make textures into a fileformat that works better with OpenGL and loads onto the GPU faster. Not a bottleneck currently, but could be.
- Add a skybox using the method I applied to DiamondDogs.
- Integrate proper world and local lighting scheme, including SSAO. Some sort of fog would be nice. Use previous skybox and a timer to shift time and create day/night cycle.
- Optimizing rendering resource usage, by experimenting with greedy meshing and compressing the mesh data. PolyVox has a nice example of a way to compress simplistic voxel mesh data.
