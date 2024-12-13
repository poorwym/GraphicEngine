uniform int numMaterials; // ��������
uniform int numTriangles; // ����������
uniform sampler2DArray textures; // ����
uniform float farPlane; // Զ�ü���
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS]; // ƽ�й�
uniform PointLight pointLights[MAX_POINT_LIGHTS]; // ���Դ
uniform int numPointLights;		// ���Դ����
uniform int numDirectionalLights; // ƽ�й�����
uniform vec3 viewPos;        // �۲���λ��
uniform vec3 cameraPosition; // ���λ��
uniform float nearPlane;    // ���ü���
uniform int screenWidth;    // ��Ļ���
uniform int screenHeight;   // ��Ļ�߶�
uniform float fov;          // �ӽ�
uniform float aspectRatio;  // ��Ļ��߱�
uniform mat4 viewMatrix;    // ��ͼ����
uniform float seed; // �������

uniform sampler2D depthMap;
uniform samplerCube skybox;