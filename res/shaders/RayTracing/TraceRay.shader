
vec4 TraceRay(Ray ray, vec3 throughput)
{
    vec4 radiance = vec4(0.0);
    for (int depth = 0; depth < MAX_DEPTH; ++depth)
    {
        int hitIndex;
        float t;
        vec3 hitNormal;
        vec2 hitTexCoord;
        if (BVHIntersection(ray, hitIndex, t, hitNormal, hitTexCoord))
        {
            vec3 ambient = GetAmbient(hitIndex, hitTexCoord);
            vec3 diffuse = GetDiffuse(hitIndex, hitTexCoord);
            vec3 specular = GetSpecular(hitIndex, hitTexCoord);

            vec3 normal = hitNormal;

            float metallic = GetMetallic(hitIndex, hitTexCoord);
            float roughness = GetRoughness(hitIndex, hitTexCoord);
            float AO = GetAO(hitIndex, hitTexCoord);
            vec3 emission = GetEmission(hitIndex, hitTexCoord);
            float alpha = GetAlpha(hitIndex, hitTexCoord);

            //float illum = GetIllum(hitIndex);
            float illum = 4;
            float dissolve = GetDissolve(hitIndex);
            // 计算交点
            vec3 hitPoint = ray.origin + ray.dir * t + normal * 0.0001;

            if(alpha < 0.01){
                ray.origin = hitPoint + ray.dir * 0.01;
                continue;
            }
            // 计算方向光
            vec3 viewDir = normalize(ray.dir);// 从光线起点到交点的向量
            vec3 ambientColor = CalculateAmbientColor(ambient, directionalLight.lightAmbient, AO);
            vec3 diffuseColor = vec3(0.0);
            //漫反射
            if(illum > 0){
                diffuseColor = CalculateDiffuseColor(diffuse, directionalLight.lightDiffuse, directionalLight.lightDir, normal);
            }
            vec3 specularColor = vec3(0.0);
            if(illum > 1){
                specularColor = CalculateSpecularColor(specular, directionalLight.lightSpecular, directionalLight.lightDir, normal, viewDir, roughness, metallic);
            }
            float dirLightShadow = CalculateDirectionShadow(hitPoint, directionalLight.lightDir, normal);
            radiance += vec4( throughput * ( emission + ambientColor + (1 - dirLightShadow) * (diffuseColor + specularColor) ) , alpha);

            // 计算点光源
            for(int i = 0; i < numPointLights; ++i){
                vec3 lightDir = normalize(pointLights[i].lightPos - hitPoint);
                vec3 pointDiffuse = vec3(0.0);
                if( illum > 0 ){
                    pointDiffuse = CalculateDiffuseColor(diffuse, pointLights[i].lightDiffuse, lightDir, normal);
                }
                vec3 pointSpecular = vec3(0.0);
                if(illum > 1) {
                    pointSpecular = CalculateSpecularColor(specular, pointLights[i].lightSpecular, lightDir, normal, viewDir, roughness, metallic);
                }float distance = distance(hitPoint, pointLights[i].lightPos);
                float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
                float pointShadow = CalculatePointShadow(hitPoint, pointLights[i].lightPos, distance, normal);
                pointDiffuse = pointDiffuse * attenuation;
                pointSpecular = pointSpecular * attenuation;
                radiance += vec4( (1 - pointShadow) * (pointDiffuse + pointSpecular), alpha);
            }
            vec3 N = normal;
            vec3 V = ray.dir;
            vec3 L = -reflect(ray.dir, normal);
            vec3 F0 = mix(vec3(0.04), specular, metallic);
            vec3 CookBrdf = CookTorranceBRDF(N, V, L, F0, roughness);
            
            throughput *= mix(vec3(0.0), CookBrdf, metallic);
            //停止采样
            if(metallic < 0.1){
                break;
            }

            ray.origin += ray.dir * t + normal * 0.0001;
            ray.dir = -L;
        }
        else
        {
            radiance += vec4( throughput * vec3(0.0), 1.0f);
            break;
        }
    }
    return radiance;
}