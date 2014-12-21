#version 400

out vec4 fragColour;
uniform float roughness;
uniform mat4 modelViewMatrix;

in vec3 normal;
in vec3 eyeDir;
in vec3 lightDir;

void main()
{
    // set important material values
    float roughnessValue = roughness; // 0 : smooth, 1: rough
    float F0 = 0.8; // fresnel reflectance at normal incidence
    float k = 0.2; // fraction of diffuse reflection (specular reflection = 1 - k)
    vec3 lightColor = vec3(0.9, 0.1, 0.1);

    // interpolating normals will change the length of the normal, so renormalize the normal.
    vec3 Normal = normalize(normal);

    // do the lighting calculation for each fragment.
    float NdotL = max(dot(normal, lightDir), 0.0);

    float specular = 0.0;
    if(NdotL > 0.0)
    {
        vec3 eyeDirection = normalize(eyeDir);

        // calculate intermediary values
        vec3 halfVector = normalize(lightDir + eyeDirection);
        float NdotH = max(dot(Normal, halfVector), 0.0);
        float NdotV = max(dot(Normal, eyeDirection), 0.0); // note: this could also be NdotL, which is the same value
        float VdotH = max(dot(eyeDirection, halfVector), 0.0);
        float mSquared = roughnessValue * roughnessValue;

        // geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));

        // roughness (or: microfacet distribution function)
        // beckmann distribution function
        float r1 = 1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
        float roughness = r1 * exp(r2);

        // fresnel
        // Schlick approximation
        float fresnel = pow(1.0 - VdotH, 5.0);
        fresnel *= (1.0 - F0);
        fresnel += F0;

        specular = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
    }

    vec3 finalValue = lightColor * NdotL * (k + specular * (1.0 - k));
    fragColour = vec4(finalValue, 1.0);
}
