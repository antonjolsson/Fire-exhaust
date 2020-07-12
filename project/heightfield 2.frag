#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

uniform vec3 material_color;
layout(binding = 1) uniform sampler2D terrainPhoto;

in vec2 texCoord;
layout(location = 0) out vec4 fragmentColor;

// This simple fragment shader is meant to be used for debug purposes
// When the geometry is ok, we will migrate to use shading.frag instead.

void main()
{
	//fragmentColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
	//fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
	//fragmentColor = vec4(material_color, 1.0);
	fragmentColor = texture(terrainPhoto, texCoord);
}