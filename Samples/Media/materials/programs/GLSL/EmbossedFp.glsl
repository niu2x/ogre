uniform sampler2D RT;
varying vec2 oUv0;
out vec4 fragColor;

void main()
{
    vec4 Color;
    Color.a = 1.0;
    Color.rgb = vec3(0.5);
    Color -= texture2D( RT, oUv0 - 0.001)*2.0;
    Color += texture2D( RT, oUv0 + 0.001)*2.0;
    Color.rgb = vec3((Color.r+Color.g+Color.b)/3.0);
    fragColor = Color;
}
