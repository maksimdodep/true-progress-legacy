#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool isStartPos = false;
        double startPercent = 0.0;
        int framesAfterReset = -1; // Счетчик кадров после рестарта
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        // При каждом рестарта сбрасываем данные и запускаем счетчик кадров
        m_fields->isStartPos = false;
        m_fields->startPercent = 0.0;
        m_fields->framesAfterReset = 0; 
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_uiLayer || !m_player1) return;

        // Если счетчик кадров активен
        if (m_fields->framesAfterReset >= 0) {
            m_fields->framesAfterReset++;

            // Ждем ровно 10 кадров, пока игра телепортирует кубик на стартпоз
            if (m_fields->framesAfterReset >= 10) {
                m_fields->framesAfterReset = -1; // Выключаем проверку до следующей смерти

                if (m_levelLength > 0.0f) {
                    double initialPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;

                    // Твоя идея: если спустя 10 кадров мы все еще дальше 0.1%, значит это 100% стартпоз!
                    if (initialPercent > 0.1) {
                        m_fields->startPercent = initialPercent;
                        m_fields->isStartPos = true;
                    }
                }
            }
        }

        // Быстрый поиск текстового лейбла процентов по значку '%'
        CCLabelBMFont* percentLabel = nullptr;
        auto uiChildren = m_uiLayer->getChildren();
        if (uiChildren) {
            for (int i = 0; i < uiChildren->count(); ++i) {
                auto child = dynamic_cast<CCLabelBMFont*>(uiChildren->objectAtIndex(i));
                if (child) {
                    std::string text = child->getString();
                    if (!text.empty() && text.back() == '%') {
                        percentLabel = child;
                        break;
                    }
                }
            }
        }

        // Если текстовое поле на экране найдено
        if (percentLabel) {
            double currentPercent = 0.0;
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            std::string formatStr;
            // Если стартпоз зафиксирован (первая часть залочена на точном значении)
            if (m_fields->isStartPos) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", m_fields->startPercent, currentPercent);
            } else {
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Перезаписываем текст поверх стандартного счетчика игры
            percentLabel->setString(formatStr.c_str());
        }
    }
};
