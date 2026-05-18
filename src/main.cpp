#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool hasStartPos = false;
        float startX = 0.0f;
    };

    // Перехватываем инициализацию уровня
    bool init(GJGameLevel* level, bool usePracticeMode, bool isPlaytest) {
        if (!PlayLayer::init(level, usePracticeMode, isPlaytest)) return false;

        // В Geometry Dash, если активирован Стартпоз, игра создаёт настройки уровня на его основе.
        // Мы проверяем настройки уровня. Если они указывают на кастомную точку, значит стартпоз активен!
        if (m_levelSettings && m_levelSettings->m_startPos) {
            auto spNode = reinterpret_cast<CCNode*>(m_levelSettings->m_startPos);
            if (spNode) {
                m_fields->startX = spNode->getPositionX();
                m_fields->hasStartPos = true;
            }
        }
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Стандартная защита от вылетов
        if (!m_uiLayer || !m_player1) return;

        // Ищем текстовый счетчик процентов на экране интерфейса
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

        // Если лейбл найден, принудительно перезаписываем его
        if (percentLabel) {
            double startPercent = 0.0;
            double currentPercent = 0.0;

            // Считаем текущий точный прогресс кубика
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Рассчитываем стартовый процент
            if (m_fields->hasStartPos && m_levelLength > 0.0f) {
                startPercent = (m_fields->startX / m_levelLength) * 100.0;
            }

            // Жесткая защита границ процентов от 0 до 100
            if (startPercent < 0.0) startPercent = 0.0;
            if (startPercent > 100.0) startPercent = 100.0;
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            std::string formatStr;
            // Исправлено: Убрали капризную проверку getPositionX() >= startX. 
            // Если стартпоз на уровне был активирован (startPercent > 0.1), выводим двойной формат.
            if (m_fields->hasStartPos && startPercent > 0.10) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                // Если стартпоза нет (или он в самом начале 0.0%), пишем просто точный текущий процент
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Перезаписываем текст в самом конце кадра, побеждая стандартный счетчик игры
            percentLabel->setString(formatStr.c_str());
        }
    }
};
