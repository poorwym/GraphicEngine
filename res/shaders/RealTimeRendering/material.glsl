vec3 GetTextureColor(int index, vec2 texCoord, int type){
    if(type == ALBEDO_MAP_INDEX) // 漫反射
        return texture(textures, vec3(texCoord, materials[index].AlbedoMapIndex)).rgb;
    else if(type == NORMAL_MAP_INDEX) // 法线贴图
        return texture(textures, vec3(texCoord, materials[index].NormalMapIndex)).rgb;
    else if(type ==EMISSION_MAP_INDEX) // 自发光
        return texture(textures, vec3(texCoord, materials[index].EmissionMapIndex)).rgb;
    else 
        return vec3(0.0);
}

float GetTextureValue(int index, vec2 texCoord, int type){
    if(type == METALLIC_MAP_INDEX) // 金属度
        return texture(textures, vec3(texCoord, materials[index].MetallicMapIndex)).r;
    else if(type == ROUGHNESS_MAP_INDEX) // 粗糙度
        return texture(textures, vec3(texCoord, materials[index].RoughnessMapIndex)).r;
    else if(type == AO_MAP_INDEX) // 环境光
        return texture(textures, vec3(texCoord, materials[index].AOMapIndex)).r;
    else if(type == ALPHA_MAP_INDEX) // 透明度
        return texture(textures, vec3(texCoord, materials[index].AlphaMapIndex)).r;
    else if(type == HEIGHT_MAP_INDEX) // 高度贴图
        return texture(textures, vec3(texCoord, materials[index].HeightMapIndex)).r;
    else
        return 0.0;
}

vec3 GetAmbient(int index, vec2 texCoord){
    if(materials[index].AlbedoMapIndex != -1)
    {
        return GetTextureColor(index, texCoord, ALBEDO_MAP_INDEX);
    }
    return materials[index].Ambient.xyz;
}

vec3 GetDiffuse(int index, vec2 texCoord){
    if(materials[index].AlbedoMapIndex != -1)
    {
        return GetTextureColor(index, texCoord, ALBEDO_MAP_INDEX);
    }
    return materials[index].Diffuse.xyz;
}

vec3 GetSpecular(int index, vec2 texCoord){
    if(materials[index].AlbedoMapIndex != -1)
    {
        return GetTextureColor(index, texCoord, ALBEDO_MAP_INDEX);
    }
    return materials[index].Specular.xyz;
}

vec3 GetEmission(int index, vec2 texCoord){
    if(materials[index].EmissionMapIndex != -1)
    {
        return GetTextureColor(index, texCoord, EMISSION_MAP_INDEX);
    }
    return materials[index].Emission.xyz;
}

float GetAO(int index, vec2 texCoord){
    if(materials[index].AOMapIndex != -1)
    {
        return GetTextureValue(index, texCoord, AO_MAP_INDEX);
    }
    return 1.0;
}

float GetMetallic(int index, vec2 texCoord){
    if(materials[index].MetallicMapIndex != -1)
    {
        return GetTextureValue(index, texCoord, METALLIC_MAP_INDEX);
    }
    return 0.0;
}

vec3 GetNormal(int index, vec2 texCoord){
    if(materials[index].NormalMapIndex != -1)
    {
        return GetTextureColor(index, texCoord, NORMAL_MAP_INDEX);
    }
    return vec3(0.0);
}

float GetRoughness(int index, vec2 texCoord){
    if(materials[index].RoughnessMapIndex != -1)
    {
        return GetTextureValue(index, texCoord, ROUGHNESS_MAP_INDEX);
    }
    return 0.8;
}
float GetAlpha(int index, vec2 texCoord){
    if(materials[index].AlphaMapIndex != -1)
    {
        return GetTextureValue(index, texCoord, ALPHA_MAP_INDEX);
    }
    return 1.0;
}

float GetIllum(int index){
    return materials[index].Illum;
}

float GetDissolve(int index){
    return materials[index].Dissolve;
}

float GetHeight(int index, vec2 texCoord){
    if(materials[index].HeightMapIndex != -1){
        return GetTextureValue(index, texCoord, HEIGHT_MAP_INDEX);
    }
    return 0.0;
}

float GetSpecularExponent(int index){
    return materials[index].SpecularExponent;
}