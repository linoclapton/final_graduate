#version 410
uniform vec3 outputColor;
in V{
//vec3 tnorm;
//vec4 eyeCoords;
vec3 color;
}tes_in;
out vec4 FragColor;
void main() {
        FragColor = vec4(outputColor,0.5);
        //FragColor = vec4(1.0,1.0,1.0,1.0);
        gl_FragDepth = 0.1f;
}
