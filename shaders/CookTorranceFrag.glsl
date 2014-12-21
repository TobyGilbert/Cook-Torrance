#version 400

out vec4 fragColour;
in vec3 Normal;
in vec4 eyeDirection;
in vec3 position;

uniform vec3 eyePosition;
uniform float roughness;
uniform mat4 modelViewMatrix;

struct lightInfo{
  vec4 position;
  vec3 intensity;
};

uniform lightInfo light;
void main(){
  float F0 = 0.8;
  float k = 0.2;
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(light.position.xyz- position);

  float specular = 0.0;

  float NdotL = max(dot(normal, lightDir), 0.0);

  if (NdotL > 0.0){
    vec3 eyeDir = normalize(-eyeDirection.xyz);
    // 1. Calculate the fresnal term (F)
    vec3 h = normalize(lightDir + eyeDir);
    float VdotH = max(dot(eyeDir, h), 0.0);

    float fresnal = F0 + ((1.0 - F0) * pow(1.0 - VdotH, 5.0));

    // 2. Calculate the roughness term (D)
    float m = roughness;
    float NdotH = max(dot(normal, h), 0.0);
    float D = (1.0 / (3.14 * (m*m) * pow(NdotH, 4.0))) * exp(( (NdotH * NdotH) - 1.0 ) /  (m*m) * (NdotH*NdotH));

    // 3. Calculate the geometrical attenuation factor (G)
    //    - If light reflected without interference G=1
    //    - If some light blocked after reflection (Gb)
    //    - If some light blocked before reaching the surface (Gc)
    //    - G = minimum of Gb and Gc

    float NdotV = max(dot(normal, eyeDir), 0.0);

    float Gb = ( 2.0 *  NdotH * NdotV) / (VdotH);
    float Gc = ( 2.0 *  NdotH * NdotL) / (VdotH);
    float G = min(1.0, min(Gb, Gc));

    specular = (fresnal * D * G) / (3.14 * NdotL * NdotV);

  }


  vec3 ambient = vec3(0.9, 0.1, 0.1);

  vec3 r = ambient * NdotL * (k +  specular * (1.0 - k));
  fragColour = vec4(r, 1.0);

}
