// 获取纹理颜色（RGB），基于材质索引和纹理坐标
vec3 GetTextureColor(int index, vec2 texCoord, int type);

// 获取纹理值（单通道），基于材质索引和纹理坐标
float GetTextureValue(int index, vec2 texCoord, int type);

// 获取环境光颜色，优先从材质纹理中获取，否则返回材质定义的环境光值
vec3 GetAmbient(int index, vec2 texCoord);

// 获取漫反射颜色，优先从材质纹理中获取，否则返回材质定义的漫反射值
vec3 GetDiffuse(int index, vec2 texCoord);

// 获取镜面反射颜色，优先从材质纹理中获取，否则返回材质定义的镜面反射值
vec3 GetSpecular(int index, vec2 texCoord);

// 获取发光颜色，优先从材质纹理中获取，否则返回材质定义的发光值
vec3 GetEmission(int index, vec2 texCoord);

// 获取环境光遮蔽（AO）值，优先从材质纹理中获取，否则返回默认值 1.0
float GetAO(int index, vec2 texCoord);

// 获取金属度值，优先从材质纹理中获取，否则基于材质的镜面指数计算
float GetMetallic(int index, vec2 texCoord);

// 获取法线信息，优先从材质纹理中获取，否则返回默认值 (0.0, 0.0, 0.0)
vec3 GetNormal(int index, vec2 texCoord);

// 获取粗糙度值，优先从材质纹理中获取，否则返回默认值 0.8
float GetRoughness(int index, vec2 texCoord);

// 获取透明度值，优先从材质纹理中获取，否则返回默认值 1.0
float GetAlpha(int index, vec2 texCoord);

// 获取光照模型标识符（illum），直接从材质中读取
float GetIllum(int index);

// 获取溶解值（用于透明效果），直接从材质中读取
float GetDissolve(int index);

// 获取高度值（位移贴图），优先从材质纹理中获取，否则返回默认值 0.0
float GetHeight(int index, vec2 texCoord);

// 获取镜面高光指数（Specular Exponent），直接从材质中读取
float GetSpecularExponent(int index);