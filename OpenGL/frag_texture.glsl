#version 450

in vec2 oPos;
uniform sampler2D texture1;
uniform vec3 resolution;
uniform vec3 imageRes;

void main(){
     if( gl_FragCoord.x < imageRes.x && gl_FragCoord.y < imageRes.y )
         gl_FragColor = texture2D(texture1,vec2(gl_FragCoord.x/imageRes.x,-gl_FragCoord.y/imageRes.y)).bgra;//vec4(oPos+0.5,0.0,1.0);//
     else
         gl_FragColor = vec4(0.0,0.0,0.0,1.0);
     return;
}