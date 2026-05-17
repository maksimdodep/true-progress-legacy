#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void updateProgressLabel() {
        // 1. Даем игре обновиться в штатном режиме
        PlayLayer::updateProgressLabel();

        // 2. Проверяем UI-слой и счетчик процентов с правильным именем m_percentageLabel
        if (m_uiLayer && m_uiLayer->m_percentageLabel) {
            
            // ПРОВЕРКА: Проверяем, есть ли на уровне активированные объекты StartPos
            if (m_startPosObjects && m_startPosObjects->count() > 0) {
                
                double startPercent = 0.0;
                
                // Достаем самый первый объект StartPos из массива игры
                auto startPosObj = static_cast<CCObject*>(m_startPosObjects->objectAtIndex(0));
                
                if (startPosObj) {
                    // Принудительно приводим к типу ноды, чтобы забрать координату X
                    auto node = static_cast<CCNode*>(startPosObj);
                    float startX = node->getPositionX();

                    // Высчитываем реальный процент старта от длины уровня
                    if (m_levelLength > 0.0f) {
                        startPercent = (startX / m_levelLength) * 100.0;
                    }
                }

                // Защита границ, чтобы не вылезло -0% или 101%
                if (startPercent < 0.0) startPercent = 0.0;
                if (startPercent > 100.0) startPercent = 100.0;

                // Получаем текущий процент рана игрока
                int currentPercent = this->getCurrentPercent();

                // Форматируем строку: [Реальный Старт]% - [Текущий]%
                std::string formatStr = fmt::format("{:.0f}% - {}%", startPercent, currentPercent);

                // Заменяем текст на главном счетчике игры верхнего слоя
                m_uiLayer->m_percentageLabel->setString(formatStr.c_str());
            }
            // ЕСЛИ СТАРТПОЗА НЕТ: код ничего не делает, и игра выводит дефолтные проценты!
        }
    }
};
