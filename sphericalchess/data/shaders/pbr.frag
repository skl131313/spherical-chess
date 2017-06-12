#version 330

uniform vec3 ulightDirection;
uniform float uroughnessValue;
uniform samplerCube cubemap;
uniform samplerCube irradiance;
uniform sampler2D splitsum;
uniform sampler2D boardSampler;
uniform vec3 albedo;
uniform vec3 eyeNormal;

in vec3 varNormal;
in vec3 varEyeDir;
in vec2 varCoord;

out vec4 fragColor;


void main()
{

    float roughnessValue = uroughnessValue;
    // set important material values
    // float roughnessValue = 0.3; // 0 : smooth, 1: rough
    float F0 = 0.2; // fresnel reflectance at normal incidence
    float k = 0.6; // fraction of diffuse reflection (specular reflection = 1 - k)
    vec3 lightColor = vec3(1.0, 0.0, 0.0);
    
    // interpolating normals will change the length of the normal, so renormalize the normal.
    vec3 normal = normalize(varNormal);
    
    vec3 lightDirection = ulightDirection;

    // do the lighting calculation for each fragment.
    float NdotL = max(dot(normal, lightDirection), 0.0);
    
    float specular = 0.0;
    vec3 eyeDir = normalize(varEyeDir);
    float NdotV = max(dot(normal, eyeDir), 0.0); // note: this could also be NdotL, which is the same value

    {
        

        // calculate intermediary values
        vec3 halfVector = normalize(lightDirection + eyeDir);
        float NdotH = max(dot(normal, halfVector), 0.0); 
        float VdotH = max(dot(eyeDir, halfVector), 0.0);
        float mSquared = roughnessValue * roughnessValue;
        
        // geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));

        // roughness (or: microfacet distribution function)
        // beckmann distribution function
        float v = ((NdotH * NdotH) * (mSquared * mSquared - 1.0) + 1.0);
        float D = (mSquared * mSquared) / (3.14 * v * v);
        
        // fresnel
        // Schlick approximation
        float fresnel = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);

        specular =  (D * geoAtt) / (NdotV * NdotL * 4.0); // (fresnel * D * geoAtt) / (NdotV * NdotL * 4.0);
    }

    vec4 boardColor = texture(boardSampler, varCoord);

    vec4 lightTestColor = vec4(1.0, 220.0 / 225.0, 80.0 / 255.0, 1.0);

    vec3 finalValue = vec3(1.0) * specular * NdotL;
    fragColor = vec4(finalValue, 1.0);

    fragColor = max(fragColor, 0.0);

    vec3 reflectVector = normalize(reflect(-eyeDir, normal));

    vec4 indirectDiffuse = vec4(albedo, 0.0) * texture(irradiance, reflectVector);
    vec4 lut = texture(splitsum, vec2(roughnessValue, NdotL));

    vec4 prefilteredColor = textureLod(cubemap, reflectVector, roughnessValue * 5.0);
    vec4 indirectSpecular = prefilteredColor * (vec4(albedo, 0.0) * lut.r + lut.g);

    fragColor = ( fragColor + indirectDiffuse + indirectSpecular);

    float alpha = clamp(dot(normal, eyeNormal) * 1.75, 0.0, 1.0);

    fragColor = (fragColor * alpha) + (vec4(0,1,0,1) * (1 - alpha));

}