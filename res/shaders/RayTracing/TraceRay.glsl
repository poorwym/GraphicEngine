vec3 GetSky(vec3 dir){
    return texture(skybox, normalize(dir)).rgb;
}

vec3 CalculateReflectDirection(vec3 direction,vec3 normal, float specExp){
    vec3 R = reflect(direction, normal); // VΪ���䷽��, NΪ����
    float xi1 = rand(normal.xy * seed); // [0,1]���������
    float xi2 = rand(direction.xy * seed); // [0,1]���������

    float phi = 2.0 * PI * xi1;
    // ��xi2ͨ�� n+1 �η����������Ʒֲ�
    float cosTheta = pow(xi2, 1.0 / (specExp + 1.0));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 w = normalize(R);
    vec3 helper = (abs(w.z) < 0.999f) ? vec3(0.0,0.0,1.0) : vec3(1.0,0.0,0.0);
    vec3 u = normalize(cross(helper, w));
    vec3 v = cross(w, u);

    vec3 localDir = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    vec3 newDir = normalize(u * localDir.x + v * localDir.y + w * localDir.z);
    return newDir;

}

vec4 TraceRay(Ray ray, vec3 throughput)
{
    vec4 radiance = vec4(0.0);
    for (int depth = 0; depth < MAX_DEPTH; ++depth)
    {
        int hitIndex;
        float t;
        vec3 hitNormal;
        vec2 hitTexCoord;
        if (BVHIntersection(ray, hitIndex, t, hitNormal, hitTexCoord, false))
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
            // ���㽻��
            vec3 hitPoint = ray.origin + ray.dir * t;

            if(alpha < 0.01){
                ray.origin = hitPoint + ray.dir * 0.01;
                continue;
            }
            // ���㷽���
            vec3 viewDir = -normalize(ray.dir); // �ӹ�����㵽���������
            for(int i = 0; i < numDirectionalLights; ++i)
            {
                vec3 ambientColor = CalculateAmbientColor(ambient, directionalLights[i].lightAmbient, AO);
                vec3 diffuseColor = vec3(0.0);
                // ������
                if(illum > 0){
                    diffuseColor = CalculateDiffuseColor(diffuse, directionalLights[i].lightDiffuse, directionalLights[i].lightDir, normal);
                }
                vec3 specularColor = vec3(0.0);
                if(illum > 1){
                    specularColor = CalculateSpecularColor(specular, directionalLights[i].lightSpecular, directionalLights[i].lightDir, normal, viewDir, roughness, metallic);
                }
                float dirLightShadow = CalculateDirectionShadow(hitPoint, directionalLights[i].lightDir, normal);
                radiance += vec4( throughput * ( emission + ambientColor + (1 - dirLightShadow) * (diffuseColor + specularColor) ) , alpha);
            }
            // ������Դ
            for(int i = 0; i < numPointLights; ++i){
                vec3 lightDir = normalize(pointLights[i].lightPos - hitPoint);
                vec3 pointDiffuse = vec3(0.0);
                if( illum > 0 ){
                    pointDiffuse = CalculateDiffuseColor(diffuse, pointLights[i].lightDiffuse, lightDir, normal);
                }
                vec3 pointSpecular = vec3(0.0);
                if(illum > 1) {
                    pointSpecular = CalculateSpecularColor(specular, pointLights[i].lightSpecular, lightDir, normal, viewDir, roughness, metallic);
                }
                float distance = distance(hitPoint, pointLights[i].lightPos);
                float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
                float pointShadow = CalculatePointShadow(hitPoint, pointLights[i].lightPos, distance, normal);
                pointDiffuse = pointDiffuse * attenuation;
                pointSpecular = pointSpecular * attenuation;
                radiance += vec4( (1 - pointShadow) * (pointDiffuse + pointSpecular), alpha);
            }
            vec3 N = normal;
            vec3 V = ray.dir;
            vec3 L = reflect(-ray.dir, normal);
            vec3 F0 = mix(vec3(0.04), specular, metallic);
            vec3 CookBrdf = CookTorranceBRDF(N, V, L, F0, roughness);

            throughput *= CookBrdf;

            // ����˹���̲�����ֹ�߼���ʼ
            // ���㵱ǰ·���ĸ���
            float maxThroughput = max(max(throughput.r, throughput.g), throughput.b);
            // ����һ����͵ļ����������ʣ��Է�ֹ���ʹ��͵��¹�����ֹ
            float survivalProbability = clamp(maxThroughput, 0.1, 1.0);

            // ����һ������� [0,1)
            float randSample = rand(hitPoint.xy * seed * float(depth));

            if(randSample > survivalProbability)
            {
                // ·����ֹ
                break;
            }

            // ����������������������ͨ��
            throughput /= survivalProbability;
            // ����˹���̲�����ֹ�߼�����

            // ���¹������ͷ���
            vec3 relectDir = CalculateReflectDirection(ray.dir, normal, GetSpecularExponent(hitIndex));
            ray.origin = hitPoint + relectDir * 0.0001;
            ray.dir = CalculateReflectDirection(ray.dir, normal, GetSpecularExponent(hitIndex));
        }
        else
        {
            radiance += vec4(throughput * GetSky(ray.dir), 1.0f);
            break;
        }
    }
    return radiance;
}
