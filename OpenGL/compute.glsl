#version 430

//layout (binding=0) buffer input_data{vec3 b;};
layout (local_size_x = 32,local_size_y = 32,local_size_z = 1) in;
//layout (binding=0,r32f) readonly uniform image3D image_data;
layout (binding=0,r8ui) coherent uniform uimage3D image_output;
uniform float currentID;
uniform int volumeZ;
uniform float spaceX;
uniform float spaceY;
uniform float spaceZ;
uniform bool undo;
uniform vec3 posA;
uniform vec3 posB;
uniform mat4 Model;
bool exclude(vec4 pos,vec2 plane,vec3 vn){
    float result = vn.x*(pos.x-plane.x)+vn.y*(pos.y-plane.y);
    if(result>0)
        return true;
    return false;
}
void main(){
     ivec3 coord = ivec3(gl_GlobalInvocationID.x,gl_GlobalInvocationID.y,gl_GlobalInvocationID.z*32);
     int maxz = coord.z+32>volumeZ?volumeZ:coord.z+32;
     float x = coord.x*spaceX;
     float y = coord.y*spaceY;
     if(undo){
         for(int i=coord.z;i<maxz;i++){
             vec4 flag = imageLoad(image_output,coord);
             if(flag.r==currentID)
                 imageStore(image_output, coord, ivec4(0,0,0,0));
             coord.z = coord.z+1;
         }
     }else{
         for(int i=coord.z;i<maxz;i++){
             coord.z = i;
             vec4 pos = vec4(x-0.5,y-0.5,i*spaceZ-0.5,1.0); 
             vec4 flag = imageLoad(image_output,coord);
             //vec4 data = imageLoad(image_data,coord);
             if(flag.r>0)//||data.r==0)
                 continue;
             pos = Model*pos;
             if(exclude(pos,vec2(posA.x,0.0),vec3(-1.0,0.0,0.0)))
                 continue;
             if(exclude(pos,vec2(posB.x,0.0),vec3(1.0,0.0,0.0)))
                 continue;
             if(exclude(pos,vec2(0.0,posA.y),vec3(0.0,1.0,0.0)))
                 continue;
             if(exclude(pos,vec2(0.0,posB.y),vec3(0.0,-1.0,0.0)))
                 continue;
             imageStore(image_output, coord, ivec4(currentID,0,0,0));
         }
     }
}
