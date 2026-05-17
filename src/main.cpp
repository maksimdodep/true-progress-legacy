#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool hasStartPos = false;
        float startX = 0.0f;
    };

    // Ловим стартпоз в момент загрузки объектов уровня
    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (obj) {
            std::string className = typeid(*obj).name();
            if (className.find("StartPosObject") != std::string::npos) {
                m_fields->startX = obj->getPositionX();
                m_fields->hasStartPos = true;
            }
        }
    }

    // Использование postUpdate гарантирует, что наш код выполнится ПОСЛЕ того,
    // как игра обновит и перезапишет стандартный текст процентов.
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Стандартная защита от вылетов
        if (!m_uiLayer || !m_player1) return;

        CCLabelBMFont* percentLabel = nullptr;
        auto children = m_uiLayer->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                auto child = dynamic_cast<CCLabelBMFont*>(children->objectAtIndex(i));
                if (child) {
                    std::string text = child->getString();
                    // Надежный поиск лейбла процентов по символу '%'
                    if (!text.empty() && text.back() == '%') {
                        percentLabel = child;
                        break;
                    }
                }
            }
        }

        // Если лейбл найден, принудительно перерисовываем его поверх логики игры
        if (percentLabel) {
            double startPercent = 0.0;
            double currentPercent = 0.0;

            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Защита от выходов за границы
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            if (m_fields->hasStartPos && m_levelLength > 0.0f) {
                startPercent = (m_fields->startX / m_levelLength) * 100.0;
            }

            if (startPercent < 0.0) startPercent = 0.0;
            if (startPercent > 100.0) startPercent = 100.0;

            std::string formatStr;
            // Условие: если стартпоз есть и мы физически находимся дальше него
            if (m_fields->hasStartPos && startPercent > 0.5f && m_player1->getPositionX() >= m_fields->startX) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Перезаписываем текст окончательно перед выводом кадра на экран
            percentLabel->setString(formatStr.c_str());
        }
    }
};
