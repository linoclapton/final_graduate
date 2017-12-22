#version 410

in V{
//vec3 tnorm;
//vec4 eyeCoords;
vec3 color;
vec2 texCoord;
}tes_in;
uniform vec4 LightPosition; // Light position in eye coords.
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity
uniform vec3 Ka;
uniform vec3 La;
uniform vec3 Ks;
uniform vec3 Ls;
uniform vec3 EyePosition;
uniform bool hasLight;
uniform vec3 colors[3];
uniform sampler1DArray samplerColor;
layout( location = 0 ) out vec4 FragColor;

void main() {

	if(hasLight ){
	//vec3 s = normalize(vec3(LightPosition - tes_in.eyeCoords));
	//vec3 v = normalize(vec3(-1.0*s + 2*dot(tes_in.tnorm,s)*tes_in.tnorm));
	//vec3 e = normalize(vec3(EyePosition-vec3(tes_in.eyeCoords)));
    //vec3 LightIntensity =Ks*Ls*pow(max(dot(v,e),0.0),100)+La * Ka + Ld * Kd * max( dot( s, tes_in.tnorm ), 0.0 );
    //FragColor = vec4(colors[1], 1.0);
    //FragColor = vec4(texture(samplerColor,vec2(1,2)).rgb, 1.0);
        FragColor = vec4(tes_in.color,1.0);
	}
	else{
	//FragColor = texture(ourTexture,tes_in.texCoord)*vec4(tes_in.color,1.0);
        /*tes_in.texCoord.y = 1.0 - tes_in.texCoord.y ;
        vec4 color1 = texture(ourTexture1,tes_in.texCoord);//vec2(0.5,0.5));
        vec4 color2 = texture(ourTexture2,tes_in.texCoord);//vec2(0.5,0.5));
        float ratio = 0.2;
        if(color2.a==0.0)
            FragColor = color1*(1-ratio);
        else
            FragColor = mix(color1,color2,ratio);*/
        FragColor = vec4(tes_in.color,0.5);
	}
}
