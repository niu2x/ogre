//-------------------------------
//Bloom_ps20.glsl
// Blends using weights the blurred image with the sharp one
// Params:
//   OriginalImageWeight
//   BlurWeight
//-------------------------------

uniform sampler2D RT;
uniform sampler2D Blur1;
out vec4 fragColor;


uniform float OriginalImageWeight;
uniform float BlurWeight;
varying vec2 texCoord;

void main()
{
    vec4 sharp;
    vec4 blur;

    sharp = texture2D( RT, texCoord);
    blur = texture2D( Blur1, texCoord);
    
    fragColor = ( (blur * BlurWeight) + (sharp * OriginalImageWeight) );
    //fragColor = vec4(0);
}
