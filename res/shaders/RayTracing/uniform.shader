uniform int numTriangles; // ����������
uniform sampler2DArray textures; // ����
uniform float farPlane; // Զ�ü���
uniform DirectionalLight directionalLight; // ƽ�й�
uniform PointLight pointLights[MAX_POINT_LIGHTS]; // ���Դ
uniform int numPointLights;		// ���Դ����
uniform vec3 viewPos;        // �۲���λ��
uniform vec3 cameraPosition; // ���λ��
uniform float nearPlane;    // ���ü���
uniform int screenWidth;    // ��Ļ����
uniform int screenHeight;   // ��Ļ�߶�
uniform float fov;          // �ӽ�
uniform float aspectRatio;  // ��Ļ���߱�
uniform mat4 viewMatrix;    // ��ͼ����
uniform float seed; // �������

uniform sampler2D depthMap;