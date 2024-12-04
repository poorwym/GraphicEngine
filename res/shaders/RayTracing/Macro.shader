
#define ALBEDO_MAP_INDEX 0
#define NORMAL_MAP_INDEX 1
#define METALLIC_MAP_INDEX 2
#define ROUGHNESS_MAP_INDEX 3
#define AO_MAP_INDEX 4
#define EMISSION_MAP_INDEX 5
#define ALPHA_MAP_INDEX 6
#define HEIGHT_MAP_INDEX 7

#define MAX_POINT_LIGHTS 3 // 最大点光源数量
#define MAX_DEPTH 1  // 最大递归深度

const float PI = 3.14159265359;

#define PCF_SAMPLES 4  // PCF 采样次数（例如 2x2 网格）
const float shadowEpsilon = 0.0001; // 防止自我遮挡的小偏移量

/*
	illum
	?	0 - 仅漫反射，无光泽效果。
	?	1 - 漫反射和环境光。
	?	2 - 漫反射、环境光和镜面反射。
	?	3 - 添加折射和反射效果，常用于玻璃、水等材质。
*/