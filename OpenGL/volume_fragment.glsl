#version 430

//in V{
//vec3 tnorm;
//vec4 eyeCoordsupwind time;
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
uniform bool DrawSelectRect;
uniform bool DrawSelectRectLine;
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
uniform sampler3D volume_label;
uniform sampler3D volume_flag;//Ìí¼Ó
uniform sampler1D color_tex;
uniform sampler1D opacity_tex;
uniform sampler2D front_tex;
uniform sampler2D back_tex;
uniform sampler2D gradient_gray_opacity;
uniform usampler2D preIntegerationTable;
uniform sampler2D graphCutTex;
uniform bool DrawSelect;//Ìí¼Ó
uniform bool GetPos;
uniform bool drawAxis;
uniform vec3 selectA;
uniform vec3 selectB;
uniform float steps;
in vec3 sPos;
in vec3 oPos;
uniform int slice;
layout( location = 0 ) out vec4 FragColor;
vec3 rayDir;
uniform bool Scat;
uniform float albedo;
uniform sampler2DArray asTable;
uniform bool graphCut;
uniform sampler1D label_flag;

bool check(float f) {
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

vec3 getGradientNormal(vec3 pos){
    vec3 n;
    if(onBorder(pos))
        n = fixClipBorderGradient(pos);
    else
        n = vec3(getVoxel(pos - vec3(spaceX, 0.0, 0.0)) - getVoxel(pos + vec3(spaceX, 0.0, 0.0)),
		getVoxel(pos - vec3(0.0, spaceY, 0.0)) - getVoxel(pos + vec3(0.0, spaceY, 0.0)),
		getVoxel(pos - vec3(0.0, 0.0, spaceZ)) - getVoxel(pos + vec3(0.0, 0.0, spaceZ))
		);
	return n;
}
vec3 getGradientNormalAll(vec3 pos){
   	vec3 n = vec3(texture(volume_tex,(pos - vec3(spaceX, 0.0, 0.0))).r - texture(volume_tex,(pos + vec3(spaceX, 0.0, 0.0))).r,
		texture(volume_tex,(pos - vec3(0.0, spaceY, 0.0))).r- texture(volume_tex,(pos + vec3(0.0, spaceY, 0.0))).r,
		texture(volume_tex,(pos - vec3(0.0, 0.0, spaceZ))).r- texture(volume_tex,(pos + vec3(0.0, 0.0, spaceZ))).r
		);
	return normalize(n);

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
vec3 LeovyColor(vec3 pos,vec3 vcolor,vec3 ec) {
    vec3 color = vec3(0.0,0.0,0.0);
    vec3 n =  getGradientNormalAll(pos);//getGradientNormalAll(pos);
    //vec3 n = (Model*vec4(norm,1.0f)).xyz;
    //vec3 n = getGradientNormal(pos);
    vec3 specular; 
    vec3 diffuse;
    float gradLength = length(n);
    vec3 H = normalize((-ec/length(-ec)).xyz+(LightPosition/length(LightPosition)).xyz);
    float NL = max(dot(n,normalize(LightPosition.xyz)),0.0);
    diffuse = vcolor*Ld*NL;
    color  += diffuse;
    //n = n/gradLength;
    specular = vcolor*Ls*pow(max(abs(dot(n,H)),0.0),32);
    color += specular; 
    vec3 ambient = vcolor*La;
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
vec4 directRendering(in vec3 first, in vec3 last, vec2 p) {

    vec4 result = vec4(0.0);
    float depthT = -1.0;
    bool finished = false;

    // calculate ray parameters
    float stepIncr, tend;
    float t = 0.0;
    vec3 direction;
    raySetup(first, last, vec3(512.0f,512.0f,175.0f), direction, stepIncr, tend);
    // 2 nested loops allow for more than 255 iterations,
    // but is slower than while (t < tend)
    for (int loop0=0; !finished && loop0<255; loop0++) {
        for (int loop1=0; !finished && loop1<255; loop1++) {

            vec3 sampleVal = first.rgb + t * direction;
            float voxel = getVoxel(sampleVal);
            // no shading is applied
            vec4 color = vec4(texture(color_tex,voxel).rgb,texture(opacity_tex,voxel));

            // perform compositing
            if (color.a > 0.0) {
                // accomodate for variable sampling rates (base interval defined by mod_compositing.frag)
                color.a = 1.0 - pow(1.0 - color.a, 200);
                result.rgb = result.rgb + (1.0 - result.a) * color.a * color.rgb;
                result.a = result.a + (1.0 -result.a) * color.a;
            }

            // save first hit ray parameter for depth value calculation
            if (depthT < 0.0 && result.a > 0.0)
                depthT = t;

            // early ray termination
            if (result.a > 0.9) {
                result.a = 1.0;
                finished = true;
            }

            t += stepIncr;
            finished = finished || (t > tend);
        }
    }

    // calculate depth value from ray parameter
    gl_FragDepth = 1.0;
    if (depthT >= 0.0)
        gl_FragDepth = depthT/tend;
        //calculateDepthValue(depthT/tend, textureLookup2Dnormalized(entryPointsDepth_, entryParameters_, p).z,
                       //                                 textureLookup2Dnormalized(exitPointsDepth_, exitParameters_, p).z);

    result = clamp(result,0.0f,1.0f);
    return result;
}
vec4 oneDimensionTransfer(){
    //if(gl_FragCoord.x>100||gl_FragCoord.y>100)
    //    return vec4(0.2,0.2,0.2,1.0);
    bool SetPos = false;
    vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
    vec4 rayEnd = texture(back_tex,oPos.xy+0.5);
    rayStart.x = (rayStart.x-0.5)*ratioX+0.5; rayEnd.x = (rayEnd.x-0.5)*ratioX+0.5;
    rayStart.y = (rayStart.y-0.5)*ratioY+0.5; rayEnd.y = (rayEnd.y-0.5)*ratioY+0.5;
    rayStart.z = (rayStart.z-0.5)*ratioZ+0.5; rayEnd.z = (rayEnd.z-0.5)*ratioZ+0.5;
    /*float re = max(rayEnd.x, max(rayEnd.y, rayEnd.z)); 
    float rs = max(rayStart.x, max(rayStart.y, rayStart.z));
	if (re > 1.00 || re < 0.0)
		discard;
	if (rs > 1.00 || rs < 0.0)
		discard;*/

    if(GetPos&&abs(oPos.x-selectA.x*0.5)<0.001&&abs(oPos.y-selectA.y*0.5)<0.001){
        SetPos = true;
    }
    //if(max(rayStart.x,max(rayStart.y,rayStart.z))>1.0)
    //    discard;
    //if(max(rayEnd.x,max(rayEnd.y,rayEnd.z))>1.0)
    //    discard;
	int step = 0;
	float alpha = 0;
	//float color = 0;
	vec3 color = vec3(0.0,0.0,0.0);
	float acc = 0;
    float accs = 0;
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
    vec4 originStart = rayStart;
    float tmpLabel = 0.4f;
    int maxDepth;
    vec2 random[5] = {vec2(-1,0),vec2(0,1),vec2(1,0),vec2(0,0),vec2(0,-1)};
    for(index=0;index<loop;index++){
       rayStart = originStart+dir*rand(gl_FragCoord.xy+random[index]);
       maxDepth = 0; 
	for(step=0;step<maxStep;step++){
		//acc = texture(volume_tex,vec3(sPos.x+0.5,-sPos.z+step*stepSize,sPos.y+0.5)).r;
        if(texture(volume_flag,vec3(rayStart+step*dir).xyz).r!=0)
            voxel = 0.0;
        else
            voxel = getVoxel(vec3(rayStart+step*dir).xyz);

        //preVoxel = getVoxel(vec3(rayStart+(step-1)*dir).xyz); 
        //voxelLabel = texture(volume_label,(rayStart+step*dir).xyz).r;
        //tmpLabel = texture(label_flag,voxelLabel).r;
		acc = texture(opacity_tex,voxel.x).r;
       accs = texture(volume_label,(rayStart+step*dir).xyz).r;
        //acc = texture(preIntegerationTable,vec2(preVoxel,voxel)).a;
		if(tmpLabel>0.3f&&acc>0.00002)
		{
			//color = color + (texture(color_tex,voxel)*(1-alpha)).rgb; //lightColor(Model*(rayStart+step*dir-0.5),texture(color_tex,voxel))*(1-alpha);
            if(acc>maxAlpha.a)
                maxAlpha = vec4((rayStart+step*dir).xyz,acc);
			//color = color + acc*(1-alpha);
            //acc = acc*pow(1+length(getGradientNormal((rayStart+step*dir).xyz)),4.0);
            //color = color + microfacet( (rayStart+step*dir).xyz, texture(color_tex,voxel).rgb,dir.xyz)*(1-alpha)*acc;
            if(Scat)
            color = color + 0.1*accs*LeovyColor( (rayStart+step*dir).xyz, texture(color_tex,voxel).rgb,dir.xyz)*(1-alpha)*(acc);
            else
            color = color + LeovyColor( (rayStart+step*dir).xyz, texture(color_tex,voxel).rgb,dir.xyz)*(1-alpha)*(acc);
            //color = color + LeovyColor( (rayStart+step*dir).xyz, vec3(voxelLabel,1.0-voxelLabel,0).rgb,dir.xyz)*(1-alpha)*acc;
            //float green = voxelLabel>0.5?voxelLabel*2-1.0:voxelLabel;
            //color = color +  vec3(voxelLabel,green,1.0-voxelLabel).rgb*(1-alpha)*acc;
			alpha = 1-(1-alpha)*(1-acc);
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
vec4 renderGraphCutBF(){
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
    vec4 dir = (rayEnd-rayStart)/maxStep;
    rayDir = normalize(dir).xyz;
    vec4 maxAlpha = vec4(0.0,0.0,0.0,0.0);
    float voxel = 0;float preVoxel = 0;
    int loop = 1,index = 0;
    vec4 result[5];
    vec4 originStart = rayStart;
    vec2 random[5] = {vec2(-1,0),vec2(0,1),vec2(1,0),vec2(0,0),vec2(0,-1)};
    for(index=0;index<loop;index++){
        rayStart = originStart;
	for(step=0;step<maxStep;step++){
		//acc = texture(volume_tex,vec3(sPos.x+0.5,-sPos.z+step*stepSize,sPos.y+0.5)).r;
        if(DrawSelect&&texture(volume_flag,vec3(rayStart+step*dir).xyz).r!=0)
            continue;
        //preVoxel = getVoxel(vec3(rayStart+(step-1)*dir).xyz); 
        voxel = getVoxel(vec3(rayStart+step*dir).xyz);
		acc =  texture(opacity_tex,voxel.x).r;
        //acc = texture(preIntegerationTable,vec2(preVoxel,voxel)).a;
		if(acc>0.002)
		{
			//color = color + (texture(color_tex,voxel)*(1-alpha)).rgb; //lightColor(Model*(rayStart+step*dir-0.5),texture(color_tex,voxel))*(1-alpha);
            if(acc>maxAlpha.a)
                maxAlpha = vec4((rayStart+step*dir).xyz,acc);
			//color = color + acc*(1-alpha);
            //color = color + microfacet( (rayStart+step*dir).xyz, texture(color_tex,voxel).rgb,dir.xyz)*(1-alpha)*acc;
            color = color + LeovyColor( (rayStart+step*dir).xyz, texture(color_tex,voxel).rgb,dir.xyz)*(1-alpha)*acc;
			alpha = 1-(1-alpha)*(1-acc);
                //color = color + LeovyColor( (rayStart+step*dir).xyz, texture(preIntegerationTable,vec2(preVoxel,voxel)).rgb,dir.xyz)*(1-alpha)*acc; 
                //color = color + texture(color_tex,voxel).rgb*(1-alpha)*acc;
			if(alpha>=0.85)//||max(color.r,max(color.g,color.b))>=1.0)
			{
				color = clamp(color,0.0,1.0);
				//FragColor = vec4(texture(color_tex,alpha).rgb,alpha);//vec4(abs(sPos.xyz),1.0);//vec4(1.0,0.0,0.0,1.0);
                if(graphCut)
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
    gl_FragDepth = 0.0f;
    //if (step >= 0.0)
        //gl_FragDepth =step*1.0f/maxStep;
    if(graphCut)
        return vec4(maxAlpha.xyz,1.0);
        //return maxAlpha;
    return outputColor;
	//return  vec4(color,alpha);
}
vec4 twoDimensionTransfer(){
    vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
    vec4 rayEnd = texture(back_tex,oPos.xy+0.5);
    //if(max(rayStart.x,max(rayStart.y,rayStart.z))>1.0)
    //    discard;
    //if(max(rayEnd.x,max(rayEnd.y,rayEnd.z))>1.0)
    //    discard;
    
	int step = 0;
	float alpha = 0;
	//float color = 0;
	vec3 color = vec3(0.0,0.0,0.0);
	float acc = 0;
	float stepSize = 1/512.0;
	float rayLength = length(rayEnd-rayStart);
	int maxStep = int(rayLength/stepSize)+1;
	//float Max=0.85,Min=0.20,width = Max-Min;
    vec4 dir = (rayEnd-rayStart)/maxStep;
    float voxel = 0;
    vec3 currentPos = vec3(0.0,0.0,0.0);
	for(step=0;step<maxStep;step++){
		//acc = texture(volume_tex,vec3(sPos.x+0.5,-sPos.z+step*stepSize,sPos.y+0.5)).r;
        currentPos = (rayStart+step*dir).xyz;
        voxel = getVoxel(currentPos);
		acc = texture(gradient_gray_opacity,vec2(getGradient(currentPos),voxel)).r;
        //return vec4(acc,0.0,0.0,1.0);
		//acc = texture(opacity_tex,voxel).r;
		if(acc>0.00002)
		{
			//color = color + (texture(color_tex,voxel)*(1-alpha)).rgb; //lightColor(Model*(rayStart+step*dir-0.5),texture(color_tex,voxel))*(1-alpha);
			alpha = 1-(1-alpha)*(1-acc);
			//color = color + acc*(1-alpha);
            if(alpha>0.0001){
                color = color + lightColor(Model*(rayStart+step*dir-0.5),texture(color_tex,voxel),dir)*(1-alpha)*acc;//(texture(color_tex,(voxel-Min)/width)*stepSize/rayLength*(1-alpha)).rgb; //lightColor(Model*(rayStart+step*dir-0.5),texture(color_tex,voxel))*(1-alpha);//stepSize/rayLength; 
            }
			if(alpha>=1.0)//||max(color.r,max(color.g,color.b))>=1.0)
			{
				color = clamp(color,0.0,1.0);
				//FragColor = vec4(texture(color_tex,alpha).rgb,alpha);//vec4(abs(sPos.xyz),1.0);//vec4(1.0,0.0,0.0,1.0);
                if(color.x<0.5&&color.y<0.5&&color.z<0.5)
                   color = color+0.5;
				return vec4(color,alpha);
			}
		}	
	}	
	if(alpha<0.01)
		discard;
	color = clamp(color,0.0,1.0);
	//FragColor = vec4(texture(color_tex,alpha).rgb,alpha);//vec4(abs(sPos.xyz),1.0);//vec4(1.0,0.0,0.0,1.0);
	//FragColor = vec4(color,0.1,0.1,alpha);
	return  vec4(color,alpha);
}

float sampleAEVolume(vec3 x){
    return 1;
}
float samplePITable(float theta,float alpha,float g){
    float result = texture(asTable,vec3(theta,alpha,g*10+9)).r;
    return 10;
}
float sampleRadiance(vec3 x,vec3 light){
    return 1;
}
vec4 DVR_AS(){
    vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
    vec4 rayEnd   = texture(back_tex,oPos.xy+0.5);
    int step = 0;
	float alpha = 0;
	vec3 color = vec3(0.0,0.0,0.0);
	float acc = 0;
	float rayLength = length(rayEnd-rayStart);
	int maxStep = int(rayLength*steps)+1;
    vec4 dir = (rayEnd-rayStart)/maxStep;
	float stepSize = length(dir);
    rayDir = normalize(dir).xyz;
    vec4 maxAlpha = vec4(0.0,0.0,0.0,0.0);
    float voxel = 0;float preVoxel = 0;
    int loop = 1,index = 0;
    vec4 result;
    vec3 accColor,acc_s,Ji;
    vec3 x_,x0,x_j,w_j;
    float ambient_radius=0.01;
    float Tao=0.0f,T=1.0f,Lb;
    float acc_t,L_tao,L_o,L_d_j;
    float theta_j;
    float g = -0.2f;
    float albedo = 0.9f;
    rayStart = rayStart+dir*rand(gl_FragCoord.xy);
    Ji = vec3(0.0f,0.0f,0.0f);
	for(step=0;step<maxStep;step++){
		//acc = texture(volume_tex,vec3(sPos.x+0.5,-sPos.z+step*stepSize,sPos.y+0.5)).r;
        if(DrawSelect&&texture(volume_flag,vec3(rayStart+step*dir).xyz).r!=0)
            continue;
        //preVoxel = getVoxel(vec3(rayStart+(step-1)*dir).xyz); 
        x_ = (rayStart+step*dir).xyz;
        x0 = x_ - ambient_radius*rayDir;
        voxel = getVoxel(x_);
		acc =  texture(opacity_tex,voxel).r;
        accColor = texture(color_tex,voxel).rgb;
        acc_s = accColor*acc*albedo;
        acc_t = sampleAEVolume(x_);
        L_tao = 0.0f;
        w_j = normalize(x_ - LightPosition.xyz);
        x_j = x_ - ambient_radius*w_j;
        theta_j = acos(dot(-rayDir,w_j));
        L_o = samplePITable(theta_j,ambient_radius*acc_t,g);
        L_d_j = sampleRadiance(x_j,LightPosition.xyz);
        L_tao = L_tao+L_d_j*L_o;
        Ji = Ji+T*acc_s*L_tao*stepSize;
        voxel = getVoxel(x0);
        accColor = texture(color_tex,voxel).rgb;
        acc = texture(opacity_tex,voxel).r;
        Tao = Tao + acc*stepSize; 
        T = exp(-Tao);
	}	
    return vec4(Ji,1.0f);
}
void main() {
    if(drawAxis){
        FragColor = axisColor;
        return;
    }
    if(DrawSelectRect){
        if( DrawSelectRectLine)
            FragColor = vec4(0.0,0.5,0.8,1.0);
        else
            FragColor = vec4(0.0,0.5,0.8,0.5);
        gl_FragDepth = 0.1f;
        return;
    }
	if(preRender)
	{
		FragColor = vec4(oPos+0.5,1.0);
        //gl_FragDepth = clamp(gl_FragDepth,-1.0,1.0);
		//FragColor = texture(front_tex,sPos.xy+0.5);
		return;
	}
    else if(graphCut){
        vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
        vec4 rayEnd = texture(back_tex,oPos.xy+0.5);
        if(rayEnd == rayStart)
            discard;
		vec4 flag = vec4(texture(graphCutTex,oPos.xy+0.5).rgb,1.0f); 
        if(flag.r<0.1)
            discard;
        FragColor = renderGraphCutBF();
        //FragColor = vec4(0.5,0.5,0.0,0.5);
        //gl_FragDepth 
    }
    else
	{
//	vec3 s = normalize(vec3(LightPosition - tes_in.eyeCoords));
//	vec3 v = normalize(vec3(-1.0*s + 2*dot(tes_in.tnorm,s)*tes_in.tnorm));
//	vec3 e = normalize(vec3(EyePosition-vec3(tes_in.eyeCoords)));
//  vec3 LightIntensity =Ks*Ls*pow(max(dot(v,e),0.0),100)+La * Ka + Ld * Kd * max( dot( s, tes_in.tnorm ), 0.0 );
//  FragColor = vec4(LightIntensity, 1.0);
//  if(check(sPos.x)||check(sPos.y)||sPos.z<0||sPos.z>1)
//		discard;	
//  FragColor = texture(front_tex,oPos.xy+0.5);
//  return;
        /*float i = texture(volume_flag,oPos.xyz+0.5).r;
        float j = i*255;
        if(j==130)
        FragColor =  vec4(i,0,0,1.0); */
		//FragColor = vec4(1.0,1.0,0.0,1.0);// 
        bool SetPos = false;
        vec4 rayStart = texture(front_tex,oPos.xy+0.5); 
        vec4 rayEnd = texture(back_tex,oPos.xy+0.5);
        if(rayEnd == rayStart)
            discard;
        //if(all(lessThan(rayStart,vec4(0.02,0.02,0.02,1.1))))
        //    discard;
        /*vec4 FragColor1 = vec4(texture(back_tex,oPos.xy+0.5).rgb,1.0f);
        vec4 FragColor2 = vec4(texture(back_tex,vec2(oPos.x+0.501,oPos.y+0.5)).rgb,1.0f);
        vec4 FragColor3 = vec4(texture(back_tex,vec2(oPos.x+0.499,oPos.y+0.5)).rgb,1.0f);
        if(all(lessThan(abs(FragColor1-FragColor2).xyz,vec3(0.001,0.001,0.001)))&&
all(lessThan(abs(FragColor1-FragColor3).xyz,vec3(0.001,0.001,0.001))))
        FragColor = vec4(1.0,0.0,0.0,1.0);
        else
        FragColor = FragColor1;
        return;*/
        //if (all(lessThan((rayEnd-rayStart).xyz,vec3(0.001,0.001,0.001))))
        //    discard;
        //FragColor = vec4(rayStart.xyz,1.0);
        //FragColor = directRendering(rayStart.xyz,rayEnd.xyz,gl_FragCoord.xy);
        //FragColor = DVR_AS();
        FragColor = oneDimensionTransfer();
        //FragColor.rgb = texture(asTable,vec3(gl_FragCoord.x/1024.0f,gl_FragCoord.y/1024.0f,14)).rrr;
        //FragColor.a = 1.0f;
        gl_FragDepth = 0.2f;
        //FragColor = vec4(texture(volume_tex,vec3(oPos.xy+0.5,slice*spaceZ)).rrr,1.0);
        //FragColor = twoDimensionTransfer();
        return;
	}
}
