#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    // Используем метод update, так как он гарантированно вызывается каждый кадр
    void update(float dt) {
        // 1. Вызываем оригинальный метод, чтобы игра жила своей жизнью
        PlayLayer::update(dt);

        // Safe check: проверяем, что UI и игрок загрузились
        if (!m_uiLayer || !m_player1) return;

        // В Geode 2.2074 поле лейбла процентов в UILayer называется m_percentageLabel
        // Проверяем его существование
        if (m_uiLayer->m_percentageLabel) {
            
            // Проверяем наличие массива стартпозов уровня (m_startPosObjects)
            if (m_startPosObjects && m_startPosObjects->count() > 0) {
                
                double startPercent = 0.0;
                CCNode* activeStartPos = nullptr;

                // В старых биндингах m_activatedStartPos доступен напрямую
                if (m_activatedStartPos) {
                    activeStartPos = reinterpret_cast<CCNode*>(m_activatedStartPos);
                } else {
                    // Если конкретный не выбран, берем первый из массива
                    auto firstObj = m_startPosObjects->objectAtIndex(0);
                    if (firstObj) {
                        activeStartPos = reinterpret_cast<CCNode*>(firstObj);
                    }
                }

                // Считаем стартовую позицию
                if (activeStartPos && m_levelLength > 0.0f) {
                    float startX = activeStartPos->getPositionX();
                    startPercent = (startX / m_levelLength) * 100.0;
                }

                // Ограничиваем рамки
                if (startPercent < 0.0) startPercent = 0.0;
                if (startPercent > 100.0) startPercent = 100.0;

                // Считаем точный ТЕКУЩИЙ прогресс игрока в реальном времени
                double currentPercent = 0.0;
                if (m_levelLength > 0.0f) {
                    currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
                }
                if (currentPercent < 0.0) currentPercent = 0.0;
                if (currentPercent > 100.0) currentPercent = 100.0;

                // Форматируем строку с точностью до десятых: "39.4%-52.8%"
                std::string formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);

                // Напрямую подменяем текст в оригинальном игровом счетчике
                m_uiLayer->m_percentageLabel->setString(formatStr.c_str());
            }
        }
    }
};
