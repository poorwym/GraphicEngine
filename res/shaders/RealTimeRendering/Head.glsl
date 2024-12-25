// ��ȡ������ɫ��RGB�������ڲ�����������������
vec3 GetTextureColor(int index, vec2 texCoord, int type);

// ��ȡ����ֵ����ͨ���������ڲ�����������������
float GetTextureValue(int index, vec2 texCoord, int type);

// ��ȡ��������ɫ�����ȴӲ��������л�ȡ�����򷵻ز��ʶ���Ļ�����ֵ
vec3 GetAmbient(int index, vec2 texCoord);

// ��ȡ��������ɫ�����ȴӲ��������л�ȡ�����򷵻ز��ʶ����������ֵ
vec3 GetDiffuse(int index, vec2 texCoord);

// ��ȡ���淴����ɫ�����ȴӲ��������л�ȡ�����򷵻ز��ʶ���ľ��淴��ֵ
vec3 GetSpecular(int index, vec2 texCoord);

// ��ȡ������ɫ�����ȴӲ��������л�ȡ�����򷵻ز��ʶ���ķ���ֵ
vec3 GetEmission(int index, vec2 texCoord);

// ��ȡ�������ڱΣ�AO��ֵ�����ȴӲ��������л�ȡ�����򷵻�Ĭ��ֵ 1.0
float GetAO(int index, vec2 texCoord);

// ��ȡ������ֵ�����ȴӲ��������л�ȡ��������ڲ��ʵľ���ָ������
float GetMetallic(int index, vec2 texCoord);

// ��ȡ������Ϣ�����ȴӲ��������л�ȡ�����򷵻�Ĭ��ֵ (0.0, 0.0, 0.0)
vec3 GetNormal(int index, vec2 texCoord);

// ��ȡ�ֲڶ�ֵ�����ȴӲ��������л�ȡ�����򷵻�Ĭ��ֵ 0.8
float GetRoughness(int index, vec2 texCoord);

// ��ȡ͸����ֵ�����ȴӲ��������л�ȡ�����򷵻�Ĭ��ֵ 1.0
float GetAlpha(int index, vec2 texCoord);

// ��ȡ����ģ�ͱ�ʶ����illum����ֱ�ӴӲ����ж�ȡ
float GetIllum(int index);

// ��ȡ�ܽ�ֵ������͸��Ч������ֱ�ӴӲ����ж�ȡ
float GetDissolve(int index);

// ��ȡ�߶�ֵ��λ����ͼ�������ȴӲ��������л�ȡ�����򷵻�Ĭ��ֵ 0.0
float GetHeight(int index, vec2 texCoord);

// ��ȡ����߹�ָ����Specular Exponent����ֱ�ӴӲ����ж�ȡ
float GetSpecularExponent(int index);