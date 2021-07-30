#include "Types/Trail/VertexPool.hpp"

#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Rendering/ShadowCastingMode.hpp"

#include "Types/Trail/AltTrail.hpp"

#include "QosmeticsLogger.hpp"

#define INFO(value...) QosmeticsLogger::GetContextLogger("VertexPool").info(value)
#define ERROR(value...) QosmeticsLogger::GetContextLogger("VertexPool").error(value)

DEFINE_TYPE(Qosmetics, VertexPool);

#define LOGINT(val) INFO("%s: %d", #val, val)
using namespace UnityEngine;

namespace Qosmetics
{
    void VertexPool::ctor(Material* material, AltTrail* owner)
    {
        INVOKE_CTOR();
        vertexTotal = vertexUsed = indexUsed = indexTotal = 0;
        vertCountChanged = false;
        this->owner = owner;
        CreateMeshObj(owner, material);
        _material = material;
        InitArrays();
        IndiceChanged = ColorChanged = UVChanged = UV2Changed = VertChanged = true;
    }

    void VertexPool::RecalculateBounds()
    {
        auto mesh = get_MyMesh();
        if (mesh) mesh->RecalculateBounds();
    }

    Mesh* VertexPool::get_MyMesh()
    {
        if (_meshFilter) return _meshFilter->get_sharedMesh();
        return nullptr;
    }

    void VertexPool::SetMeshObjectActive(bool flag)
    {
        if (!_meshFilter) return;
        _meshFilter->get_gameObject()->SetActive(flag);
    }

    void VertexPool::Destroy()
    {
        if (_gameObject) UnityEngine::Object::Destroy(_gameObject);
    }

    VertexSegment VertexPool::GetVertices(int vcount, int icount)
    {
        LOGINT(vcount);
        LOGINT(vertexUsed);
        LOGINT(vertexTotal);

        LOGINT(icount);
        LOGINT(indexUsed);
        LOGINT(indexTotal);

        int vertNeed = 0;
        int indexNeed = 0;
        if ((vertexUsed + vcount) >= vertexTotal)
        {
            INFO("calculating vertneed");
            LOGINT((vcount / BlockSize));
            LOGINT((((vcount / BlockSize) + 1) * BlockSize));
            vertNeed = ((vcount / BlockSize) + 1) * BlockSize;
        }

        if ((indexUsed + icount) >= indexTotal)
        {
            INFO("calculating indexNeed");
            LOGINT((icount / BlockSize));
            LOGINT((((icount / BlockSize) + 1) * BlockSize));
            indexNeed = ((icount / BlockSize) + 1) * BlockSize;
        }

        vertexUsed += vcount;
        indexUsed += icount;

        INFO("vertNeed: %d, indexNeed: %d", vertNeed, indexNeed);
        if (vertNeed != 0 || indexNeed != 0)
        {
            EnlargeArrays(vertNeed, indexNeed);
            vertexTotal += vertNeed;
            indexTotal += indexNeed;
        }
        return VertexSegment(vertexUsed - vcount, vcount, indexUsed - icount, icount, this);
    }

    void VertexPool::EnlargeArrays(int count, int icount)
    {
        INFO("Enlarging Arrays...");
        int length = Vertices->Length();
        auto tempVertices = Vertices;
        Vertices = reinterpret_cast<Array<Vector3>*>(il2cpp_functions::array_new(classof(Vector3), length + count));//Array<Vector3>::NewLength(length + count);
        for (auto i = 0; i < length; i++) Vertices->values[i] = tempVertices->values[i];
        tempVertices->CopyTo(Vertices, 0);

        INFO("Vertices: %p", Vertices);
        INFO("Length: %lu", Vertices->Length());


        length = UVs->Length();
        auto tempUVs = UVs;
        UVs = reinterpret_cast<Array<Vector2>*>(il2cpp_functions::array_new(classof(Vector2), length + count));//Array<Vector2>::NewLength(length + count);
        for (auto i = 0; i < length; i++) UVs->values[i] = tempUVs->values[i];
        //tempUVs->CopyTo(UVs, 0);

        length = Colors->Length();
        auto tempColors = Colors;
        Colors = reinterpret_cast<Array<Color>*>(il2cpp_functions::array_new(classof(Color), length + count));//Array<Color>::NewLength(length + count);
        for (auto i = 0; i < length; i++) Colors->values[i] = tempColors->values[i];
        //tempColors->CopyTo(Colors, 0);

        length = Indices->Length();
        auto tempIndices = Indices;
        Indices = reinterpret_cast<Array<int>*>(il2cpp_functions::array_new(classof(int), length + icount));//Array<int>::NewLength(length + icount);
        for (auto i = 0; i < length; i++) Indices->values[i] = tempIndices->values[i];
        //tempIndices->CopyTo(Indices, 0);
        
        vertCountChanged = true;
        IndiceChanged = true;
        ColorChanged = true;
        UVChanged = true;
        VertChanged = true;
        UV2Changed = true;
        INFO("Enlarged Arrays!");
    }

    void VertexPool::LateUpdate()
    {
        auto mymesh = get_MyMesh();
        if (!mymesh) return;

        //if (vertCountChanged) mymesh->Clear();

        INFO("Mesh: %p", mymesh);
        INFO("Vertices: %p", Vertices);
        INFO("Bounds: %p", Vertices->bounds);
        INFO("Length: %lu", Vertices->bounds ? Vertices->bounds->length : Vertices->max_length);

        mymesh->set_vertices(Vertices);
        if (UVChanged) mymesh->set_uv(UVs);
        if (ColorChanged) mymesh->set_colors(Colors);
        if (IndiceChanged) mymesh->set_triangles(Indices);

        ElapsedTime += Time::get_deltaTime();

        if (ElapsedTime > BoundsScheduleTime || FirstUpdate)
        {
            RecalculateBounds();
            ElapsedTime = 0.0f;
        }

        if (ElapsedTime > BoundsScheduleTime)
            FirstUpdate = false;
        
        vertCountChanged = false;
        IndiceChanged = false;
        ColorChanged = false;
        UVChanged = false;
        UV2Changed = false;
        VertChanged = false;
    }

    void VertexPool::CreateMeshObj(AltTrail* owner, Material* material)
    {
        static Il2CppString* saberTrailName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("SaberTrail");
        _gameObject = GameObject::New_ctor(saberTrailName);
        _gameObject->set_layer(owner->get_gameObject()->get_layer());
        _meshFilter = _gameObject->AddComponent<MeshFilter*>();
        auto meshrenderer = _gameObject->AddComponent<MeshRenderer*>();

        _gameObject->get_transform()->set_position(Vector3::get_zero());
        _gameObject->get_transform()->set_rotation(Quaternion::get_identity());
        
        meshrenderer->set_shadowCastingMode(UnityEngine::Rendering::ShadowCastingMode::Off);
        meshrenderer->set_receiveShadows(false);
        meshrenderer->set_sharedMaterial(material);
        //meshrenderer->set_sortingLayerName(this->owner->SortingLayerName);
        meshrenderer->set_sortingOrder(this->owner->SortingOrder);
        _meshFilter->set_sharedMesh(Mesh::New_ctor());
    }

    void VertexPool::InitArrays()
    {
        Vertices = reinterpret_cast<Array<Vector3>*>(il2cpp_functions::array_new(classof(Vector3), 4));//Array<Vector3>::NewLength(4);
        UVs = reinterpret_cast<Array<Vector2>*>(il2cpp_functions::array_new(classof(Vector2), 4));//Array<Vector2>::NewLength(4);
        Colors = reinterpret_cast<Array<Color>*>(il2cpp_functions::array_new(classof(Color), 4));//Array<Color>::NewLength(4);
        Indices = reinterpret_cast<Array<int>*>(il2cpp_functions::array_new(classof(int), 6));//Array<int>::NewLength(6);
        vertexTotal = 4;
        indexTotal = 6;
    }
}