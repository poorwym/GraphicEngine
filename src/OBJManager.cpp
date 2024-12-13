#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif
#include "OBJManager.h"
#include "SceneManager.h"

OBJManager::OBJManager()
{
}

OBJManager::~OBJManager()
{
}

extern SceneManager g_SceneManager;
extern std::vector<Material> g_MaterialList;

MeshComponent* OBJManager::Load(const std::string& filePath, const std::string fileName, float scaleRate)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, (filePath + fileName).c_str(), filePath.c_str(), true)) {
        std::cerr << "TinyOBJLoader Error: " << err << std::endl;
        return nullptr;
    }
    std::cout << "shapes = " << shapes.size() << std::endl;
    std::cout << "materials= " << materials.size() << std::endl;

    // 加载所有材质
    std::vector<PBRMaterial*> loadedMaterials;
    for (const auto& mat : materials) {
        PBRMaterial* pbrMat = ProcessMaterial(filePath, mat);
        loadedMaterials.push_back(pbrMat);
    }

    std::map<int, std::vector<Vertex>> materialVerticesMap;
    std::map<int, std::vector<unsigned int>> materialIndicesMap;

    // 遍历每个形状
    for (const auto& shape : shapes) {
        size_t indexOffset = 0;
        // 遍历每个面
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];

            int materialID = shape.mesh.material_ids[f];
            if (materialID < 0 || materialID >= materials.size()) {
                materialID = 0; // 使用默认材质
            }

            // 确保映射中存在该材质的条目
            if (materialVerticesMap.find(materialID) == materialVerticesMap.end()) {
                materialVerticesMap[materialID] = std::vector<Vertex>();
                materialIndicesMap[materialID] = std::vector<unsigned int>();
            }

            // 遍历面中的每个顶点
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
                Vertex vertex = {};
                vertex.Position = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2],
                    1.0f
                };
                if (idx.normal_index >= 0) {
                    vertex.Normal = {
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2],
                        1.0f
                    };
                }
                if (idx.texcoord_index >= 0) {
                    vertex.TexCoords = {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1],
                        0.0f,
                        0.0f
                    };
                }

                // 添加顶点并更新索引
                materialVerticesMap[materialID].push_back(vertex);
                materialIndicesMap[materialID].push_back(materialVerticesMap[materialID].size() - 1);
            }
            indexOffset += fv;
        }
    }

    // 创建 MeshComponent 并添加合并后的 Mesh
    MeshComponent* meshComponent = new MeshComponent();
    for (const auto& pair : materialVerticesMap) {
        int materialID = pair.first;
        std::vector<Vertex>& vertices = materialVerticesMap[materialID];
        std::vector<unsigned int>& indices = materialIndicesMap[materialID];

        Mesh* mesh = new Mesh(vertices, indices, loadedMaterials[materialID], scaleRate);
        meshComponent->AddMesh(mesh);
    }

    return meshComponent;
}

PBRMaterial* OBJManager::ProcessMaterial(const std::string& filePath, const tinyobj::material_t& m)
{
    Material m_Material = default_material;
    std::cout << "Load Texture " << filePath << std::endl;
    if (m.ambient) m_Material.Ambient = glm::vec4(m.ambient[0], m.ambient[1], m.ambient[2], 0.0f);
    if (m.diffuse) m_Material.Diffuse = glm::vec4(m.diffuse[0], m.diffuse[1], m.diffuse[2], 0.0f);
    if (m.specular) m_Material.Specular = glm::vec4(m.specular[0], m.specular[1], m.specular[2], 0.0f);
    if (m.illum) m_Material.Illum = m.illum;
    if (m.dissolve) m_Material.Dissolve = m.dissolve;
    if (m.emission) m_Material.Emission = glm::vec4(m.emission[0], m.emission[1], m.emission[2], 0.0f);
    if (m.shininess) m_Material.SpecularExponent = m.shininess;
    if (m.ior) m_Material.OpticalDensity = m.ior;

    if (m.diffuse_texname.size() > 0) { // Albedo
        std::string Path = filePath + m.diffuse_texname;
        m_Material.AlbedoMapIndex = g_SceneManager.AddTexture(Path.c_str());
    }
    if (m.bump_texname.size() > 0) { // HeightMap
        std::string Path = filePath + m.bump_texname;
        m_Material.HeightMapIndex = g_SceneManager.AddTexture(Path.c_str());
        m_Material.BumpMutiplier = 0.05f;
        //m_Material.HeightMap = g_TextureManager.AddTexture(new CPUTexture(Path));
    }
    if (m.emissive_texname.size()) { // Emission
        std::string Path = filePath + m.emissive_texname;
        m_Material.EmissionMapIndex = g_SceneManager.AddTexture(Path.c_str());
    }
    if (m.roughness_texname.size()) { // Roughness
        std::string Path = filePath + m.roughness_texname;
        m_Material.RoughnessMapIndex = g_SceneManager.AddTexture(Path.c_str());
    }
    if (m.metallic_texname.size()) { // Metallic
        std::string Path = filePath + m.metallic_texname;
        m_Material.MetallicMapIndex = g_SceneManager.AddTexture(Path.c_str());
    }
    if (m.normal_texname.size()) { // Normal
        std::string Path = filePath + m.normal_texname;
        m_Material.NormalMapIndex = g_SceneManager.AddTexture(Path.c_str());
    }
    if (m.alpha_texname.size()) { // Alpha
        std::string Path = filePath + m.alpha_texname;
        m_Material.AlphaMapIndex = g_SceneManager.AddTexture(Path.c_str());
    }
    g_MaterialList.push_back(m_Material);
    return new PBRMaterial(m_Material);
}
