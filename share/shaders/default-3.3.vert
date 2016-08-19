#version 330

in vec3 VertexPosition;

out vec2 vsTextureCoord0;

void main( void )
{
	gl_Position = vec4( VertexPosition, 1 );

	vsTextureCoord0 = ( VertexPosition.xy + 1.0 ) / 2.0;
}
