// GLTFManager.cpp
#include "GLTFManager.h"
#include <iostream>
#include "SceneManager.h"

extern SceneManager g_SceneManager;
// ���캯��
GLTFManager::GLTFManager() {}

// ��������
GLTFManager::~GLTFManager() {}

// LoadGLTF: ���� GLTF �ļ������ظ��ڵ�
SceneNode* GLTFManager::LoadGLTF(const std::string& filePath, const std::string& fileName, float scaleRate) {
    if (!LoadModel(filePath, fileName)) {
        std::cerr << "Failed to load GLTF model: " << fileName << std::endl;
        return nullptr;
    }

    SceneNode* root = new SceneNode("Root", static_cast<void*>(nullptr), nullptr);

    // �������г�����ͨ��ֻ��һ����
    for (const auto& scene : model.scenes) {
        for (const auto& nodeIndex : scene.nodes) {
            ProcessNode(nodeIndex, root, scaleRate);
        }
    }

    return root;
}

// LoadModel: ʹ�� TinyGLTF ����ģ��
bool GLTFManager::LoadModel(const std::string& filePath, const std::string& fileName) {
    std::string fullPath = filePath + fileName;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, fullPath);

    if (!warn.empty()) {
        std::cout << "TinyGLTF Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "TinyGLTF Error: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load GLTF file: " << fullPath << std::endl;
        return false;
    }

    return true;
}

// ProcessNode: �ݹ鴦�� GLTF �Ľڵ㣬��������״�ṹ
SceneNode* GLTFManager::ProcessNode(int nodeIndex, SceneNode* parent, float scaleRate) {
    const tinygltf::Node& node = model.nodes[nodeIndex];
    std::string nodeName = node.name.empty() ? "Unnamed_Node_" + std::to_string(nodeIndex) : node.name;

    // ��ȡ�任����
    glm::mat4 localTransform = GetTransform(node);

    // ���� Entity
    Entity* entity = new Entity(nodeName);
    // ��ȡλ�á���ת������
    glm::vec3 position, rotation, scale;
    // �����Ϊ TRS
    position = glm::vec3(localTransform[3]);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);

    entity->SetPosition(position);
    entity->SetRotation(rotation);
    entity->SetScale(scale);

    // ���� SceneNode
    SceneNode* sceneNode = new SceneNode(nodeName, entity, parent);

    // ����ڵ������񣬴�������
    if (node.mesh >= 0) {
        MeshComponent* meshComponent = ProcessMesh(node.mesh, scaleRate);
        if (meshComponent) {
            entity->AddComponent(meshComponent);
        }
    }

    // �ݹ鴦���ӽڵ�
    for (const auto& childIndex : node.children) {
        ProcessNode(childIndex, sceneNode, scaleRate);
    }

    return sceneNode;
}

// ProcessMesh: �� GLTF ����ת��Ϊ����� MeshComponent �� Mesh
MeshComponent* GLTFManager::ProcessMesh(int meshIndex, float scaleRate) {
    if (meshIndex < 0 || meshIndex >= model.meshes.size()) {
        std::cerr << "Invalid mesh index: " << meshIndex << std::endl;
        return nullptr;
    }

    const tinygltf::Mesh& mesh = model.meshes[meshIndex];
    MeshComponent* meshComponent = new MeshComponent();

    for (const auto& primitive : mesh.primitives) {
        // ����������������
        if (primitive.mode != TINYGLTF_MODE_TRIANGLES) continue;

        // ��ȡ��������
        if (primitive.attributes.find("POSITION") == primitive.attributes.end()) {
            std::cerr << "Mesh primitive missing POSITION attribute." << std::endl;
            continue;
        }

        const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
        const tinygltf::Accessor& normAccessor = (primitive.attributes.find("NORMAL") != primitive.attributes.end()) ?
            model.accessors[primitive.attributes.find("NORMAL")->second] :
            tinygltf::Accessor();
        const tinygltf::Accessor& texAccessor = (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) ?
            model.accessors[primitive.attributes.find("TEXCOORD_0")->second] :
            tinygltf::Accessor();
        const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices];

        // ��ȡ��������ͼ
        const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
        const tinygltf::BufferView& normView = (normAccessor.bufferView >= 0 && normAccessor.bufferView < model.bufferViews.size()) ?
            model.bufferViews[normAccessor.bufferView] :
            tinygltf::BufferView();
        const tinygltf::BufferView& texView = (texAccessor.bufferView >= 0 && texAccessor.bufferView < model.bufferViews.size()) ?
            model.bufferViews[texAccessor.bufferView] :
            tinygltf::BufferView();
        const tinygltf::BufferView& indicesView = model.bufferViews[indicesAccessor.bufferView];

        // ��ȡ������
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
        const tinygltf::Buffer& normBuffer = (normAccessor.bufferView >= 0 && normAccessor.bufferView < model.buffers.size()) ?
            model.buffers[normView.buffer] :
            tinygltf::Buffer();
        const tinygltf::Buffer& texBuffer = (texAccessor.bufferView >= 0 && texAccessor.bufferView < model.buffers.size()) ?
            model.buffers[texView.buffer] :
            tinygltf::Buffer();
        const tinygltf::Buffer& indicesBuffer = model.buffers[indicesView.buffer];

        // ��ȡ��������
        std::vector<Vertex> vertices;
        vertices.reserve(posAccessor.count);
        for (size_t v = 0; v < posAccessor.count; v++) {
            Vertex vertex;
            // λ��
            const float* pos = reinterpret_cast<const float*>(&posBuffer.data[posView.byteOffset + posAccessor.byteOffset + v * sizeof(float) * 3]);
            vertex.Position = glm::vec4(pos[0] * scaleRate, pos[1] * scaleRate, pos[2] * scaleRate, 1.0f);

            // ���ߣ�����У�
            if (normAccessor.bufferView >= 0 && normAccessor.bufferView < model.accessors.size()) {
                const float* norm = reinterpret_cast<const float*>(&normBuffer.data[normView.byteOffset + normAccessor.byteOffset + v * sizeof(float) * 3]);
                vertex.Normal = glm::vec4(norm[0], norm[1], norm[2], 0.0f);
            }
            else {
                vertex.Normal = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            }

            // UV������У�
            if (texAccessor.bufferView >= 0 && texAccessor.bufferView < model.accessors.size()) {
                const float* tex = reinterpret_cast<const float*>(&texBuffer.data[texView.byteOffset + texAccessor.byteOffset + v * sizeof(float) * 2]);
                vertex.TexCoords = glm::vec4(tex[0], tex[1], 0.0f, 0.0f);
            }
            else {
                vertex.TexCoords = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            }

            // ��ʼ�����ߺ͸����ߣ���ѡ�����һ�����㣩
            vertex.Tangent = glm::vec4(0.0f);
            vertex.Bitangent = glm::vec4(0.0f);

            vertices.push_back(vertex);
        }

        // ��ȡ��������
        std::vector<unsigned int> indices;
        indices.reserve(indicesAccessor.count);
        for (size_t i = 0; i < indicesAccessor.count; i++) {
            unsigned int index = 0;
            size_t byteOffset = indicesView.byteOffset + indicesAccessor.byteOffset + i * (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(unsigned short) : sizeof(unsigned int));
            if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                index = *((unsigned short*)(&indicesBuffer.data[byteOffset]));
            }
            else { // TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT
                index = *((unsigned int*)(&indicesBuffer.data[byteOffset]));
            }
            indices.push_back(index);
        }

        // �������
        PBRMaterial* material = nullptr;
        if (primitive.material >= 0 && primitive.material < model.materials.size()) {
            material = ProcessMaterial(primitive.material, ""); // �滻Ϊ���ַ�����ʵ������·��
        }

        // ���� Mesh ����ӵ� MeshComponent
        Mesh* mesh = new Mesh(vertices, indices, material, scaleRate);
        meshComponent->AddMesh(mesh);
    }

    return meshComponent;
}

// Helper function to convert float array to glm::vec3
static glm::vec3 ConvertToVec3(const float* v)
{
    return glm::vec3(v[0], v[1], v[2]);
}

// Helper function to convert float array to glm::vec4
static glm::vec4 ConvertToVec4(const float* v)
{
    return glm::vec4(v[0], v[1], v[2], v[3]);
}

static glm::vec4 ConvertToVec4(const double* v)
{
    return glm::vec4(float(v[0]), float(v[1]), float(v[2]), float(v[3]));
}

// ProcessMaterial: �� GLTF ����ת��Ϊ����� PBRMaterial
PBRMaterial* GLTFManager::ProcessMaterial(int materialIndex, const std::string& filePath) {
    if (materialIndex < 0 || materialIndex >= static_cast<int>(model.materials.size())) {
        std::cerr << "Invalid material index: " << materialIndex << std::endl;
        return nullptr;
    }
    std::cout << "Load GLTF Material Textures " << filePath << std::endl;

    Material m_Material = default_material;
    const tinygltf::Material& m = model.materials[materialIndex];

    // 1. ������ɫ���� (Ambient �� Diffuse)
    if (m.pbrMetallicRoughness.baseColorFactor.size() == 4) {
        m_Material.Ambient = ConvertToVec4(m.pbrMetallicRoughness.baseColorFactor.data());
        m_Material.Diffuse = m_Material.Ambient; // �򻯴���
    }

    // 2. ������ɫ��ͼ (Albedo)
    if (m.pbrMetallicRoughness.baseColorTexture.index >= 0) {
        int texIndex = m.pbrMetallicRoughness.baseColorTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // ȷ�� texIndex ��Ч
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // ȷ�� source ��Ч
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.AlbedoMapIndex = g_SceneManager.AddTexture(texPath.c_str());
            }
        }
    }

    // 3. �����Ⱥʹֲڶ���ͼ (MetallicRoughness)
    if (m.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
        int texIndex = m.pbrMetallicRoughness.metallicRoughnessTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // ȷ�� texIndex ��Ч
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // ȷ�� source ��Ч
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.MetallicMapIndex = g_SceneManager.AddTexture(texPath.c_str());
                m_Material.RoughnessMapIndex = g_SceneManager.AddTexture(texPath.c_str()); // GLTF ͨ���������Ⱥʹֲڶȴ洢��ͬһ��ͼ�Ĳ�ͬͨ��
            }
        }
    }

    // 4. ������ͼ (Normal)
    if (m.normalTexture.index >= 0) {
        int texIndex = m.normalTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // ȷ�� texIndex ��Ч
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // ȷ�� source ��Ч
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.NormalMapIndex = g_SceneManager.AddTexture(texPath.c_str());
            }
        }
    }

    // 5. AO��ͼ (Ambient Occlusion) - ��� GLTF ���ʰ���
    // ע�⣺��׼ GLTF ������ AO ��ͼ��ͨ����Ҫʹ����չ���Զ��巽��
    // �������û�� AO ��ͼ

    // 6. �������� (Emission)
    if (m.emissiveFactor.size() == 3) {
        m_Material.Emission = ConvertToVec4(m.emissiveFactor.data());
    }

    // 7. ������ͼ (Emission)
    if (m.emissiveTexture.index >= 0) {
        int texIndex = m.emissiveTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // ȷ�� texIndex ��Ч
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // ȷ�� source ��Ч
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.EmissionMapIndex = g_SceneManager.AddTexture(texPath.c_str());
            }
        }
    }

    // 8. Alphaģʽ�Ͳü���ֵ (Alpha Mode and Cutoff)
    if (m.alphaMode == "BLEND" && m.alphaCutoff > 0.0) {
        // GLTF ͨ�������е�����͸����ͼ������ʹ�û�����ɫ��ͼ�� alpha ͨ��
        // �������ʹ�� Albedo ��ͼ�� alpha ͨ����Ϊ͸����
        if (m.pbrMetallicRoughness.baseColorTexture.index >= 0) {
            m_Material.AlphaMapIndex = m_Material.AlbedoMapIndex;
        }
        m_Material.Dissolve = static_cast<float>(m.alphaCutoff); // ʹ�� alphaCutoff ��Ϊ͸����
    }
    else if (m.alphaMode == "MASK" && m.alphaCutoff > 0.0) {
        // ʹ�òü�ģʽ
        if (m.pbrMetallicRoughness.baseColorTexture.index >= 0) {
            m_Material.AlphaMapIndex = m_Material.AlbedoMapIndex;
        }
        m_Material.Dissolve = static_cast<float>(m.alphaCutoff); // ʹ�� alphaCutoff ��Ϊ͸����
    }
    else {
        // OPAQUE ģʽ
        m_Material.Dissolve = 1.0f;
    }

    // 9. Specular �� OpticalDensity ������
    // GLTF PBR ��ֱ���ṩ Specular ���ԣ���������Ҫ�Զ���
    // ����򻯴����� Specular ����ΪĬ��ֵ
    m_Material.Specular = glm::vec4(1.0f); // Ĭ��ֵ
    m_Material.OpticalDensity = 1.0f; // Ĭ��ֵ

    // 10. ����ֲڶ�����
    if (m.pbrMetallicRoughness.roughnessFactor > 0.0) {
        m_Material.SpecularExponent = 1.0f / static_cast<float>(m.pbrMetallicRoughness.roughnessFactor); // ��ʾ��
    }

    // 11. �߶���ͼ (HeightMap) - GLTF ��׼��֧�֣���ʹ����չ
    // ������Զ�����չ���ض����󣬿��ڴ˴���

    // 12. ��������Ĭ�ϻ�� GLTF ��ȡ������
    // ������Ҫ��չ

    // 13. Debug���������Ϣ
    std::cout << "Material loaded: " << std::endl;
    std::cout << "  AlbedoMapIndex: " << m_Material.AlbedoMapIndex << std::endl;
    std::cout << "  NormalMapIndex: " << m_Material.NormalMapIndex << std::endl;
    std::cout << "  MetallicMapIndex: " << m_Material.MetallicMapIndex << std::endl;
    std::cout << "  RoughnessMapIndex: " << m_Material.RoughnessMapIndex << std::endl;
    std::cout << "  EmissionMapIndex: " << m_Material.EmissionMapIndex << std::endl;
    std::cout << "  AlphaMapIndex: " << m_Material.AlphaMapIndex << std::endl;
    std::cout << "  Dissolve: " << m_Material.Dissolve << std::endl;
    std::cout << "  SpecularExponent: " << m_Material.SpecularExponent << std::endl;
}
static glm::vec3 quatToEuler(const glm::vec4& q) {
    glm::vec3 euler;

    // Roll (x-axis rotation)
    double sinr_cosp = 2.0 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    euler.x = glm::degrees(atan2(sinr_cosp, cosr_cosp));

    // Pitch (y-axis rotation)
    double sinp = 2.0 * (q.w * q.y - q.z * q.x);
    if (abs(sinp) >= 1)
        euler.y = glm::degrees(copysign(glm::half_pi<float>(), sinp)); // use 90 degrees if out of range
    else
        euler.y = glm::degrees(asin(sinp));

    // Yaw (z-axis rotation)
    double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    euler.z = glm::degrees(atan2(siny_cosp, cosy_cosp));

    return euler;
}

// GetTransform: �� GLTF �ڵ�����ȡ�任����
glm::mat4 GLTFManager::GetTransform(const tinygltf::Node& node) {
    glm::mat4 transform = glm::mat4(1.0f);

    if (node.matrix.size() == 16) {
        // ʹ�þ���
        for (int i = 0; i < 16; ++i) {
            transform[i / 4][i % 4] = node.matrix[i];
        }
    }
    else {
        // ʹ�� TRS
        if (node.translation.size() == 3) {
            transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }
        if (node.rotation.size() == 4) {
            glm::vec3 euler = quatToEuler(glm::vec4(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]));
            transform = glm::rotate(transform, glm::radians(euler.z), glm::vec3(0.0f, 0.0f, 1.0f));
            transform = glm::rotate(transform, glm::radians(euler.y), glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::rotate(transform, glm::radians(euler.x), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (node.scale.size() == 3) {
            transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
        }
    }

    return transform;
}