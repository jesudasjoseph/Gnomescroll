#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#ifdef GL_EXT_gpu_shader4
    flat varying vec2 Light;
#else
    varying vec2 Light;
#endif


// Attributes
/*
    control_node_CameraPosition =   shader->get_uniform("ChunkPosition");

    control_node_Vertex =           shader->get_attribute("InVertex");
    control_node_TexCoord =         shader->get_attribute("InTexCoord");
    control_node_Brightness =       shader->get_attribute("InBrightness");
*/

//uniform float CameraPosition;

varying float fogFragDepth;

attribute vec2 InTexCoord;
//attribute vec4 InBrightness;
attribute vec2 InLight;

varying vec2 texCoord;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoord = InTexCoord;
    fogFragDepth = distance(gl_Vertex.xy, gl_ModelViewMatrixInverse[3].xy);
    Light = InLight;
}
