uniform sampler2D RT;
uniform sampler2D Sum;
uniform float blur;

varying vec2 oUv0;
out vec4 fragColor;


void main()
{
   vec4 render = texture2D(RT, oUv0);
   vec4 sum = texture2D(Sum, oUv0);

   fragColor = mix(render, sum, blur);
}
