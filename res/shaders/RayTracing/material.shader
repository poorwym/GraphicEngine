
vec3 GetTextureColor(int index, vec2 texCoords, int slot){
    if(slot== 0)//albedo
    {
        if(triangles[index].material.AlbedoMapIndex != -1 ) 
            return texture(textures, vec3(texCoords, triangles[index].material.AlbedoMapIndex)).rgb;
    }
    else if(slot== 1)// normal
    {
        if(triangles[index].material.NormalMapIndex != -1 ) 
            return texture(textures, vec3(texCoords, triangles[index].material.NormalMapIndex)).rgb;
    }
    else if(slot== 5)// emission
    {
        if(triangles[index].material.EmissionMapIndex != -1 )
            return texture(textures, vec3(texCoords, triangles[index].material.EmissionMapIndex)).rgb;
    }
    return vec3(0.0);
}

float GetTextureValue(int index, vec2 texCoords, int slot){
    if( slot == 2)// metallic
    {
        if(triangles[index].material.MetallicMapIndex != -1 ) 
            return texture(textures, vec3(texCoords, triangles[index].material.MetallicMapIndex)).r;
    }
    else if( slot == 3)// roughness
    {
        if(triangles[index].material.RoughnessMapIndex != -1 ) 
            return texture(textures, vec3(texCoords, triangles[index].material.RoughnessMapIndex)).r;
    }
    else if( slot == 4)// ao
    {
        if(triangles[index].material.AOMapIndex != -1 ) 
            return texture(textures, vec3(texCoords, triangles[index].material.AOMapIndex)).r;
    }
    else if( slot == 6)// alpha
    {
        if(triangles[index].material.AlphaMapIndex != -1 ) 
            return texture(textures, vec3(texCoords, triangles[index].material.AlphaMapIndex)).r;
    }
    return -1.0;
}

vec3 GetAmbient(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.AlbedoMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, ALBEDO_MAP_INDEX);
    }
    return triangles[hitIndex].material.Ambient.xyz;
}

vec3 GetDiffuse(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.AlbedoMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, ALBEDO_MAP_INDEX);
    }
    return triangles[hitIndex].material.Diffuse.xyz;
}

vec3 GetSpecular(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.AlbedoMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, ALBEDO_MAP_INDEX);
    }
    return triangles[hitIndex].material.Specular.xyz;
}

vec3 GetEmission(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.EmissionMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, EMISSION_MAP_INDEX);
    }
    return triangles[hitIndex].material.Emission.xyz;
}

float GetAO(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.AOMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, AO_MAP_INDEX);
    }
    return 1.0;
}

float GetMetallic(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.MetallicMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, METALLIC_MAP_INDEX);
    }
    return triangles[hitIndex].material.SpecularExponent / 100;
}

vec3 GetNormal(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.NormalMapIndex != -1)
    {
        return GetTextureColor(hitIndex, hitTexCoord, NORMAL_MAP_INDEX);
    }
}

float GetRoughness(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.RoughnessMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, ROUGHNESS_MAP_INDEX);
    }
    return 1.0;
}
float GetAlpha(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.AlphaMapIndex != -1)
    {
        return GetTextureValue(hitIndex, hitTexCoord, ALPHA_MAP_INDEX);
    }
    return 1.0;
}

float GetIllum(int hitIndex){
    return triangles[hitIndex].material.Illum;
}

float GetDissolve(int hitIndex){
    return triangles[hitIndex].material.Dissolve;
}

float GetHeight(int hitIndex, vec2 hitTexCoord){
    if(triangles[hitIndex].material.HeightMapIndex != -1){
        return GetTextureValue(hitIndex, hitTexCoord, HEIGHT_MAP_INDEX);
    }
    return 0.0;
}

float GetSpecularExponent(int hitIndex){
    return triangles[hitIndex].material.SpecularExponent;
}