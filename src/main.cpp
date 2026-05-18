#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool hasStartPos = false;
        float startX = 0.0f;
        bool firstFrame = true;
    };

    bool init(GJGameLevel* level, bool usePracticeMode, bool isPlaytest) {
        if (!PlayLayer::init(level, usePracticeMode, isPlaytest)) return false;
        
        // Сбрасываем флаги при старте уровня
        m_fields->hasStartPos = false;
        m_fields->startX = 0.0f;
        m_fields->firstFrame = true;
        
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Стандартная защита от вылетов
        if (!m_uiLayer || !m_player1) return;

        // В самый первый кадр игры проверяем, где появился игрок.
        // Если он появился дальше координаты X = 20.0f, значит, активен StartPos!
        if (m_fields->firstFrame) {
            m_fields->firstFrame = false;
            float currentX = m_player1->getPositionX();
            if (currentX > 20.0f) {
                m_fields->startX = currentX;
                m_fields->hasStartPos = true;
            }
        }

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

        // Если лейбл найден, перезаписываем его значение
        if (percentLabel) {
            double startPercent = 0.0;
            double currentPercent = 0.0;

            // Считаем текущий точный прогресс кубика
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Рассчитываем стартовый процент на основе сохраненной в первый кадр позиции
            if (m_fields->hasStartPos && m_levelLength > 0.0f) {
                startPercent = (m_fields->startX / m_levelLength) * 100.0;
            }

            // Жесткая защита границ процентов от 0 до 100
            if (startPercent < 0.0) startPercent = 0.0;
            if (startPercent > 100.0) startPercent = 100.0;
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            std::string formatStr;
            // Если стартпоз активен, выводим двойной формат
            if (m_fields->hasStartPos && startPercent > 0.10) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                // Если стартпоза нет, пишем просто точный текущий процент
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Перезаписываем текст в самом конце кадра
            percentLabel->setString(formatStr.c_str());
        }
    }
};
