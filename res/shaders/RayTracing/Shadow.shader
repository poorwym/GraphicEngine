
// Ԥ����Ĳ���ƫ�ƣ�2x2 ����
vec2 pcfOffsets[PCF_SAMPLES] = vec2[](vec2(-0.25, -0.25),
                                     vec2(0.25, -0.25),
                                     vec2(-0.25, 0.25),
                                     vec2(0.25, 0.25));

float CalculateDirectionShadow(vec3 hitPoint, vec3 lightDir, vec3 normal){
    float shadow = 0.0;

    // ��������뾶�����Ը�����Ҫ����
    float radius = 0.05;

    // ������������tangent �� bitangent��
    vec3 up = abs(lightDir.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, lightDir));
    vec3 bitangent = cross(lightDir, tangent);

    for(int i = 0; i < PCF_SAMPLES; ++i){
        // �������ƫ��
        float randX = rand(gl_FragCoord.xy + float(i));
        float randY = rand(gl_FragCoord.xy + float(i) + 1.0);
        vec2 jitter = vec2(randX, randY) * 2.0 - vec2(1.0); // ��Χ [-1, 1]
        vec2 offset = pcfOffsets[i] * radius + jitter * (radius * 0.5); // ���Ԥ����ƫ�ƺ��������

        // ����ƫ�ƺ�Ĺ��߷���
        vec3 perturbedLightDir = normalize(lightDir + tangent * offset.x + bitangent * offset.y);

        // ƫ�ƹ�����㣬���������ڵ�
        vec3 origin = hitPoint + normal * shadowEpsilon;

        // ������Ӱ����
        Ray shadowRay;
        shadowRay.origin = origin;
        shadowRay.dir = perturbedLightDir;

        float t1, t2;
        vec3 t3;
        vec2 t4;

        // ���й����볡�����ཻ����
        shadow += BVHIntersection(shadowRay, t1, t2, t3, t4) ? 1.0 : 0.0;
    }

    // ����ƽ����Ӱ����
    shadow /= float(PCF_SAMPLES);

    return shadow; // ���� 0.0�����ڵ����� 1.0����ȫ�ڵ�������Ӱ����
}

float CalculatePointShadow(vec3 hitPoint, vec3 lightDir, float distance, vec3 normal){
    float shadow = 0.0;

    // ��������뾶�����Ը�����Ҫ����
    float radius = 0.5;

    // ����ԭʼ��Դλ��
    vec3 originalLightPos = hitPoint + lightDir * distance;

    // ������������tangent �� bitangent��
    vec3 up = abs(lightDir.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, lightDir));
    vec3 bitangent = cross(lightDir, tangent);

    for(int i = 0; i < PCF_SAMPLES; ++i){
        // �������ƫ��
        float randX = rand(gl_FragCoord.xy + float(i));
        float randY = rand(gl_FragCoord.xy + float(i) + 1.0);
        vec2 jitter = vec2(randX, randY) * 2.0 - vec2(1.0); // ��Χ [-1, 1]
        vec2 offset = pcfOffsets[i] * radius + jitter * (radius * 0.5); // ���Ԥ����ƫ�ƺ��������

        // ����ƫ�ƺ�Ĺ�Դλ��
        vec3 perturbedLightPos = originalLightPos + tangent * offset.x + bitangent * offset.y;

        // �����µĹ��߷�����µľ���
        vec3 newLightDir = normalize(perturbedLightPos - hitPoint);
        float newDistance = length(perturbedLightPos - hitPoint);

        // ƫ�ƹ�����㣬���������ڵ�
        vec3 origin = hitPoint + newLightDir * shadowEpsilon;

        // ������Ӱ����
        Ray shadowRay;
        shadowRay.origin = origin;
        shadowRay.dir = newLightDir;

        float t1, t_distance;
        vec3 t3;
        vec2 t4;

        // ���й����볡�����ཻ����
        if(BVHIntersection(shadowRay, t1, t_distance, t3, t4)){
            if(t_distance < newDistance){
                shadow += 1.0; // �ڵ�
            }
        }
    }

    // ����ƽ����Ӱ����
    shadow /= float(PCF_SAMPLES);

    return shadow; // ���� 0.0�����ڵ����� 1.0����ȫ�ڵ�������Ӱ����
}