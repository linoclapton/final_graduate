#version 410

in V{
vec3 tnorm;
vec4 eyeCoords;
//vec3 color;
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
//uniform vec3 colors[3];
//uniform sampler1DArray samplerColor;
layout( location = 0 ) out vec4 FragColor;

void main() {

	if(hasLight ){
        vec3 s = normalize(vec3(LightPosition - tes_in.eyeCoords));
        vec3 v = normalize(vec3(-1.0*s + 2*dot(tes_in.tnorm,s)*tes_in.tnorm));
        vec3 e = normalize(vec3(EyePosition-vec3(tes_in.eyeCoords)));
        vec3 LightIntensity =Ks*Ls*pow(max(dot(v,e),0.0),10)+La * Ka + Ld * Kd * max( dot( s, tes_in.tnorm ), 0.0 );
        FragColor = vec4(LightIntensity, 1.0);

    //FragColor = vec4(colors[1], 1.0);
    //FragColor = vec4(texture(samplerColor,vec2(1,2)).rgb, 1.0);
        //FragColor = vec4(tes_in.color,1.0);
	}
	else{
        FragColor = vec4(0.8,0.6,0.4,1.0);
	}
}
