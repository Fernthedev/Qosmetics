#pragma once

#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Color.hpp"
#include <string>
#include "HMUI/TitleViewController.hpp"
#include "HMUI/ViewController.hpp"
#include "Zenject/DiContainer.hpp"

class UIUtils
{
    public:
        static TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title);
        static TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title, UnityEngine::Color color);
        static TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title, UnityEngine::Color leftColor, UnityEngine::Color rightColor);
        static void SetTitleColor(HMUI::TitleViewController* titleView, UnityEngine::Color color, bool buttonanim = false);
        static void SetupViewController(HMUI::ViewController* vc);
        static void AddViewComponents(UnityEngine::GameObject* go, Zenject::DiContainer* container);

};