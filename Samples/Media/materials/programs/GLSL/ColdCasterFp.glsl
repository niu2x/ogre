varying vec2 NDotV;
out vec4 fragColor;

void main()
{
   fragColor = vec4(NDotV.x / 2.0);
}
