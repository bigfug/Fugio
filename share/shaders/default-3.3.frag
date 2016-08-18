#version 330

in vec2 vsTextureCoord0;

layout ( location = 0 ) out vec4 FragmentColour;

void main( void )
{
	FragmentColour = vec4( 1.0, 0.5, 0.25, 1 );
}

