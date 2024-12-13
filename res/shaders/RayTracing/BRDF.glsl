
vec3 FresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


// º∆À„ Cook-Torrance BRDF
vec3 CookTorranceBRDF(vec3 N, vec3 V, vec3 L, vec3 F0, float roughness){
    vec3 H = normalize(V + L);
    float NDF = GGXDistribution(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;

    return specular;
}
