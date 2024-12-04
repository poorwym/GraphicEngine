// 生成伪随机数
float rand(vec2 seed);

// 获取指定材质槽位的纹理颜色
vec3 GetTextureColor(int index, vec2 texCoords, int slot);

// 获取指定材质槽位的纹理数值
float GetTextureValue(int index, vec2 texCoords, int slot);

// 光线追踪函数
vec4 TraceRay(Ray ray, vec3 throughput);

// 计算环境光
vec3 CalculateAmbientColor(vec3 albedo, vec3 ambientLight, float AO); // 环境光

// 计算漫反射光
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal); // 漫反射光

// 计算镜面光
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic); // 镜面光

// 计算方向光阴影
float CalculateDirectionShadow(vec3 hitPoint, vec3 lightDir, vec3 normal); // 计算方向光阴影

// 计算点光源阴影
float CalculatePointShadow(vec3 hitPoint, vec3 lightDir, float distance, vec3 normal); // 计算点光源阴影

// BVH 树的光线相交测试
bool BVHIntersection(Ray ray, out int hitIndex, out float tMin, out vec3 hitNormal, out vec2 hitTexCoord);

// Möller-Trumbore 算法实现的光线与三角形相交测试
bool RayIntersectsTriangle(Ray ray, Triangle tri, out float t, out vec3 hitNormal, out vec2 hitTexCoord, int index);

// 场景光线相交测试
bool SceneIntersection(Ray ray, out int hitIndex, out float tMin, out vec3 hitNormal, out vec2 hitTexCoord);

// AABB 与 Ray 的相交测试
bool AABBIntersect(Ray ray, AABB aabb, out float tMin, out float tMax);

// 获取材质的环境光颜色（Ambient）
vec3 GetAmbient(int hitIndex, vec2 hitTexCoord);

// 获取材质的漫反射颜色（Diffuse）
vec3 GetDiffuse(int hitIndex, vec2 hitTexCoord);

// 获取材质的镜面反射颜色（Specular）
vec3 GetSpecular(int hitIndex, vec2 hitTexCoord);

// 获取材质的自发光颜色（Emission）
vec3 GetEmission(int hitIndex, vec2 hitTexCoord);

// 获取材质的环境光遮蔽值（AO）
float GetAO(int hitIndex, vec2 hitTexCoord);

// 获取材质的金属度（Metallic）
float GetMetallic(int hitIndex, vec2 hitTexCoord);

// 获取材质的法线方向（Normal）
vec3 GetNormal(int hitIndex, vec2 hitTexCoord);

// 获取材质的粗糙度（Roughness）
float GetRoughness(int hitIndex, vec2 hitTexCoord);

// 获取材质的透明度（Alpha）
float GetAlpha(int hitIndex, vec2 hitTexCoord);

// 获取材质的光照模型（Illumination Model）
float GetIllum(int hitIndex);

// 获取材质的不透明度（Dissolve）
float GetDissolve(int hitIndex);
