
vec3 GetTextureColor(int index, vec2 texCoords, int slot){
    if(slot== 0)//albedo
    {
        return texture(textures, vec3(texCoords, materials[triangles[index].materialIndex].AlbedoMapIndex)).rgb;
    }
    else if(slot== 1)// normal
    {
        return texture(textures, vec3(texCoords, materials[triangles[index].materialIndex].NormalMapIndex)).rgb;
    }
    else if(slot== 5)// emission
    {
        return texture(textures, vec3(texCoords, materials[triangles[index].materialIndex].EmissionMapIndex)).rgb;
    }
    return vec3(0.0);
}

float GetTextureValue(int index, vec2 texCoords, int slot){
    if( slot == 2)// metallic
    {
        return texture(textures, vec3(texCoords, materials[triangles[index].materialIndex].MetallicMapIndex)).r;
    }
    else if( slot == 3)// roughness
    {
        return texture(textures, vec3(texCoords, materials[triangles[index].materialIndex].RoughnessMapIndex)).r;
    }
    else if( slot == 4)// ao
    {
        return texture(textures, vec3(texCoords, materials[triangles[index].materialIndex].AOMapIndex)).r;
    }
    else if( slot == 6)// alpha
    {
         return texture(textures, vec3(texCoords, materials[triangles[index].materialIndex].AlphaMapIndex)).r;
    }
    return 0.0;
}

vec3 GetAmbient(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].AlbedoMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, ALBEDO_MAP_INDEX);
    }
    return materials[triangles[hitIndex].materialIndex].Ambient.xyz;
}

vec3 GetDiffuse(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].AlbedoMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, ALBEDO_MAP_INDEX);
    }
    return materials[triangles[hitIndex].materialIndex].Diffuse.xyz;
}

vec3 GetSpecular(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].AlbedoMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, ALBEDO_MAP_INDEX);
    }
    return materials[triangles[hitIndex].materialIndex].Specular.xyz;
}

vec3 GetEmission(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].EmissionMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, EMISSION_MAP_INDEX);
    }
    return materials[triangles[hitIndex].materialIndex].Emission.xyz;
}

float GetAO(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].AOMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, AO_MAP_INDEX);
    }
    return 1.0;
}

float GetMetallic(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].MetallicMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, METALLIC_MAP_INDEX);
    }
    return materials[triangles[hitIndex].materialIndex].Metallic;
}

vec3 GetNormal(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].NormalMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, NORMAL_MAP_INDEX);
    }
}

float GetRoughness(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].RoughnessMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, ROUGHNESS_MAP_INDEX);
    }
    return materials[triangles[hitIndex].materialIndex].Roughness;
}
float GetAlpha(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].AlphaMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, ALPHA_MAP_INDEX);
    }
    return 1.0;
}

float GetIllum(int hitIndex){
    return materials[triangles[hitIndex].materialIndex].Illum;
}

float GetDissolve(int hitIndex){
    return materials[triangles[hitIndex].materialIndex].Dissolve;
}

float GetHeight(int hitIndex, vec2 hitTexCoord){
    if(materials[triangles[hitIndex].materialIndex].HeightMapIndex != -1){
        return GetTextureValue(hitIndex, hitTexCoord, HEIGHT_MAP_INDEX);
    }
    return 0.0;
}

float GetSpecularExponent(int hitIndex){
    return materials[triangles[hitIndex].materialIndex].SpecularExponent;
}