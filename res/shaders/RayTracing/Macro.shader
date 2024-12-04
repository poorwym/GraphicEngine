
#define ALBEDO_MAP_INDEX 0
#define NORMAL_MAP_INDEX 1
#define METALLIC_MAP_INDEX 2
#define ROUGHNESS_MAP_INDEX 3
#define AO_MAP_INDEX 4
#define EMISSION_MAP_INDEX 5
#define ALPHA_MAP_INDEX 6
#define HEIGHT_MAP_INDEX 7

#define MAX_POINT_LIGHTS 3 // �����Դ����
#define MAX_DEPTH 1  // ���ݹ����

const float PI = 3.14159265359;

#define PCF_SAMPLES 4  // PCF �������������� 2x2 ����
const float shadowEpsilon = 0.0001; // ��ֹ�����ڵ���Сƫ����

/*
	illum
	?	0 - �������䣬�޹���Ч����
	?	1 - ������ͻ����⡣
	?	2 - �����䡢������;��淴�䡣
	?	3 - �������ͷ���Ч���������ڲ�����ˮ�Ȳ��ʡ�
*/