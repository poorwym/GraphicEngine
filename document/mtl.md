��Ȼ�������ҽ���ϸ���� TinyObjLoader �� `material_t` �ṹ��ĸ����ֶ����Ƽ��京�塣`material_t` ���ڴ洢�� MTL �ļ��н��������Ĳ������ԡ���Щ���Զ�����ģ�ͱ�����Ӿ����ԣ�����ɫ�������ʡ�������ͼ�ȡ�

## `material_t` �ṹ���ֶ����

������ `material_t` �ṹ�����Ҫ�ֶμ����Ӧ�� MTL �ļ����

| �ֶ�����          | ��������       | ��Ӧ MTL ����      | ����                                                         |
|-------------------|----------------|--------------------|--------------------------------------------------------------|
| `name`            | `std::string`  | `newmtl`            | ���ʵ����ƣ������� OBJ �ļ������øò��ʡ�                   |
| `ambient`         | `float[3]`     | `Ka`                | �����ⷴ���ʣ�Ambient Color������ʾ�����ڻ������µ���ɫ��     |
| `diffuse`         | `float[3]`     | `Kd`                | �����䷴���ʣ�Diffuse Color������ʾ���ʵĻ�����ɫ��           |
| `specular`        | `float[3]`     | `Ks`                | ���淴�䷴���ʣ�Specular Color������ʾ���ʵĸ߹���ɫ��         |
| `transmittance`   | `float[3]`     | `Kt`                | ͸���ʣ�Transmittance������ʾ���ʵ�͸���ȡ�                   |
| `emission`        | `float[3]`     | `Ke`                | �Է�����ɫ��Emission Color������ʾ�������������ɫ��         |
| `shininess`       | `float`        | `Ns`                | ���淴��ָ����Specular Exponent�������Ƹ߹�Ĵ�С����ȡ�       |
| `ior`             | `float`        | `Ni`                | �����ʣ�Index of Refraction��������͸�����ʵĹ�ѧ���ԡ�         |
| `d`               | `float`        | `d` �� `Tr`         | ͸���ȣ�Dissolve������ʾ���ʵ�͸���ȡ�ֵ��Χͨ��Ϊ 0.0����ȫ͸������ 1.0����ȫ��͸������ |
| `illum`           | `int`          | `illum`             | ����ģ�ͣ�Illumination Model�����������������Դ������         |
| `map_Ka`          | `std::string`  | `map_Ka`            | ��������ͼ��Ambient Texture Map�����ļ�·����                  |
| `map_Kd`          | `std::string`  | `map_Kd`            | ��������ͼ��Diffuse Texture Map�����ļ�·����                  |
| `map_Ks`          | `std::string`  | `map_Ks`            | ���淴����ͼ��Specular Texture Map�����ļ�·����                |
| `map_Ns`          | `std::string`  | `map_Ns`            | ���淴��ָ����ͼ��Specular Exponent Texture Map�����ļ�·����    |
| `map_d`           | `std::string`  | `map_d`             | ͸������ͼ��Dissolve Texture Map�����ļ�·����                  |
| `map_bump`        | `std::string`  | `map_bump` �� `bump` | ������ͼ��Normal Map�����ļ�·��������ģ�����ϸ�ڡ�            |
| `map_disp`        | `std::string`  | `disp`              | λ����ͼ��Displacement Map�����ļ�·�������ڼ���λ�ơ�            |
| `map_decal`       | `std::string`  | `decal`             | ������ͼ��Decal Texture Map�����ļ�·���������ڲ��������ϸ�ڡ�    |
| `map_refl`        | `std::string`  | `map_refl`          | ������ͼ��Reflection Texture Map�����ļ�·��������ģ�ⷴ��Ч����  |

### �ֶ���ϸ˵��

1. **`name` (`std::string`)**
   - **��Ӧ���** `newmtl`
   - **������** ������ʵ����ƣ����� OBJ �ļ������á����磺
     ```mtl
     newmtl Material1
     ```

2. **`ambient` (`float[3]`)**
   - **��Ӧ���** `Ka`
   - **������** �����ⷴ���ʣ�ָ�������ڻ������µ���ɫ��ͨ���� RGB ������������ʾ��
     ```mtl
     Ka 1.000 1.000 1.000
     ```

3. **`diffuse` (`float[3]`)**
   - **��Ӧ���** `Kd`
   - **������** �����䷴���ʣ�ָ�����ʵĻ�����ɫ��
     ```mtl
     Kd 0.800 0.800 0.800
     ```

4. **`specular` (`float[3]`)**
   - **��Ӧ���** `Ks`
   - **������** ���淴�䷴���ʣ�ָ�����ʵĸ߹���ɫ��
     ```mtl
     Ks 0.500 0.500 0.500
     ```

5. **`transmittance` (`float[3]`)**
   - **��Ӧ���** `Kt`
   - **������** ͸���ʣ�ָ�����ʵ�͸������ɫ������͸�����ʡ�
     ```mtl
     Kt 0.000 0.000 0.000
     ```

6. **`emission` (`float[3]`)**
   - **��Ӧ���** `Ke`
   - **������** �Է�����ɫ��ָ���������������ɫ��
     ```mtl
     Ke 0.000 0.000 0.000
     ```

7. **`shininess` (`float`)**
   - **��Ӧ���** `Ns`
   - **������** ���淴��ָ�������Ƹ߹�Ĵ�С����ȡ�ֵԽ�󣬸߹�ԽС�Ҹ�������
     ```mtl
     Ns 32.0
     ```

8. **`ior` (`float`)**
   - **��Ӧ���** `Ni`
   - **������** �����ʣ�����͸�����ʵĹ�ѧ���ԡ������ڲ�����ˮ�Ȳ��ʡ�
     ```mtl
     Ni 1.5
     ```

9. **`d` (`float`)**
   - **��Ӧ���** `d` �� `Tr`
   - **������** ͸���ȣ���ʾ���ʵ�͸���̶ȡ�`d` ��ʾ�ܽ�ȣ�Dissolve����`Tr` ��ʾ��ת�ܽ�ȣ�Transparency����ֵ��Χͨ��Ϊ 0.0����ȫ͸������ 1.0����ȫ��͸������
     ```mtl
     d 1.0
     ```

10. **`illum` (`int`)**
    - **��Ӧ���** `illum`
    - **������** ����ģ�ͣ��������������Դ������������ֵ������
      - `0`���޹��ա�
      - `1`���������⡣
      - `2`�������� + ������ + ���淴�䡣
      - `3`�������� + ������ + ���淴�� + �߹⡣
      - `...`�����ิ�ӵĹ���ģ�͡�
      ```mtl
      illum 2
      ```

11. **`map_Ka` (`std::string`)**
    - **��Ӧ���** `map_Ka`
    - **������** ��������ͼ���ļ�·����������ǿ���ʵĻ�����Ч����
      ```mtl
      map_Ka ambient1.png
      ```

12. **`map_Kd` (`std::string`)**
    - **��Ӧ���** `map_Kd`
    - **������** ��������ͼ���ļ�·����ָ�����ʵĻ�����ɫ����
      ```mtl
      map_Kd diffuse1.png
      ```

13. **`map_Ks` (`std::string`)**
    - **��Ӧ���** `map_Ks`
    - **������** ���淴����ͼ���ļ�·����ָ�����ʵĸ߹�����
      ```mtl
      map_Ks specular1.png
      ```

14. **`map_Ns` (`std::string`)**
    - **��Ӧ���** `map_Ns`
    - **������** ���淴��ָ����ͼ���ļ�·�������ڿ��Ʋ�ͬ����ĸ߹�ǿ�ȡ�
      ```mtl
      map_Ns specularExponent1.png
      ```

15. **`map_d` (`std::string`)**
    - **��Ӧ���** `map_d`
    - **������** ͸������ͼ���ļ�·�������ڿ��Ʋ��ʵ�͸���ȱ仯��
      ```mtl
      map_d opacity1.png
      ```

16. **`map_bump` (`std::string`)**
    - **��Ӧ���** `map_bump` �� `bump`
    - **������** ������ͼ���ļ�·��������ģ�����ϸ�ںͰ�͹Ч����
      ```mtl
      map_bump normal1.png
      ```
      ��
      ```mtl
      bump normal1.png
      ```

17. **`map_disp` (`std::string`)**
    - **��Ӧ���** `disp`
    - **������** λ����ͼ���ļ�·�������ڼ���λ��Ч����
      ```mtl
      disp displacement1.png
      ```

18. **`map_decal` (`std::string`)**
    - **��Ӧ���** `decal`
    - **������** ������ͼ���ļ�·���������ڲ��ʱ������ϸ�ڡ�
      ```mtl
      decal decal1.png
      ```

19. **`map_refl` (`std::string`)**
    - **��Ӧ���** `map_refl`
    - **������** ������ͼ���ļ�·��������ģ����ʵķ���Ч����
      ```mtl
      map_refl reflection1.png
      ```

## ʾ�������� `material_t` �ֶ�

������һ�� MTL �ļ��������£�

```mtl
# ����1
newmtl Material1
Ka 1.000 1.000 1.000
Kd 0.800 0.800 0.800
Ks 0.500 0.500 0.500
Ns 32.0
d 1.0
illum 2
map_Kd diffuse1.png
map_Ks specular1.png
map_bump normal1.png

# ����2
newmtl Material2
Ka 0.000 0.000 0.000
Kd 0.600 0.600 0.600
Ks 0.900 0.900 0.900
Ns 64.0
d 0.8
illum 3
map_Kd diffuse2.png
map_Ks specular2.png
```

ʹ�� TinyObjLoader �����󣬶�Ӧ�� `material_t` �ṹ�彫�����������ݣ�

### Material1

- `name`��`"Material1"`
- `ambient`��[1.0, 1.0, 1.0]
- `diffuse`��[0.8, 0.8, 0.8]
- `specular`��[0.5, 0.5, 0.5]
- `shininess`��32.0
- `dissolve`��1.0
- `illum`��2
- `map_Kd`��`"diffuse1.png"`
- `map_Ks`��`"specular1.png"`
- `map_bump`��`"normal1.png"`

### Material2

- `name`��`"Material2"`
- `ambient`��[0.0, 0.0, 0.0]
- `diffuse`��[0.6, 0.6, 0.6]
- `specular`��[0.9, 0.9, 0.9]
- `shininess`��64.0
- `dissolve`��0.8
- `illum`��3
- `map_Kd`��`"diffuse2.png"`
- `map_Ks`��`"specular2.png"`
- `map_bump`��`""`��δ���壩

## ����ڴ�����ʹ�� `material_t`

������һ��ʾ�����룬չʾ��α���������� `materials`������ÿ�� `material_t` ���ֶ�ӳ�䵽�Զ���� `PBRMaterial` ���У�

```cpp
#include "tiny_obj_loader.h"
#include "PBRMaterial.h"
#include "Texture.h"
#include <vector>
#include <string>
#include <iostream>

// ������һ�� PBRMaterial �����ڴ洢��������
PBRMaterial* ConvertToPBRMaterial(const tinyobj::material_t& mat, const std::string& base_dir) {
    PBRMaterial* pbrMat = new PBRMaterial();

    // ��������ɫ
    pbrMat->SetAmbientColor(glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]));

    // ��������ɫ
    pbrMat->SetDiffuseColor(glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]));

    // ���淴����ɫ
    pbrMat->SetSpecularColor(glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]));

    // ����ָ��
    pbrMat->SetSpecularExponent(mat.shininess);

    // ͸����
    pbrMat->SetOpacity(mat.dissolve);

    // ��ͼ����
    if (!mat.diffuse_texname.empty()) {
        std::string texPath = base_dir + mat.diffuse_texname;
        Texture* diffuseMap = new Texture(texPath);
        pbrMat->SetAlbedoMap(diffuseMap);
    }

    if (!mat.specular_texname.empty()) {
        std::string texPath = base_dir + mat.specular_texname;
        Texture* specularMap = new Texture(texPath);
        pbrMat->SetSpecularMap(specularMap);
    }

    if (!mat.bump_texname.empty()) {
        std::string texPath = base_dir + mat.bump_texname;
        Texture* normalMap = new Texture(texPath);
        pbrMat->SetNormalMap(normalMap);
    }

    // ������Ҫ����������ͼ...

    return pbrMat;
}

int main() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::string inputfile = "path/to/model.obj";
    std::string base_dir = "path/to/"; // MTL ����ͼ����Ŀ¼

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), base_dir.c_str(), true);

    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "ERR: " << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load/parse .obj.\n";
        return 1;
    }

    // ת�����в���
    std::vector<PBRMaterial*> pbrMaterials;
    for (const auto& mat : materials) {
        PBRMaterial* pbrMat = ConvertToPBRMaterial(mat, base_dir);
        pbrMaterials.push_back(pbrMat);
    }

    // ��������ģ������...

    // ��󣬼ǵ��ͷŲ�����Դ
    for (auto mat : pbrMaterials) {
        delete mat;
    }

    return 0;
}
```

## �ܽ�

`material_t` �ṹ������˴� MTL �ļ��н������������в������ԣ�ÿ���ֶζ�Ӧ MTL �ļ��е��ض������Щ���Զ����˲��ʵ��Ӿ����ԣ�����ɫ�������ʡ�͸�����Լ�����������ͼ����ʹ�� TinyObjLoader ���� OBJ �� MTL �ļ�ʱ����Ⲣ��ȷ������Щ�ֶζ���ʵ����ʵ����Ⱦ������Ҫ��

### �ؼ���عˣ�

1. **�ֶζ�Ӧ��ϵ��** ÿ�� `material_t` �ֶζ�Ӧ MTL �ļ��е��ض����ȷ���ڽ���ʱ��ȷӳ�䡣
2. **��ͼ·������** �������·����·��ƴ�ӣ�ȷ����ͼ�ļ���ȷ���ء�
3. **������** �ڼ��ز��ʺ���ͼʱ����ʱ��׽��������󣬱�����Ⱦ�쳣��
4. **��Դ����** ʹ������ָ���������Դ������ԣ������ڴ�й©����Դ�ظ����ء�

�����ʵ�ֹ��������������������Ҫ��һ���İ���������ʱ���ʣ�