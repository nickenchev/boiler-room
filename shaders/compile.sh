#!/bin/bash
glslc gbuffer.vert -o gbuffer.vert.spv
glslc gbuffer.frag -o gbuffer.frag.spv
glslc deferred.vert -o deferred.vert.spv
glslc deferred.frag -o deferred.frag.spv
glslc skybox.vert -o skybox.vert.spv
glslc skybox.frag -o skybox.frag.spv
glslc ui.vert -o ui.vert.spv
glslc ui.frag -o ui.frag.spv
glslc glyph.frag -o glyph.frag.spv
glslc debug.vert -o debug.vert.spv
glslc debug.frag -o debug.frag.spv
