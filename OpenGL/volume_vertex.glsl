#version 430

layout (location = 0) in vec3 VertexPosition;
//layout (location = 1) in vec3 VertexNormal;
//out V{
//vec3 tnorm;
//vec4 eyeCoords;
//}v_out;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Camera;
uniform mat3 NormalMatrix;
uniform mat4 Projection;
uniform bool preRender;
uniform bool DrawSelectRect;
uniform bool drawAxis;
out vec3 sPos;
out vec3 oPos;
void main()
{
   // v_out.tnorm = normalize( NormalMatrix * VertexNormal);
    //v_out.eyeCoords = View * Model* vec4(VertexPosition,1.0);
    if(drawAxis){
        gl_Position =   Model*vec4(VertexPosition,1.0);
        return;
    }
    if(DrawSelectRect){
        gl_Position =   vec4(VertexPosition,1.0);
        return;
    }
    if(preRender){
        gl_Position =  Camera*Model*View*vec4(VertexPosition,1.0);//Projection * View *
        sPos =(Camera*Model*View*vec4(VertexPosition,1.0)).xyz;
        oPos = VertexPosition.xyz;//vec3(VertexPosition.x,VertexPosition.y,VertexPosition.z);
    }else{
        gl_Position =  vec4(VertexPosition,1.0);//Projection * View *
        sPos =VertexPosition.xyz;
        oPos = VertexPosition*0.5;
    }
} 