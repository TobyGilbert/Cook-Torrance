#version 400

struct lights{
  vec3 direction;
  vec3 colour;
};

uniform int numLights;
uniform lights light[2];

out vec4 fragColour;
uniform float roughness;
uniform mat4 modelViewMatrix;

in vec3 normal;
in vec3 eyeDir;

void main(){
  vec3 r = vec3(0.0);

  for (int i=0; i<numLights; i++){
    float F0 = 0.8;
    float k = 0.2;
    vec3 Normal = normalize(normal);
    float NdotL = max(dot(Normal, normalize(light[i].direction)), 0.0);

    float specular = 0.0;
    if (NdotL > 0.0){
      vec3 eyeDirection = normalize(eyeDir);
      // 1. Calculate the fresnal term (F)
      vec3 h = normalize(normalize(light[i].direction) + eyeDirection);
      float VdotH = max(dot(eyeDirection, h), 0.0);
      float fresnal = F0 + ((1.0 - F0) * pow(1.0 - VdotH, 5.0));

      // 2. Calculate the roughness term (D)
      float m = roughness;
      float NdotH = max(dot(Normal, h), 0.0);
      float D1 = 1.0 / (3.14 * (m*m) * pow(NdotH, 4.0));
      float D2 = (NdotH * NdotH - 1.0) / ((m*m) * NdotH * NdotH);
      //float D = (1.0 / (3.14 * (m*m) * pow(NdotH, 4.0))) * exp((NdotH * NdotH - 1.0) / ((m*m) * NdotH * NdotH));
      float D = D1 * exp(D2);

      // 3. Calculate the geometrical attenuation factor (G)
      //    - If light reflected without interference G=1
      //    - If some light blocked after reflection (Gb)
      //    - If some light blocked before reaching the surface (Gc)
      //    - G = minimum of Gb and Gc

      float NdotV = max(dot(Normal, eyeDirection), 0.0);
      float Gb = ( 2.0 *  NdotH * NdotV) / (VdotH);
      float Gc = ( 2.0 *  NdotH * NdotL) / (VdotH);
      float G = min(1.0, min(Gb, Gc));

      specular = (fresnal * D * G) / (3.14 * NdotL * NdotV);
    }
    r += light[i].colour * NdotL * (k +  specular * (1.0 - k));
  }
  fragColour = vec4(r, 1.0);
}
