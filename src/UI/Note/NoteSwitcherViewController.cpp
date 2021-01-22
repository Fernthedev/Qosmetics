#include "UI/Note/NoteSwitcherViewController.hpp"
#include "config.hpp"
#include "Config/NoteConfig.hpp"
#include "Data/Descriptor.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Toggle_ToggleEvent.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"
#include "HMUI/ScrollView.hpp"
#include "HMUI/ModalView.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/InputFieldView.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "Logging/UILogger.hpp"
#include "Qosmetic/QuestNote.hpp"
#include "Utils/FileUtils.hpp"
#include "Data/QosmeticsDescriptorCache.hpp"
#include "Data/CreatorCache.hpp"

#include "UI/Note/NotePreviewViewController.hpp"

using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace UnityEngine::Events;
using namespace HMUI;

DEFINE_CLASS(Qosmetics::NoteSwitcherViewController);

#define INFO(value...) UILogger::GetLogger().WithContext("Note Switching").info(value)
#define ERROR(value...) UILogger::GetLogger().WithContext("Note Switching").error(value)

namespace Qosmetics
{
    void NoteSwitcherViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        DescriptorCache::Write();
        SaveConfig();
        QuestNote::SelectionDefinitive();
    }

    void NoteSwitcherViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            get_gameObject()->AddComponent<Touchable*>();
            GameObject* settingsLayout = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(get_transform());

            ExternalComponents* externalComponents = settingsLayout->GetComponent<ExternalComponents*>();
            RectTransform* scrollTransform = externalComponents->Get<RectTransform*>();
            scrollTransform->set_sizeDelta(UnityEngine::Vector2(0.0f, 0.0f));
            
            Button* defaultButton = QuestUI::BeatSaberUI::CreateUIButton(settingsLayout->get_transform(), "default bloqs", il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction*>(classof(UnityEngine::Events::UnityAction*), il2cpp_utils::createcsstr("", il2cpp_utils::StringType::Manual), +[](Il2CppString* fileName, Button* button){
                INFO("Default note selected!");
                if (QuestNote::GetActiveNote() && QuestNote::GetActiveNote()->get_isLoading()) return;
                QuestNote::SetActiveNote((NoteData*)nullptr);
                NotePreviewViewController* previewController = Object::FindObjectOfType<NotePreviewViewController*>();//
                if (previewController) previewController->UpdatePreview();
                else ERROR("Couldn't find preview controller");
            }));

            HorizontalLayoutGroup* selectionLayout = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(settingsLayout->get_transform());
            VerticalLayoutGroup* infoLayout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(selectionLayout->get_transform());
            VerticalLayoutGroup* buttonList = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(selectionLayout->get_transform());

            std::vector<Descriptor*>& descriptors = DescriptorCache::GetNoteDescriptors();
            for (int i = 0; i < descriptors.size(); i++)
            {
                HorizontalLayoutGroup* buttonLayout = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(buttonList->get_transform());

                AddButtonsForDescriptor(buttonLayout->get_transform(), descriptors[i]);
                AddTextForDescriptor(infoLayout->get_transform(), descriptors[i]);
            }
        }
    }

    void NoteSwitcherViewController::AddButtonsForDescriptor(Transform* layout, Descriptor* descriptor)
    {
        if (!layout || !descriptor) return;

        std::string stringName = descriptor->get_fileName();

        //layout->get_gameObject()->AddComponent<Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));

        Button* selectButton = QuestUI::BeatSaberUI::CreateUIButton(layout, "select", il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction*>(classof(UnityEngine::Events::UnityAction*), il2cpp_utils::createcsstr(stringName, il2cpp_utils::StringType::Manual), +[](Il2CppString* fileName, Button* button){
            if (!fileName) return;
            if (QuestNote::GetActiveNote() && QuestNote::GetActiveNote()->get_isLoading()) return;
            std::string name = to_utf8(csstrtostr(fileName));
            Descriptor* descriptor = DescriptorCache::GetDescriptor(name, note);
            QuestNote::SetActiveNote(descriptor, true);
            NotePreviewViewController* previewController = Object::FindObjectOfType<NotePreviewViewController*>();//
            if (previewController) previewController->UpdatePreview();
            else INFO("Couldn't find preview controller");
            DescriptorCache::Write();
            INFO("Selected note %s", descriptor->get_name().c_str());
        }));

        selectButton->get_gameObject()->set_name(il2cpp_utils::createcsstr(stringName));
        Button* eraseButton = QuestUI::BeatSaberUI::CreateUIButton(layout, "delete", il2cpp_utils::MakeDelegate<UnityEngine::Events::UnityAction*>(classof(UnityEngine::Events::UnityAction*), il2cpp_utils::createcsstr(stringName, il2cpp_utils::StringType::Manual), +[](Il2CppString* fileName, Button* button){
            if (!fileName) return;
            std::string name = to_utf8(csstrtostr(fileName));
            Descriptor* descriptor = DescriptorCache::GetDescriptor(name, note);
            if (fileexists(descriptor->get_filePath())) 
            {
                INFO("Deleting %s", descriptor->get_filePath().c_str());
                deletefile(descriptor->get_filePath());
            }
        }));


    }

    extern bool shouldRainbow(UnityEngine::Color color);

    void NoteSwitcherViewController::AddTextForDescriptor(Transform* layout, Descriptor* descriptor)
    {
        if (!layout || !descriptor) return; // if either is nullptr, early return
        
        //layout->get_gameObject()->AddComponent<Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));

        std::string buttonName = descriptor->get_name();
        
        if (buttonName == "") // if the name is empty, use the filename instead
        {
            buttonName = descriptor->get_fileName();
            if (buttonName != "" && buttonName.find(".") != std::string::npos) buttonName.erase(buttonName.find_last_of("."));
        }

        if (buttonName.find("rainbow") != std::string::npos || buttonName.find("Rainbow") != std::string::npos) buttonName = FileUtils::rainbowIfy(buttonName);

        std::string authorName = descriptor->get_author();

        if (authorName == "")
        {
            authorName = "---";
        }

        UnityEngine::Color textColor = CreatorCache::GetCreatorColor(authorName);

        if (shouldRainbow(textColor))
            authorName = FileUtils::rainbowIfy(authorName);

        TMPro::TextMeshProUGUI* name = QuestUI::BeatSaberUI::CreateText(layout, buttonName);
        TMPro::TextMeshProUGUI* authorText = QuestUI::BeatSaberUI::CreateText(layout, authorName);

        QuestUI::BeatSaberUI::AddHoverHint(name->get_gameObject(), descriptor->get_description());
        authorText->set_color(textColor);
        authorText->set_fontSize(authorText->get_fontSize() * 0.5f);
    }
}