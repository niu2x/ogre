uniform sampler2D texMap;

varying vec4 colour;
varying vec4 uv;
out vec4 fragColor;


/*
  Basic fragment program using texture and diffuse colour.
*/
void main()
{
	fragColor = texture2D(texMap, uv.xy) * colour;
}
