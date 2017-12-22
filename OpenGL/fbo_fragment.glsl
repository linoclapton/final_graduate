#version 410

in V{
vec2 texCoord;
//vec3 color;
}tes_in;
uniform sampler2D tex;
//uniform vec3 colors[3];
//uniform sampler1DArray samplerColor;
layout( location = 0 ) out vec4 FragColor;

void main() {

        FragColor = texture(tex,tes_in.texCoord*0.5+0.5);
        //FragColor = vec4(0.5,0.5,0.0,1.0); 
}
