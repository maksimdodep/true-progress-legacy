#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float dt) {
        // 1. Штатное обновление игры
        PlayLayer::update(dt);

        // Проверяем UI и игрока
        if (!m_uiLayer || !m_player1) return;

        // Исправлено: в этой версии поле называется m_percentLabel
        if (m_uiLayer->m_percentLabel) {
            
            double startPercent = 0.0;
            CCNode* activeStartPosNode = nullptr;

            // Исправлено: используем m_activeStartPos (как подсказал компилятор)
            if (m_activeStartPos) {
                activeStartPosNode = reinterpret_cast<CCNode*>(m_activeStartPos);
            }

            // Если активный стартпоз найден, считаем проценты от него
            if (activeStartPosNode && m_levelLength > 0.0f) {
                float startX = activeStartPosNode->getPositionX();
                startPercent = (startX / m_levelLength) * 100.0;
            }

            // Считаем точный текущий процент игрока с дробной частью
            double currentPercent = 0.0;
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Защита от выхода за границы 0-100%
            if (startPercent < 0.0) startPercent = 0.0;
            if (startPercent > 100.0) startPercent = 100.0;
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            // Если игрок идет НЕ с начала уровня (выбран стартпоз)
            if (startPercent > 0.01) {
                // Форматируем строку: "39.4%-52.8%"
                std::string formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
                m_uiLayer->m_percentLabel->setString(formatStr.c_str());
            } else {
                // Если стартпоза нет, выводим просто текущий точный процент "52.8%"
                std::string formatStr = fmt::format("{:.1f}%", currentPercent);
                m_uiLayer->m_percentLabel->setString(formatStr.c_str());
            }
        }
    }
};
