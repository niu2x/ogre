varying vec4 oUv0;
out vec4 fragColor;
// Basic fragment program to display 3d uv
void main()
{
	vec3 n = normalize(oUv0.xyz);
	fragColor = vec4(n.x, n.y, n.z, 1.0);
}
