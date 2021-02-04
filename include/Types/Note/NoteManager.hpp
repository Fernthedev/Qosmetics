#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "Types/Note/NoteItem.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics, NoteManager, UnityEngine::MonoBehaviour,     
    DECLARE_METHOD(UnityEngine::GameObject*, GetActivePrefab);
    DECLARE_METHOD(void, SetActiveModel, Il2CppString* csname);
    DECLARE_METHOD(void, SetDefault);
    //DECLARE_OVERRIDE_METHOD(UnityEngine::GameObject*, GetActivePrefab, il2cpp_utils::FindMethodUnsafe("Qosmetics", "ModelManager", "GetActivePrefab", 0));
    //DECLARE_OVERRIDE_METHOD(void, SetActiveModel, il2cpp_utils::FindMethodUnsafe("Qosmetics", "ModelManager", "SetActiveModel", 1), Il2CppString* csname);
    //DECLARE_OVERRIDE_METHOD(void, SetDefault, il2cpp_utils::FindMethodUnsafe("Qosmetics", "ModelManager", "SetDefault", 0));

    DECLARE_METHOD(UnityEngine::GameObject*, get_leftArrow);    
    DECLARE_METHOD(UnityEngine::GameObject*, get_rightArrow);    
    DECLARE_METHOD(UnityEngine::GameObject*, get_leftDot);
    DECLARE_METHOD(UnityEngine::GameObject*, get_rightDot);
    DECLARE_METHOD(UnityEngine::GameObject*, get_leftDebris);
    DECLARE_METHOD(UnityEngine::GameObject*, get_rightDebris);
    DECLARE_METHOD(UnityEngine::GameObject*, get_bomb);
    
    DECLARE_METHOD(Il2CppString*, get_leftArrowName);    
    DECLARE_METHOD(Il2CppString*, get_rightArrowName);    
    DECLARE_METHOD(Il2CppString*, get_leftDotName);
    DECLARE_METHOD(Il2CppString*, get_rightDotName);
    DECLARE_METHOD(Il2CppString*, get_leftDebrisName);
    DECLARE_METHOD(Il2CppString*, get_rightDebrisName);
    DECLARE_METHOD(Il2CppString*, get_bombName);

    public:
        void internalSetActiveModel(std::string name, bool load = false);
        void SetActiveNote(std::string name);
        ItemType get_type();
        NoteItem& get_item();
        
    protected:
        NoteItem* activeItem;
    REGISTER_FUNCTION(NoteManager,
        REGISTER_METHOD(SetActiveModel);
        REGISTER_METHOD(GetActivePrefab);
        REGISTER_METHOD(SetDefault);

        REGISTER_METHOD(get_leftArrow);    
        REGISTER_METHOD(get_rightArrow);    
        REGISTER_METHOD(get_leftDot);
        REGISTER_METHOD(get_rightDot);
        REGISTER_METHOD(get_leftDebris);
        REGISTER_METHOD(get_rightDebris);
        REGISTER_METHOD(get_bomb);

        REGISTER_METHOD(get_leftArrowName);    
        REGISTER_METHOD(get_rightArrowName);    
        REGISTER_METHOD(get_leftDotName);
        REGISTER_METHOD(get_rightDotName);
        REGISTER_METHOD(get_leftDebrisName);
        REGISTER_METHOD(get_rightDebrisName);
        REGISTER_METHOD(get_bombName);
    )
)