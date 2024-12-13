#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include "GLTFManager.h"
#include <iostream>
#include "SceneManager.h"

extern SceneManager g_SceneManager;
// 构造函数
GLTFManager::GLTFManager() {}

// 析构函数
GLTFManager::~GLTFManager() {}

// LoadGLTF: 加载 GLTF 文件并返回根节点
SceneNode* GLTFManager::Load(const std::string& filePath, const std::string& fileName, float scaleRate) {
    if (!LoadModel(filePath, fileName)) {
        std::cerr << "Failed to load GLTF model: " << fileName << std::endl;
        return nullptr;
    }

    SceneNode* root = new SceneNode("Root", static_cast<void*>(nullptr), nullptr);

    // 遍历所有场景（通常只有一个）
    for (const auto& scene : model.scenes) {
        for (const auto& nodeIndex : scene.nodes) {
            std::cout << "Node name: " << scene.nodes[nodeIndex] << std::endl;
            //ProcessNode(nodeIndex, root, scaleRate, filePath);
            SceneNode* sceneNode = ProcessNode(scene.nodes[nodeIndex], root, scaleRate, filePath);
            root->AddChild(sceneNode);
        }
    }

    return root;
}

// LoadModel: 使用 TinyGLTF 加载模型
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

// ProcessNode: 递归处理 GLTF 的节点，并保持树状结构
SceneNode* GLTFManager::ProcessNode(int nodeIndex, SceneNode* parent, float scaleRate, const std::string& filePath) {
    const tinygltf::Node& node = model.nodes[nodeIndex];
    std::string nodeName = node.name.empty() ? "Unnamed_Node_" + std::to_string(nodeIndex) : node.name;

    // 获取变换矩阵
    glm::mat4 localTransform = GetTransform(node);

    

    // 创建 SceneNode
    SceneNode* sceneNode = nullptr;

    // 如果节点有网格，处理网格
    if (node.mesh >= 0) {
        // 创建 Entity
        Entity* entity = new Entity(nodeName + "Entity");
        // 提取位置、旋转和缩放
        glm::vec3 position, rotation, scale;
        // 解矩阵为 TRS
        position = glm::vec3(localTransform[3]);
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f);

        entity->SetPosition(position);
        entity->SetRotation(rotation);
        entity->SetScale(scale);
        MeshComponent* meshComponent = ProcessMesh(node.mesh, scaleRate, filePath);
        if (meshComponent) {
            entity->AddComponent(meshComponent);
        }
        sceneNode = new SceneNode(nodeName, entity, parent);
    }
    else {
        sceneNode = new SceneNode(nodeName, static_cast<void*>(nullptr), parent);
    }

    // 递归处理子节点
    for (const auto& childIndex : node.children) {
        SceneNode* childNode = ProcessNode(childIndex, sceneNode, scaleRate, filePath);
        sceneNode->AddChild(childNode);
    }

    return sceneNode;
}
// Helper function to convert TRIANGLE_STRIP to TRIANGLES
static std::vector<unsigned int> ConvertTriangleStripToTriangles(const std::vector<unsigned int>& stripIndices) {
    std::vector<unsigned int> triangles;
    if (stripIndices.size() < 3) return triangles;

    for (size_t i = 0; i + 2 < stripIndices.size(); ++i) {
        if (i % 2 == 0) {
            // Even index: (v0, v1, v2)
            triangles.push_back(stripIndices[i]);
            triangles.push_back(stripIndices[i + 1]);
            triangles.push_back(stripIndices[i + 2]);
        }
        else {
            // Odd index: (v1, v0, v2) to maintain consistent winding
            triangles.push_back(stripIndices[i + 1]);
            triangles.push_back(stripIndices[i]);
            triangles.push_back(stripIndices[i + 2]);
        }
    }
    return triangles;
}

// Helper function to convert TRIANGLE_FAN to TRIANGLES
static std::vector<unsigned int> ConvertTriangleFanToTriangles(const std::vector<unsigned int>& fanIndices) {
    std::vector<unsigned int> triangles;
    if (fanIndices.size() < 3) return triangles;

    for (size_t i = 1; i + 1 < fanIndices.size(); ++i) {
        triangles.push_back(fanIndices[0]);
        triangles.push_back(fanIndices[i]);
        triangles.push_back(fanIndices[i + 1]);
    }
    return triangles;
}

MeshComponent* GLTFManager::ProcessMesh(int meshIndex, float scaleRate, const std::string& filePath) {
    if (meshIndex < 0 || meshIndex >= static_cast<int>(model.meshes.size())) {
        std::cerr << "Invalid mesh index: " << meshIndex << std::endl;
        return nullptr;
    }

    const tinygltf::Mesh& mesh = model.meshes[meshIndex];
    MeshComponent* meshComponent = new MeshComponent();

    // 遍历 primitive
    for (const auto& primitive : mesh.primitives) {
        // 检查模式
        if (primitive.mode != TINYGLTF_MODE_TRIANGLES &&
            primitive.mode != TINYGLTF_MODE_TRIANGLE_STRIP &&
            primitive.mode != TINYGLTF_MODE_TRIANGLE_FAN) {
            continue; // 不支持的模式跳过
        }

        // 检查 POSITION attribute
        auto posIt = primitive.attributes.find("POSITION");
        if (posIt == primitive.attributes.end()) {
            std::cerr << "Mesh primitive missing POSITION attribute." << std::endl;
            continue;
        }

        int posAccessorIndex = posIt->second;
        if (posAccessorIndex < 0 || posAccessorIndex >= static_cast<int>(model.accessors.size())) {
            std::cerr << "Invalid POSITION accessor index: " << posAccessorIndex << std::endl;
            continue;
        }
        const tinygltf::Accessor& posAccessor = model.accessors[posAccessorIndex];

        // 检查 NORMAL attribute（可选）
        int normAccessorIndex = -1;
        auto normIt = primitive.attributes.find("NORMAL");
        if (normIt != primitive.attributes.end()) {
            normAccessorIndex = normIt->second;
            if (normAccessorIndex < 0 || normAccessorIndex >= static_cast<int>(model.accessors.size())) {
                std::cerr << "Invalid NORMAL accessor index: " << normAccessorIndex << std::endl;
                continue;
            }
        }
        const tinygltf::Accessor normAccessor = (normAccessorIndex >= 0) ? model.accessors[normAccessorIndex] : tinygltf::Accessor();

        // 检查 TEXCOORD_0 attribute（可选）
        int texAccessorIndex = -1;
        auto texIt = primitive.attributes.find("TEXCOORD_0");
        if (texIt != primitive.attributes.end()) {
            texAccessorIndex = texIt->second;
            if (texAccessorIndex < 0 || texAccessorIndex >= static_cast<int>(model.accessors.size())) {
                std::cerr << "Invalid TEXCOORD_0 accessor index: " << texAccessorIndex << std::endl;
                continue;
            }
        }
        const tinygltf::Accessor texAccessor = (texAccessorIndex >= 0) ? model.accessors[texAccessorIndex] : tinygltf::Accessor();

        // 检查 indicesAccessor
        if (primitive.indices < 0 || primitive.indices >= static_cast<int>(model.accessors.size())) {
            std::cerr << "Invalid indices accessor index: " << primitive.indices << std::endl;
            continue;
        }
        const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices];

        // 检查各个 accessor 的 bufferView 范围
        if (posAccessor.bufferView < 0 || posAccessor.bufferView >= static_cast<int>(model.bufferViews.size())) {
            std::cerr << "posAccessor.bufferView out of range." << std::endl;
            continue;
        }

        if (normAccessorIndex >= 0 && (normAccessor.bufferView < 0 || normAccessor.bufferView >= static_cast<int>(model.bufferViews.size()))) {
            std::cerr << "normAccessor.bufferView out of range." << std::endl;
            continue;
        }

        if (texAccessorIndex >= 0 && (texAccessor.bufferView < 0 || texAccessor.bufferView >= static_cast<int>(model.bufferViews.size()))) {
            std::cerr << "texAccessor.bufferView out of range." << std::endl;
            continue;
        }

        if (indicesAccessor.bufferView < 0 || indicesAccessor.bufferView >= static_cast<int>(model.bufferViews.size())) {
            std::cerr << "indicesAccessor.bufferView out of range." << std::endl;
            continue;
        }

        // 获取 BufferView
        const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
        const tinygltf::BufferView& normView = (normAccessorIndex >= 0) ? model.bufferViews[normAccessor.bufferView] : tinygltf::BufferView();
        const tinygltf::BufferView& texView = (texAccessorIndex >= 0) ? model.bufferViews[texAccessor.bufferView] : tinygltf::BufferView();
        const tinygltf::BufferView& indicesView = model.bufferViews[indicesAccessor.bufferView];

        // 检查 bufferView 的 buffer 范围
        if (posView.buffer < 0 || posView.buffer >= static_cast<int>(model.buffers.size())) {
            std::cerr << "posView.buffer out of range." << std::endl;
            continue;
        }
        if (normAccessorIndex >= 0 && (normView.buffer < 0 || normView.buffer >= static_cast<int>(model.buffers.size()))) {
            std::cerr << "normView.buffer out of range." << std::endl;
            continue;
        }
        if (texAccessorIndex >= 0 && (texView.buffer < 0 || texView.buffer >= static_cast<int>(model.buffers.size()))) {
            std::cerr << "texView.buffer out of range." << std::endl;
            continue;
        }
        if (indicesView.buffer < 0 || indicesView.buffer >= static_cast<int>(model.buffers.size())) {
            std::cerr << "indicesView.buffer out of range." << std::endl;
            continue;
        }

        // 获取 Buffer
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
        const tinygltf::Buffer& normBuffer = (normAccessorIndex >= 0) ? model.buffers[normView.buffer] : tinygltf::Buffer();
        const tinygltf::Buffer& texBuffer = (texAccessorIndex >= 0) ? model.buffers[texView.buffer] : tinygltf::Buffer();
        const tinygltf::Buffer& indicesBuffer = model.buffers[indicesView.buffer];

        // 检查数据大小避免越界
        size_t posStride = (posAccessor.ByteStride(posView) > 0) ? posAccessor.ByteStride(posView) : (3 * sizeof(float));
        size_t posDataSizeNeeded = posView.byteOffset + posAccessor.byteOffset + posAccessor.count * posStride;
        if (posDataSizeNeeded > posBuffer.data.size()) {
            std::cerr << "Position data out of range in buffer." << std::endl;
            continue;
        }

        size_t normStride = 0;
        size_t normDataSizeNeeded = 0;
        if (normAccessorIndex >= 0) {
            normStride = (normAccessor.ByteStride(normView) > 0) ? normAccessor.ByteStride(normView) : (3 * sizeof(float));
            normDataSizeNeeded = normView.byteOffset + normAccessor.byteOffset + normAccessor.count * normStride;
            if (normDataSizeNeeded > normBuffer.data.size()) {
                std::cerr << "Normal data out of range in buffer." << std::endl;
                continue;
            }
        }

        size_t texStride = 0;
        size_t texDataSizeNeeded = 0;
        if (texAccessorIndex >= 0) {
            texStride = (texAccessor.ByteStride(texView) > 0) ? texAccessor.ByteStride(texView) : (2 * sizeof(float));
            texDataSizeNeeded = texView.byteOffset + texAccessor.byteOffset + texAccessor.count * texStride;
            if (texDataSizeNeeded > texBuffer.data.size()) {
                std::cerr << "Texture coord data out of range in buffer." << std::endl;
                continue;
            }
        }

        size_t indexComponentSize = (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) ? sizeof(unsigned short) : sizeof(unsigned int);
        size_t indexDataSizeNeeded = indicesView.byteOffset + indicesAccessor.byteOffset + indicesAccessor.count * indexComponentSize;
        if (indexDataSizeNeeded > indicesBuffer.data.size()) {
            std::cerr << "Index data out of range in buffer." << std::endl;
            continue;
        }

        // 提取顶点数据
        std::vector<Vertex> vertices;
        vertices.reserve(posAccessor.count);
        for (size_t v = 0; v < posAccessor.count; v++) {
            Vertex vertex;
            // position
            {
                size_t posOffset = posView.byteOffset + posAccessor.byteOffset + v * posStride;
                const float* pos = reinterpret_cast<const float*>(&posBuffer.data[posOffset]);
                vertex.Position = glm::vec4(pos[0] * scaleRate, pos[1] * scaleRate, pos[2] * scaleRate, 1.0f);
            }

            // normal
            if (normAccessorIndex >= 0) {
                size_t normOffset = normView.byteOffset + normAccessor.byteOffset + v * normStride;
                const float* norm = reinterpret_cast<const float*>(&normBuffer.data[normOffset]);
                vertex.Normal = glm::vec4(norm[0], norm[1], norm[2], 0.0f);
            }
            else {
                vertex.Normal = glm::vec4(0.0f);
            }

            // texcoords
            if (texAccessorIndex >= 0) {
                size_t texOffset = texView.byteOffset + texAccessor.byteOffset + v * texStride;
                const float* tex = reinterpret_cast<const float*>(&texBuffer.data[texOffset]);
                vertex.TexCoords = glm::vec4(tex[0], 1.0f - tex[1], 0.0f, 0.0f);
            }
            else {
                vertex.TexCoords = glm::vec4(0.0f);
            }

            // tangent/bitangent 初始化为0
            vertex.Tangent = glm::vec4(0.0f);
            vertex.Bitangent = glm::vec4(0.0f);

            vertices.push_back(vertex);
        }

        // 提取索引数据
        std::vector<unsigned int> rawIndices;
        rawIndices.reserve(indicesAccessor.count);
        for (size_t i = 0; i < indicesAccessor.count; i++) {
            size_t byteOffset = indicesView.byteOffset + indicesAccessor.byteOffset + i * indexComponentSize;
            unsigned int index = 0;
            if (indicesAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                index = static_cast<unsigned int>(*reinterpret_cast<const unsigned short*>(&indicesBuffer.data[byteOffset]));
            }
            else {
                index = *reinterpret_cast<const unsigned int*>(&indicesBuffer.data[byteOffset]);
            }
            // 可选：检查 index 是否在 vertices 范围内（一般 glTF 保证正确性，这里可不做，或做日志）
            rawIndices.push_back(index);
        }

        // 转换为三角形列表
        std::vector<unsigned int> triangleIndices;
        if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
            triangleIndices = rawIndices;
        }
        else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
            triangleIndices = ConvertTriangleStripToTriangles(rawIndices);
        }
        else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_FAN) {
            triangleIndices = ConvertTriangleFanToTriangles(rawIndices);
        }

        // 处理材质
        PBRMaterial* material = nullptr;
        if (primitive.material >= 0 && primitive.material < static_cast<int>(model.materials.size())) {
            material = ProcessMaterial(primitive.material, filePath);
        }
        else if (primitive.material < 0 || primitive.material >= static_cast<int>(model.materials.size())) {
            std::cerr << "Invalid material index: " << primitive.material << std::endl;
        }

        // 创建 Mesh 并添加到 MeshComponent
        Mesh* m = new Mesh(vertices, triangleIndices, material, scaleRate);
        meshComponent->AddMesh(m);
    }

    std::cout << "Load GLTF Mesh " << meshIndex << std::endl;
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

// ProcessMaterial: 将 GLTF 材质转换为引擎的 PBRMaterial
PBRMaterial* GLTFManager::ProcessMaterial(int materialIndex, const std::string& filePath) {
    if (materialIndex < 0 || materialIndex >= static_cast<int>(model.materials.size())) {
        std::cerr << "Invalid material index: " << materialIndex << std::endl;
        return nullptr;
    }
    std::cout << "Load GLTF Material Textures " << filePath << std::endl;

    Material m_Material = default_material;
    const tinygltf::Material& m = model.materials[materialIndex];

    // 1. 基础颜色因子 (Ambient 和 Diffuse)
    if (m.pbrMetallicRoughness.baseColorFactor.size() == 4) {
        m_Material.Ambient = ConvertToVec4(m.pbrMetallicRoughness.baseColorFactor.data());
        m_Material.Diffuse = m_Material.Ambient; // 简化处理
    }

    // 2. 基础颜色贴图 (Albedo)
    if (m.pbrMetallicRoughness.baseColorTexture.index >= 0) {
        int texIndex = m.pbrMetallicRoughness.baseColorTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // 确保 texIndex 有效
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // 确保 source 有效
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.AlbedoMapIndex = g_SceneManager.AddTexture(texPath.c_str());
            }
        }
    }

    // 3. 金属度和粗糙度贴图 (MetallicRoughness)
    if (m.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
        int texIndex = m.pbrMetallicRoughness.metallicRoughnessTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // 确保 texIndex 有效
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // 确保 source 有效
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.MetallicMapIndex = g_SceneManager.AddTexture(texPath.c_str());
                m_Material.RoughnessMapIndex = g_SceneManager.AddTexture(texPath.c_str()); // GLTF 通常将金属度和粗糙度存储在同一贴图的不同通道
            }
        }
    }

    // 4. 法线贴图 (Normal)
    if (m.normalTexture.index >= 0) {
        int texIndex = m.normalTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // 确保 texIndex 有效
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // 确保 source 有效
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.NormalMapIndex = g_SceneManager.AddTexture(texPath.c_str());
            }
        }
    }

    // 5. AO贴图 (Ambient Occlusion) - 如果 GLTF 材质包含
    // 注意：标准 GLTF 不包含 AO 贴图，通常需要使用扩展或自定义方法
    // 这里假设没有 AO 贴图

    // 6. 发射因子 (Emission)
    if (m.emissiveFactor.size() == 3) {
        m_Material.Emission = ConvertToVec4(m.emissiveFactor.data());
    }

    // 7. 发射贴图 (Emission)
    if (m.emissiveTexture.index >= 0) {
        int texIndex = m.emissiveTexture.index;
        if (texIndex < static_cast<int>(model.textures.size())) { // 确保 texIndex 有效
            const tinygltf::Texture& tex = model.textures[texIndex];
            if (tex.source >= 0 && tex.source < static_cast<int>(model.images.size())) { // 确保 source 有效
                const tinygltf::Image& image = model.images[tex.source];
                std::string texPath = filePath + image.uri;
                m_Material.EmissionMapIndex = g_SceneManager.AddTexture(texPath.c_str());
            }
        }
    }

    // 8. Alpha模式和裁剪阈值 (Alpha Mode and Cutoff)
    if (m.alphaMode == "BLEND" && m.alphaCutoff > 0.0) {
        // GLTF 通常不会有单独的透明贴图，可能使用基础颜色贴图的 alpha 通道
        // 这里假设使用 Albedo 贴图的 alpha 通道作为透明度
        if (m.pbrMetallicRoughness.baseColorTexture.index >= 0) {
            m_Material.AlphaMapIndex = m_Material.AlbedoMapIndex;
        }
        m_Material.Dissolve = static_cast<float>(m.alphaCutoff); // 使用 alphaCutoff 作为透明度
    }
    else if (m.alphaMode == "MASK" && m.alphaCutoff > 0.0) {
        // 使用裁剪模式
        if (m.pbrMetallicRoughness.baseColorTexture.index >= 0) {
            m_Material.AlphaMapIndex = m_Material.AlbedoMapIndex;
        }
        m_Material.Dissolve = static_cast<float>(m.alphaCutoff); // 使用 alphaCutoff 作为透明度
    }
    else {
        // OPAQUE 模式
        m_Material.Dissolve = 1.0f;
    }

    // 9. Specular 和 OpticalDensity 等属性
    // GLTF PBR 不直接提供 Specular 属性，您可能需要自定义
    // 这里简化处理，将 Specular 设置为默认值
    m_Material.Specular = glm::vec4(1.0f); // 默认值
    m_Material.OpticalDensity = 1.0f; // 默认值

    // 10. 处理粗糙度因子
    if (m.pbrMetallicRoughness.roughnessFactor > 0.0) {
        m_Material.SpecularExponent = 1.0f / static_cast<float>(m.pbrMetallicRoughness.roughnessFactor); // 简化示例
    }

    g_MaterialList.push_back(m_Material);
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

// GetTransform: 从 GLTF 节点中提取变换矩阵
glm::mat4 GLTFManager::GetTransform(const tinygltf::Node& node) {
    glm::mat4 transform = glm::mat4(1.0f);

    if (node.matrix.size() == 16) {
        // 使用矩阵
        for (int i = 0; i < 16; ++i) {
            transform[i / 4][i % 4] = node.matrix[i];
        }
    }
    else {
        // 使用 TRS
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