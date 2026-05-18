#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool isStartPos = false;
        double startPercent = 0.0;
        bool needCheck = false; // Флаг, который говорит, нужно ли проверить стартпоз
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        // При каждом рестарте/смерти сбрасываем данные и взводим флаг проверки
        m_fields->isStartPos = false;
        m_fields->startPercent = 0.0;
        m_fields->needCheck = true; 
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_uiLayer || !m_player1) return;

        // Фиксируем стартовый процент в самый первый кадр ПОСЛЕ рестарта,
        // когда кубик гарантированно встал на свою физическую позицию
        if (m_fields->needCheck) {
            m_fields->needCheck = false; // Проверяем только один раз за попытку

            if (m_levelLength > 0.0f) {
                double initialPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;

                // Твоя гениальная идея: если процент больше микроскопического нуля
                if (initialPercent > 0.01) {
                    m_fields->startPercent = initialPercent;
                    m_fields->isStartPos = true;
                }
            }
        }

        // Быстрый поиск оригинального текстового лейбла процентов по значку '%'
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
            // Считаем точный текущий процент кубика прямо сейчас
            double currentPercent = 0.0;
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Защита границ процентов от 0 до 100
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            std::string formatStr;
            // Если мы зафиксировали стартпоз (первая часть теперь залочена на точном значении)
            if (m_fields->isStartPos) {
                // Выводим двойной формат: "39.4%-52.8%"
                formatStr = fmt::format("{:.1f}%-{:.1f}%", m_fields->startPercent, currentPercent);
            } else {
                // Если стартпоза нет, выводим просто текущий точный процент: "52.8%"
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Перезаписываем текст поверх стандартного счетчика игры
            percentLabel->setString(formatStr.c_str());
        }
    }
};
