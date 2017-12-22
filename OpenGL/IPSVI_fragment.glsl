#version 430

//in V{
//vec3 tnorm;
//vec4 eyeCoords;
//}tes_in;
uniform mat4 Model;
uniform vec4 LightPosition; // Light position in eye coords.
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity
uniform vec3 Ka;
uniform vec3 La;
uniform vec3 Ks;
uniform vec3 Ls;
uniform vec3 EyePosition;
uniform bool hasLight;
uniform bool preRender;
uniform float WindowMin;
uniform float WindowMax;
uniform float WindowWidth;
uniform float spaceX;
uniform float spaceY;
uniform float spaceZ;
uniform float ratioX;
uniform float ratioY;
uniform float ratioZ;
uniform vec4 axisColor;
uniform sampler3D volume_tex;
uniform sampler1D color_tex;
uniform sampler1D opacity_tex;
uniform sampler2D front_tex;
uniform sampler2D back_tex;
layout(binding=5,rgba32f) coherent uniform image2D illumA;
//layout(binding=5,rgba8ui) coherent uniform uimage2D illumA;
//layout(rgba8) uniform image2D illumB;
uniform bool drawAxis;
uniform bool drawImage;
uniform float steps;
in vec3 sPos;
in vec3 oPos;
uniform int slice;
layout( location = 0 ) out vec4 FragColor;
vec3 rayDir;

bool check(float f){
	if(f+0.5<0||f+0.5>1)
		return true;
	return false;
}
float rand(vec2 x){
    int n = int(x.x * 40.0 + x.y * 6400.0);
    n = (n << 13) ^ n;
    return 1.0 - float( (n * (n * n * 15731 + 789221) + \
             1376312589) & 0x7fffffff) / 1073741824.0;
}
float getVoxel(vec3 pos){
    float voxel = texture(volume_tex,pos).r; 
    //float voxel = texture(volume_label,pos).r; 
    if(voxel<WindowMin)
        voxel = 0;
    else if(voxel>WindowMax)
        voxel = 1;
    else
        voxel = (voxel-WindowMin)/WindowWidth;
    return voxel;
}
bool onBorder(vec3 samplePos) {
    if(samplePos.x == 1.0 || samplePos.x == 0.0 || samplePos.y == 1.0 || samplePos.y == 0.0 || samplePos.z == 1.0 || samplePos.z == 0.0){
            return true;
    }
    return false;
}

vec3 fixClipBorderGradient(vec3 pos) {
    vec3 v0 = normalize(texture(front_tex,  vec2(gl_FragCoord.x+2.5, gl_FragCoord.y) ).rgb - pos);
    vec3 v1 = normalize(texture(front_tex,  vec2(gl_FragCoord.x, gl_FragCoord.y+2.5) ).rgb - pos);
    //FIXME: error handling if v0 or v1 is (0,0,0)
    vec3 gradient = cross(v0, v1);
    gradient = faceforward(gradient, rayDir, gradient);
    return gradient;
}

vec3 getGradientNormal(vec3 pos){    vec3 n;    if(onBorder(pos))        n = fixClipBorderGradient(pos);    else        n = vec3(getVoxel(pos - vec3(spaceX, 0.0, 0.0)) - getVoxel(pos + vec3(spaceX, 0.0, 0.0)),		getVoxel(pos - vec3(0.0, spaceY, 0.0)) - getVoxel(pos + vec3(0.0, spaceY, 0.0)),		getVoxel(pos - vec3(0.0, 0.0, spaceZ)) - getVoxel(pos + vec3(0.0, 0.0, spaceZ))		)*0.5;	return n;}
vec3 getGradientNormalAll(vec3 pos){
   	vec3 n = vec3(texture(volume_tex,(pos - vec3(spaceX, 0.0, 0.0))).r - texture(volume_tex,(pos + vec3(spaceX, 0.0, 0.0))).r,
		texture(volume_tex,(pos - vec3(0.0, spaceY, 0.0))).r- texture(volume_tex,(pos + vec3(0.0, spaceY, 0.0))).r,
		texture(volume_tex,(pos - vec3(0.0, 0.0, spaceZ))).r- texture(volume_tex,(pos + vec3(0.0, 0.0, spaceZ))).r
		);
    n = n*0.5;
	return n;

}
vec3 getGradientNormalOp(vec3 pos){
   	vec3 n = vec3(texture(opacity_tex,texture(volume_tex,(pos - vec3(spaceX, 0.0, 0.0))).r).r - 
        texture(opacity_tex,texture(volume_tex,(pos + vec3(spaceX, 0.0, 0.0))).r).r,
		texture(opacity_tex,texture(volume_tex,(pos - vec3(0.0, spaceY, 0.0))).r).r- 
        texture(opacity_tex,texture(volume_tex,(pos + vec3(0.0, spaceY, 0.0))).r).r,
		texture(opacity_tex,texture(volume_tex,(pos - vec3(0.0, 0.0, spaceZ))).r).r- 
        texture(opacity_tex,texture(volume_tex,(pos + vec3(0.0, 0.0, spaceZ))).r).r
		);
    n = n*0.5;
	return n;
}
float getGradient(vec3 pos){
   	vec3 n = vec3(getVoxel(pos - vec3(spaceX, 0.0, 0.0)) - getVoxel(pos + vec3(spaceX, 0.0, 0.0)),
		getVoxel(pos - vec3(0.0, spaceY, 0.0)) - getVoxel(pos + vec3(0.0, spaceY, 0.0)),
		getVoxel(pos - vec3(0.0, 0.0, spaceZ)) - getVoxel(pos + vec3(0.0, 0.0, spaceZ))
		);
    float r = length(n);
    return r;
}
vec3 lightColor(vec4 pos,vec4 color,vec4 view){
	vec3 tn = getGradientNormal(pos.xyz);
    float gm = length(tn);
    vec3 lightIntensity=vec3(0.0,0.0,0.0);
    vec3 c = color.rgb;
    if(gm>0.1){
        tn = tn/gm;
        vec3 s = normalize(3*normalize( texture2D(back_tex, vec2(0.5, 0.5))) - pos).xyz;
        vec3 v = normalize(-view.xyz);
        vec3 e = normalize(v+s);
        //vec3 LightIntensity =c*Ls*pow(max(dot(v,e),0.0),100)+La * c + Ld * c * max( dot( s, tn ), 0.0 );
        lightIntensity = Ls*pow(max(dot(tn,e),0.0),8.0) + Ld * c * max( dot( s, tn ), 0.0 );
    }
    lightIntensity =lightIntensity + La*c;
    return lightIntensity;
}
vec3 microfacet(vec3 pos,vec3 vcolor,vec3 dir){
    vec3 N = normalize(getGradientNormal(pos));
    vec3 V = normalize(dir);
    vec3 L = normalize(LightPosition.xyz-pos);
    vec3 H = normalize((EyePosition/length(EyePosition)).xyz+(LightPosition/length(LightPosition)).xyz);//normalize(normalize(dir)+normalize(L));
    vec3 lightIntensity = vec3(1.0,1.0,1.0);
    float n_h = dot(N,H);
    float n_v = dot(N,V);
    float n_l = dot(N,L);
    float v_h = dot(V,H);
    float mean  = 0.7;
    float scale = 0.2;
    //vec3 Kd = vec3(1.0,1.0,1.0);
    //vec3 Ks = vec3(1.0,1.0,1.0);
    vec3 diffuse = Kd*max(n_l,0);
    float fresnel = pow(1.0+v_h,4);
    float delta = acos(n_h).x;
    float exponent = -pow((delta/mean),2);
    float microfacets = scale*exp(exponent);
    float term1 = 2*n_h*n_v/v_h;
    float term2 = 2*n_h*n_l/v_h;
    float selfshadow = min(1,min(term1,term2));
    vec3 specular = Ks*fresnel*microfacets*selfshadow/n_v;
    return vcolor*(diffuse+specular);
}
vec3 LeovyColor(vec3 pos,vec3 vcolor,vec3 ec){
    vec3 color = vec3(0.0,0.0,0.0);
    vec3 n =  getGradientNormal(pos);//getGradientNormalAll(pos);
    n = (Model*vec4(n,1.0f)).xyz;
    //vec3 n = getGradientNormal(pos);
    vec3 specular; 
    vec3 diffuse;
    float gradLength = length(n);
    //if(gradLength>0.001){
    n = n/gradLength;
    vec3 H = normalize((EyePosition/length(EyePosition)).xyz+(LightPosition/length(LightPosition)).xyz);
    float NL = max(dot(-n,LightPosition.xyz),0.0);
    diffuse = vcolor*Ld*NL;
    color  += diffuse;
    if(NL<=0)
       specular = vec3(0,0,0);
    else
       specular = vcolor*Ls*pow(max(dot(-n,H),0.0),8);
    color += specular; 
    //}
    vec3 ambient = Ka*La;
    color  += ambient;
    //color =vcolor*La+La/(1+pos.z)*(vcolor*max(dot(-n,LightPosition.xyz),0.0)+vcolor*pow(max(dot(-n,H),0.0),10));
    // return color;
    return color;
}
void raySetup(in vec3 first, in vec3 last, in vec3 dimension, out vec3 rayDirection, out float tIncr, out float tEnd) {
    // calculate ray parameters
    rayDirection = last - first;
    tEnd = length(rayDirection);
    rayDirection = normalize(rayDirection);
    tIncr = 1.0/(2*length(rayDirection*dimension));
}
vec4 oneDimensionTransfer(){
    bool SetPos = false;
    vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
    vec4 rayEnd = texture(back_tex,oPos.xy+0.5);
    rayStart.x = (rayStart.x-0.5)*ratioX+0.5; rayEnd.x = (rayEnd.x-0.5)*ratioX+0.5;
    rayStart.y = (rayStart.y-0.5)*ratioY+0.5; rayEnd.y = (rayEnd.y-0.5)*ratioY+0.5;
    rayStart.z = (rayStart.z-0.5)*ratioZ+0.5; rayEnd.z = (rayEnd.z-0.5)*ratioZ+0.5;
	int step = 0;
	float alpha = 0;
	vec3 color = vec3(0.0,0.0,0.0);
	float acc = 0;
	float stepSize = 0.003;
	float rayLength = length(rayEnd-rayStart);
	int maxStep = int(rayLength*steps)+1;
	float Max=0.85,Min=0.20,width = Max-Min;
    vec4 dir = (rayEnd-rayStart)/maxStep;
    rayDir = normalize(dir).xyz;
    vec4 maxAlpha = vec4(0.0,0.0,0.0,0.0);
    float voxel = 0;float voxelLabel = 0;
    int loop = 1,index = 0;
    vec4 result[5];
    vec4 localColor = vec4(0.0,0.0,0.0,0.0);
    vec4 originStart = rayStart;
    vec4 illumIn;
    vec4 illumOut;
    float tmpLabel = 0.4f;
    int maxDepth;
    vec2 random[5] = {vec2(-1,0),vec2(0,1),vec2(1,0),vec2(0,0),vec2(0,-1)};
    float IPSVIr;
    for(index=0;index<loop;index++){
       rayStart = originStart+dir*rand(gl_FragCoord.xy+random[index]);
       maxDepth = 0; 
	for(step=0;step<maxStep;step++){
		//acc = texture(volume_tex,vec3(sPos.x+0.5,-sPos.z+step*stepSize,sPos.y+0.5)).r;
        //preVoxel = getVoxel(vec3(rayStart+(step-1)*dir).xyz); 
        illumIn = imageLoad(illumA, ivec2(vec3(rayStart+step*dir).yz*480));
        voxel = getVoxel(vec3(rayStart+step*dir).xyz);
        //voxelLabel = texture(volume_label,(rayStart+step*dir).xyz).r;
        //tmpLabel = texture(label_flag,voxelLabel).r;
		acc = texture(opacity_tex,voxel.x).r;
        //acc = texture(preIntegerationTable,vec2(preVoxel,voxel)).a;
		if(tmpLabel>0.3f&&acc>0.00002)
		{
			//color = color + (texture(color_tex,voxel)*(1-alpha)).rgb; //lightColor(Model*(rayStart+step*dir-0.5),texture(color_tex,voxel))*(1-alpha);
            if(acc>maxAlpha.a)
                maxAlpha = vec4((rayStart+step*dir).xyz,acc);
			//color = color + acc*(1-alpha);
            //color = color + microfacet( (rayStart+step*dir).xyz, texture(color_tex,voxel).rgb,dir.xyz)*(1-alpha)*acc;
            localColor = vec4( LeovyColor((rayStart+step*dir).xyz, texture(color_tex,voxel).rgb,dir.xyz),acc );
            illumOut.rgb = ((1.0 - acc)*illumIn.rgb + acc*localColor.rgb);
            illumOut.a = ((1.0 - acc)*illumIn.a + acc);
            IPSVIr = 0.05;
            localColor = mix(localColor,illumIn,IPSVIr);
            imageStore(illumA, ivec2(vec3(rayStart+step*dir).yz*480), illumOut);
            color = color + localColor.rgb*(1-alpha)*localColor.a;
            
            //color = color + LeovyColor( (rayStart+step*dir).xyz, vec3(voxelLabel,1.0-voxelLabel,0).rgb,dir.xyz)*(1-alpha)*acc;
            //float green = voxelLabel>0.5?voxelLabel*2-1.0:voxelLabel;
            //color = color +  vec3(voxelLabel,green,1.0-voxelLabel).rgb*(1-alpha)*acc;
			alpha = 1-(1-alpha)*(1-localColor.a);
                //color = color + LeovyColor( (rayStart+step*dir).xyz, texture(preIntegerationTable,vec2(preVoxel,voxel)).rgb,dir.xyz)*(1-alpha)*acc; 
                //color = color + texture(color_tex,voxel).rgb*(1-alpha)*acc;
			if(alpha>=0.95)//||max(color.r,max(color.g,color.b))>=1.0)
			{
                if(maxDepth==0){
                    gl_FragDepth = step*1.0f/maxStep;
                    maxDepth = step;
                }
				color = clamp(color,0.0,1.0);
				//FragColor = vec4(texture(color_tex,alpha).rgb,alpha);//vec4(abs(sPos.xyz),1.0);//vec4(1.0,0.0,0.0,1.0);
                if(SetPos)
                    return vec4(maxAlpha.xyz,1.0);
                result[index] = vec4(color,alpha);
				//return vec4(color,alpha);
                continue;
			}
		}	
	}	
	color = clamp(color,0.0,1.0);
	result[index] = vec4(color,alpha);
    }
	if(alpha<0.01)
		discard;
    vec4 outputColor = vec4(0.0,0.0,0.0,0.0);
    for(index=0;index<loop;index++)
    outputColor = outputColor+result[index];
    outputColor = outputColor/loop;
	outputColor = clamp(outputColor,0.0,1.0);
    outputColor.a = alpha;
	//FragColor = vec4(texture(color_tex,alpha).rgb,alpha);//vec4(abs(sPos.xyz),1.0);//vec4(1.0,0.0,0.0,1.0);
	//FragColor = vec4(color,0.1,0.1,alpha);
    //gl_FragDepth = 1.0f;
    //gl_FragDepth = 0.0f;
    //if (step >= 0.0)
    if(SetPos)
        return vec4(maxAlpha.xyz,1.0);
        //return maxAlpha;

    return outputColor;
	//return  vec4(color,alpha);
}

void main() {
    if(drawAxis){
        FragColor = axisColor;
        return;
    }
	if(preRender)
	{
		FragColor = vec4(oPos+0.5,1.0);
        gl_FragDepth = clamp(gl_FragDepth,-1.0,1.0);
		return;
	}
    else if(drawImage)
	{
        bool SetPos = false;
        vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
        vec4 rayEnd = texture(back_tex,oPos.xy+0.5);
        if(rayEnd == rayStart)
            discard;
        FragColor = oneDimensionTransfer();
        //imageStore(illumA, ivec2(gl_FragCoord.xy), ivec4(FragColor*255));
        //imageStore(illumA, ivec2(gl_FragCoord.xy), FragColor);
        gl_FragDepth = 0.2f;
        return;
	}else{
        bool SetPos = false;
        vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
        vec4 rayEnd = texture(back_tex,oPos.xy+0.5);
        if(rayEnd == rayStart)
            discard;
        FragColor = imageLoad(illumA, ivec2(gl_FragCoord.xy));
        //uvec4 f = imageLoad(illumA, ivec2(gl_FragCoord.xy));
        //FragColor = vec4(f)/255.0;
        //f.x = f.x+100;f.w =  255;
        //FragColor = f;
        gl_FragDepth = 0.2f;
        return;
    }
}
