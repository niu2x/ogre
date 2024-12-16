varying vec4 ambientUV;
out vec4 fragColor;


// Basic fragment program to display UV
void main()
{
	// wrap values using fract
	fragColor = vec4(fract(ambientUV.x), fract(ambientUV.y), 0.0, 1.0);
}
