uniform sampler2D RT;
uniform sampler2D noise;
varying vec2 oUv0;
out vec4 fragColor;


void main()
{
	float c = dot(texture2D(RT, oUv0), vec4(0.30, 0.11, 0.59, 0.00));
	float n = texture2D(noise, oUv0).r*2.0 - 1.0;
	c += n;
	if (c > 0.5)
	{
		c = 0.0;
	}
	else
	{
		c = 1.0;
	}   
	fragColor = vec4(c,c,c,1.0);
}