#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void updateProgressLabel() {
        // 1. Даем игре обновиться в штатном режиме
        PlayLayer::updateProgressLabel();

        // 2. Проверяем UI-слой и счетчик процентов
        if (m_uiLayer && m_uiLayer->m_percentageLabel) {
            
            // Проверяем, есть ли вообще стартпозы на уровне
            if (m_startPosObjects && m_startPosObjects->count() > 0) {
                
                double startPercent = 0.0;
                CCNode* activeStartPos = nullptr;

                // Проверяем, активирован ли конкретный стартпоз игроком
                if (m_activatedStartPos) {
                    activeStartPos = reinterpret_cast<CCNode*>(m_activatedStartPos);
                } else {
                    // Если нет явно активного, берем первый доступный из массива
                    auto firstObj = m_startPosObjects->objectAtIndex(0);
                    if (firstObj) {
                        activeStartPos = reinterpret_cast<CCNode*>(firstObj);
                    }
                }

                // Если стартпоз успешно определен, считаем его X координату
                if (activeStartPos) {
                    float startX = activeStartPos->getPositionX();
                    if (m_levelLength > 0.0f) {
                        startPercent = (startX / m_levelLength) * 100.0;
                    }
                }

                // Ограничиваем рамки процентов от 0 до 100
                if (startPercent < 0.0) startPercent = 0.0;
                if (startPercent > 100.0) startPercent = 100.0;

                // Считаем точный ТЕКУЩИЙ процент игрока с дробной частью
                double currentPercent = 0.0;
                if (m_player1 && m_levelLength > 0.0f) {
                    currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
                }
                if (currentPercent < 0.0) currentPercent = 0.0;
                if (currentPercent > 100.0) currentPercent = 100.0;

                // Форматируем строку с точностью до одной десятой: "39.4%-52.8%"
                std::string formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);

                // Выводим текст на стандартное верхнее место по центру
                m_uiLayer->m_percentageLabel->setString(formatStr.c_str());
            }
        }
    }
};
