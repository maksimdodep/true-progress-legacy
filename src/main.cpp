#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool hasStartPos = false;
        float startX = 0.0f;
    };

    bool init(GJGameLevel* level, bool usePracticeMode, bool isPlaytest) {
        if (!PlayLayer::init(level, usePracticeMode, isPlaytest)) return false;

        m_fields->hasStartPos = false;
        m_fields->startX = 0.0f;

        // Железобетонный поиск стартпоза через Cocos2d-x массив объектов
        // В GD все объекты уровня гарантированно лежат в m_objects
        if (m_objects) {
            for (int i = 0; i < m_objects->count(); ++i) {
                // Приводим объект к базовому игровому классу GameObject
                auto obj = dynamic_cast<GameObject*>(m_objects->objectAtIndex(i));
                if (obj) {
                    // ID объекта 31 в Geometry Dash — это ВСЕГДА StartPosObject
                    if (obj->m_objectID == 31) {
                        m_fields->startX = obj->getPositionX();
                        m_fields->hasStartPos = true;
                        break; // Нам нужен первый активный стартпоз уровня
                    }
                }
            }
        }

        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Стандартная защита от вылетов
        if (!m_uiLayer || !m_player1) return;

        // Поиск текстового лейбла процентов на UI-слое
        CCLabelBMFont* percentLabel = nullptr;
        auto uiChildren = m_uiLayer->getChildren();
        if (uiChildren) {
            for (int i = 0; i < uiChildren->count(); ++i) {
                auto child = dynamic_cast<CCLabelBMFont*>(uiChildren->objectAtIndex(i));
                if (child) {
                    std::string text = child->getString();
                    // Текстовое поле процентов всегда заканчивается на '%'
                    if (!text.empty() && text.back() == '%') {
                        percentLabel = child;
                        break;
                    }
                }
            }
        }

        // Если текстовое поле на экране успешно найдено
        if (percentLabel) {
            double startPercent = 0.0;
            double currentPercent = 0.0;

            // Считаем текущий точный прогресс кубика в реальном времени
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Если при загрузке мы зафиксировали стартпоз (ID 31)
            if (m_fields->hasStartPos && m_levelLength > 0.0f) {
                startPercent = (m_fields->startX / m_levelLength) * 100.0;
            }

            // Защита границ процентов от 0 до 100
            if (startPercent < 0.0) startPercent = 0.0;
            if (startPercent > 100.0) startPercent = 100.0;
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            // Собираем итоговую строку интерфейса
            std::string formatStr;
            // Если стартпоз найден и его процент больше нуля
            if (m_fields->hasStartPos && startPercent > 0.1f) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                // Если стартпоза нет, выводим просто точный текущий процент
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Перезаписываем текст на экране в самом конце кадра
            percentLabel->setString(formatStr.c_str());
        }
    }
};
