uniform int numMaterials; // 材质数量
uniform int numTriangles; // 三角形数量
uniform sampler2DArray textures; // 纹理
uniform float farPlane; // 远裁剪面
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS]; // 平行光
uniform PointLight pointLights[MAX_POINT_LIGHTS]; // 点光源
uniform int numPointLights;		// 点光源数量
uniform int numDirectionalLights; // 平行光数量
uniform vec3 viewPos;        // 观察者位置
uniform vec3 cameraPosition; // 相机位置
uniform float nearPlane;    // 近裁剪面
uniform int screenWidth;    // 屏幕宽度
uniform int screenHeight;   // 屏幕高度
uniform float fov;          // 视角
uniform float aspectRatio;  // 屏幕宽高比
uniform mat4 viewMatrix;    // 视图矩阵
uniform float seed; // 随机种子

uniform sampler2D depthMap;
uniform samplerCube skybox;