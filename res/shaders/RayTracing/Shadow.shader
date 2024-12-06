
// 预定义的采样偏移（2x2 网格）
vec2 pcfOffsets[PCF_SAMPLES] = vec2[](vec2(-0.25, -0.25),
                                     vec2(0.25, -0.25),
                                     vec2(-0.25, 0.25),
                                     vec2(0.25, 0.25));

float CalculateDirectionShadow(vec3 hitPoint, vec3 lightDir, vec3 normal){
    float shadow = 0.0;

    // 定义采样半径，可以根据需要调整
    float radius = 0.05;

    // 生成正交基（tangent 和 bitangent）
    vec3 up = abs(lightDir.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, lightDir));
    vec3 bitangent = cross(lightDir, tangent);

    for(int i = 0; i < PCF_SAMPLES; ++i){
        // 生成随机偏移
        float randX = rand(gl_FragCoord.xy + float(i));
        float randY = rand(gl_FragCoord.xy + float(i) + 1.0);
        vec2 jitter = vec2(randX, randY) * 2.0 - vec2(1.0); // 范围 [-1, 1]
        vec2 offset = pcfOffsets[i] * radius + jitter * (radius * 0.5); // 结合预定义偏移和随机抖动

        // 生成偏移后的光线方向
        vec3 perturbedLightDir = normalize(lightDir + tangent * offset.x + bitangent * offset.y);

        // 偏移光线起点，避免自我遮挡
        vec3 origin = hitPoint + normal * shadowEpsilon;

        // 构建阴影光线
        Ray shadowRay;
        shadowRay.origin = origin;
        shadowRay.dir = perturbedLightDir;

        float t1, t2;
        vec3 t3;
        vec2 t4;

        // 进行光线与场景的相交测试
        shadow += BVHIntersection(shadowRay, t1, t2, t3, t4) ? 1.0 : 0.0;
    }

    // 计算平均阴影因子
    shadow /= float(PCF_SAMPLES);

    return shadow; // 返回 0.0（无遮挡）到 1.0（完全遮挡）的阴影因子
}

float CalculatePointShadow(vec3 hitPoint, vec3 lightDir, float distance, vec3 normal){
    float shadow = 0.0;

    // 定义采样半径，可以根据需要调整
    float radius = 0.5;

    // 计算原始光源位置
    vec3 originalLightPos = hitPoint + lightDir * distance;

    // 生成正交基（tangent 和 bitangent）
    vec3 up = abs(lightDir.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, lightDir));
    vec3 bitangent = cross(lightDir, tangent);

    for(int i = 0; i < PCF_SAMPLES; ++i){
        // 生成随机偏移
        float randX = rand(gl_FragCoord.xy + float(i));
        float randY = rand(gl_FragCoord.xy + float(i) + 1.0);
        vec2 jitter = vec2(randX, randY) * 2.0 - vec2(1.0); // 范围 [-1, 1]
        vec2 offset = pcfOffsets[i] * radius + jitter * (radius * 0.5); // 结合预定义偏移和随机抖动

        // 生成偏移后的光源位置
        vec3 perturbedLightPos = originalLightPos + tangent * offset.x + bitangent * offset.y;

        // 计算新的光线方向和新的距离
        vec3 newLightDir = normalize(perturbedLightPos - hitPoint);
        float newDistance = length(perturbedLightPos - hitPoint);

        // 偏移光线起点，避免自我遮挡
        vec3 origin = hitPoint + newLightDir * shadowEpsilon;

        // 构建阴影光线
        Ray shadowRay;
        shadowRay.origin = origin;
        shadowRay.dir = newLightDir;

        float t1, t_distance;
        vec3 t3;
        vec2 t4;

        // 进行光线与场景的相交测试
        if(BVHIntersection(shadowRay, t1, t_distance, t3, t4)){
            if(t_distance < newDistance){
                shadow += 1.0; // 遮挡
            }
        }
    }

    // 计算平均阴影因子
    shadow /= float(PCF_SAMPLES);

    return shadow; // 返回 0.0（无遮挡）到 1.0（完全遮挡）的阴影因子
}