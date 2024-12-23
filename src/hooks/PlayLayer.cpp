#include "PlayLayer.hpp"
#include "Geode/binding/StartPosObject.hpp"
#include "UILayer.hpp"

using namespace geode::prelude;

void HookPlayLayer::addObject(GameObject* obj) {
    if (obj->m_objectID == 31) {
        #ifdef GEODE_IS_WINDOWS
            if(static_cast<StartPosObject*>(obj)->m_startSettings->m_disableStartPos && Mod::get()->getSettingValue<bool>("ignoreDisabled")) {
                PlayLayer::addObject(obj);
                return;
            }
        #endif
        m_fields->m_startPosObjects.push_back(obj);
    }
    PlayLayer::addObject(obj);
}

void HookPlayLayer::updateStartPos(int idx) {
    if (Mod::get()->getSettingValue<bool>("onlyPractice") && !m_isPracticeMode) return;

    if(idx < 0) idx = m_fields->m_startPosObjects.size();
    if(idx > m_fields->m_startPosObjects.size()) idx = 0;

    if(idx == 0) {
        m_isTestMode = false;
        updateTestModeLabel();
    } else {
        m_isTestMode = true;
        updateTestModeLabel();
    }

    m_currentCheckpoint = nullptr;
    m_fields->m_startPosIdx = idx;

    GameObject* object = nullptr;

    if(true) {
        object = idx > 0 ? m_fields->m_startPosObjects[idx - 1] : nullptr;
    } else {
        auto rand = std::rand() % m_fields->m_startPosObjects.size() + 1;
        object = idx > 0 ? m_fields->m_startPosObjects[rand] : nullptr;
    }
    setStartPosObject(static_cast<StartPosObject*>(object));

    if(m_isPracticeMode)
        resetLevelFromStart();

    resetLevel();
    startMusic();

    static_cast<HookUILayer*>(m_uiLayer)->updateUI();
}

void HookPlayLayer::createObjectsFromSetupFinished() {
    PlayLayer::createObjectsFromSetupFinished();

    std::sort(m_fields->m_startPosObjects.begin(), m_fields->m_startPosObjects.end(), [](auto* a, auto* b) { return a->getPositionX() < b->getPositionX(); });

    if(this->m_startPosObject) {
        auto currentIdx = find(m_fields->m_startPosObjects.begin(), m_fields->m_startPosObjects.end(), this->m_startPosObject) - m_fields->m_startPosObjects.begin();
        m_fields->m_startPosIdx = currentIdx + 1;
    }

    static_cast<HookUILayer*>(m_uiLayer)->updateUI();
}

void HookPlayLayer::togglePracticeMode(bool practiceMode) {
    if (Mod::get()->getSettingValue<bool>("onlyPractice") && !practiceMode) {
        updateStartPos(0);
    } else {
        static_cast<HookUILayer*>(m_uiLayer)->updateUI();
    }
    PlayLayer::togglePracticeMode(practiceMode);
}
