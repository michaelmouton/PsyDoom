#version 460

//----------------------------------------------------------------------------------------------------------------------
// Sky shader: vertex
// This is used for rendering the sky in parts of the ceiling or on walls that have been marked as 'sky'.
// Stretches the top of the wall past the end of the screen.
//----------------------------------------------------------------------------------------------------------------------
#include "ShaderCommon_Vert.h"

DECLARE_UNIFORMS()
DECLARE_VS_INPUTS_VVERTEX_DRAW()

layout(location = 0) flat out vec2 out_uv_offset;
layout(location = 1) flat out ivec2 out_texWinPos;      // Note: pre-convert these to ivec2 for the frag shader
layout(location = 2) flat out ivec2 out_texWinSize;
layout(location = 3) flat out ivec2 out_clutPos;

void main() {
    // Transform the vertex and potentially extend upwards or downwards.
    //
    // If this is the top vertex of the sky wall then it will be marked as such with the 'v' coordinate being '1.0'.
    // If this is the case then stretch it upwards based on the depth from the viewer.
    // The opposite stretch (downwards, for lower sky walls) is applied if the 'v' coordinate is '-1.0'.
    //
    // We stretch based on depth to try and extend as little as possible and avoid precision issues.
    // Initially I extended sky walls by a fixed (large) amount and this caused occasional stitching artifacts, due to precision loss.
    //
    gl_Position = uniforms.mvpMatrix * vec4(in_pos, 1);
    gl_Position.y -= in_uv.y * max(gl_Position.w, 0) * 2.0;     // Max with zero to avoid issues with vertices behind the near clip plane!

    // Pass along all the other info we need to draw the sky
    out_uv_offset = vec2(in_uv.x, 0);
    out_texWinPos = ivec2(in_texWinPos);
    out_texWinSize = ivec2(in_texWinSize);
    out_clutPos = ivec2(in_clutPos);
}
