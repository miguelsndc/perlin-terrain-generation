#version 460 core

layout(vertices=4) out;

uniform mat4 view;
uniform mat4 model;

in vec2 TexCoord[];
out vec2 TextureCoord[];

void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

	if (gl_InvocationID == 0) {
		const int MIN_TESSELLATION_LEVEL = 4;
		const int MAX_TESSELLATION_LEVEL = 64;
		const float MIN_DISTANCE = 20;
		const float MAX_DISTANCE = 800;

		vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
		vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
		vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
		vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;

		const float	distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
		const float	distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
		const float	distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
		const float	distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

		float tessLevel0 = mix( MAX_TESSELLATION_LEVEL, MIN_TESSELLATION_LEVEL, min(distance10, distance00) );
		float tessLevel1 = mix( MAX_TESSELLATION_LEVEL, MIN_TESSELLATION_LEVEL, min(distance00, distance01) );
		float tessLevel2 = mix( MAX_TESSELLATION_LEVEL, MIN_TESSELLATION_LEVEL, min(distance01, distance11) );
		float tessLevel3 = mix( MAX_TESSELLATION_LEVEL, MIN_TESSELLATION_LEVEL, min(distance11, distance10) );
		
		gl_TessLevelOuter[0] = tessLevel0;
		gl_TessLevelOuter[1] = tessLevel1;
		gl_TessLevelOuter[2] = tessLevel2;
		gl_TessLevelOuter[3] = tessLevel3;
		
		// set inner tessellation level to the max of the two parallel edges
		gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
		gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
	}
}