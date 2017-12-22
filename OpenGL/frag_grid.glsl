#version 450

in vec2 oPos;
uniform sampler2D texture1;
uniform vec3 resolution;
uniform vec3 imageRes;

void main(){
     //gl_FragColor = vec4(1.0,1.0,0.0,1.0);
    vec4 colors[2][3] = {{vec4(1.0,0.0,0.0,1.0),vec4(0.0,1.0,0.0,1.0),vec4(0.0,0.0,1.0,1.0)},
    {vec4(0.0,1.0,0.0,1.0),vec4(0.0,0.0,1.0,1.0),vec4(1.0,0.0,0.0,1.0)}};
    if(int(gl_FragCoord.y) % 8 == 0  || (int(gl_FragCoord.x) % 8) == 0 )
         gl_FragColor = vec4(0.0,0.0,0.0,1.0);
    else
         gl_FragColor = vec4(1.0,1.0,1.0,1.0);
     //gl_FragColor = colors[(int(gl_FragCoord.y) % 16)/8][(int(gl_FragCoord.x) % 24)/8];
     //gl_FragColor = vec4(0.0,0.0,0.0,1.0);
     /*if( gl_FragCoord.x < imageRes.x && gl_FragCoord.y < imageRes.y )
         gl_FragColor = texture2D(texture1,vec2(gl_FragCoord.x/imageRes.x,-gl_FragCoord.y/imageRes.y));//vec4(oPos+0.5,0.0,1.0);//
     else
         gl_FragColor = vec4(0.0,0.0,0.0,1.0);//*/
     return;
}