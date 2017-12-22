#version 450

in vec2 oPos;
uniform vec3 imageRes;
uniform vec3 pickPos;

void main(){
     //gl_FragColor = vec4(1.0,1.0,0.0,1.0);
    int a1 = (int(gl_FragCoord.y) % 16)/8;
    int a2 = (int(pickPos.y) % 16)/8;
    int b1 = (int(gl_FragCoord.x) % 24)/8;
    int b2 = (int(pickPos.x) % 24)/8;
    if( a1 == a2 && b1 == b2)
    if( length(pickPos.xy-gl_FragCoord.xy)<10.0)
         gl_FragColor = vec4(1.0,0.0,0.0,1.0);
     /*if( gl_FragCoord.x < imageRes.x && gl_FragCoord.y < imageRes.y )
         gl_FragColor = texture2D(texture1,vec2(gl_FragCoord.x/imageRes.x,-gl_FragCoord.y/imageRes.y));//vec4(oPos+0.5,0.0,1.0);//
     else
         gl_FragColor = vec4(0.0,0.0,0.0,1.0);//*/
     return;
}