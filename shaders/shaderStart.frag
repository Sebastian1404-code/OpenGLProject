#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform sampler2D alphaTexture;


//directional light
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

uniform bool dirLight;


//point light
uniform vec3 lightPos;
uniform bool enablePosLight;
float constant=1.0f;
float linear=0.35f;
float quadratic=0.44f;

//fog
uniform vec3 viewerLoc;

vec3 CalcPointLight()
{

    if(!enablePosLight) {
	return vec3(0.0f);
}
	vec3 cameraPosEye=vec3(0.0f);
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
    vec3 normalEye = normalize(fNormal);
    vec3 lightDirN = normalize(lightPos - fPosEye.xyz);

    // diffuse shading
    float diff = max(dot(normalEye, lightDirN), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float spec = pow(max(dot(viewDirN, reflectDir), 0.0), shininess);

    // attenuation
    float distance    = length(lightPos - fPosEye.xyz);
    float attenuation = 1.0 / (constant + linear * distance + 
  			     quadratic * (distance * distance));   
 
    // combine results
    vec3 ambientPoint  = ambientStrength  * lightColor * vec3(texture(diffuseTexture, fTexCoords));
    vec3 diffusePoint  =  diff * lightColor * vec3(texture(diffuseTexture, fTexCoords));
    vec3 specularPoint = specularStrength * spec * vec3(texture(specularTexture, fTexCoords));
    ambientPoint  *= attenuation;
    diffusePoint  *= attenuation;
    specularPoint *= attenuation;
    return (ambientPoint + diffusePoint + specularPoint);
}

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeShadow(){
	
	//adjustments
	vec3 normalEye = normalize(fNormal);
	vec3 lightDirN = normalize(lightDir);


	if (texture(diffuseTexture, fTexCoords).a <= 0.5f) {
        	return 0.0f; // No shadow for transparent pixels
   	 }
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5f + 0.5f;

	float closestDepth = texture( shadowMap , normalizedCoords.xy ).r;
	float currentDepth = normalizedCoords.z;
	//float bias=0.005f;
	float bias = max(0.05 * (1.0 - dot(normalEye, lightDirN)), 0.005); 
	//float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	

	float shadow = 0.0;
    	vec2 texelSize = 1.0 / textureSize(shadowMap, 0); 
    	for (int x = -1; x <= 1; ++x) {
        	for (int y = -1; y <= 1; ++y) {
            	float pcfDepth = texture(shadowMap, normalizedCoords.xy + vec2(x,y) * texelSize).r; 
           	shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        	}    
    	}

    	shadow /= 9.0;

	if(normalizedCoords.z > 1.0f){
		return 0.0f;
	}
    

	return shadow;
	
}

float computeFog()
{
 float fogDensity = 0.02f;
 float fragmentDistance = length(viewerLoc-fPosEye.xyz);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
 return clamp(fogFactor, 0.0f, 1.0f);
}


void main() 
{
	vec3 color;
	vec4 diffuseColor = texture(diffuseTexture, fTexCoords);

	
	if(diffuseColor.a<=0.1f) 
		discard;

	float alpha=1.0f;
	if (textureSize(alphaTexture, 0).x > 1) {
        	alpha = texture(alphaTexture, fTexCoords).r; 
	}
	if(alpha<0.1) {
		discard;
	}

	if(dirLight) 
	{
	computeLightComponents();
	
	ambient *= diffuseColor.rgb;
	diffuse *= diffuseColor.rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();
	color = min((ambient + ( 1.0f - shadow ) * diffuse) + ( 1.0f - shadow ) * 	specular, 1.0f);
	}	
	else color= texture(diffuseTexture, fTexCoords).rgb;
	
	if(enablePosLight)
	{
		fColor=vec4(CalcPointLight()+ambient*0.1f,diffuseColor.a);
	}
	else
	{
	color+=CalcPointLight();


	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);	
	fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
	}

}
