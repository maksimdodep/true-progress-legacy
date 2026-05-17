#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void updateProgressLabel() {
        // 1. Сначала запускаем оригинальный код игры, чтобы обновился текущий процент
        PlayLayer::updateProgressLabel();
        
        // 2. Проверяем, существует ли текстовое поле процентов на экране
        if (m_progressLabel) {
            
            // Если игрок заспавнился со StartPos (в режиме теста или практики с точки)
            if (m_isTestMode || m_isPracticeMode) {
                double startPercent = 0.0;
                
                // Достаем настройки уровня и проверяем физический объект StartPos
                if (m_levelSettings && m_levelSettings->m_startPosition) {
                    float startX = m_levelSettings->m_startPosition->getPosition().x;
                    
                    // Защита от деления на ноль (проверяем длину уровня)
                    if (m_levelLength > 0.0f) {
                        startPercent = (startX / m_levelLength) * 100.0;
                        
                        // Ограничиваем рамками от 0 до 100 на всякий случай
                        if (startPercent < 0.0) startPercent = 0.0;
                        if (startPercent > 100.0) startPercent = 100.0;
                    }
                }
                
                // Получаем текущий процент игрока
                int currentPercent = this->getCurrentPercent();
                
                // Форматируем строку в нужный вид: [Старт]% - [Текущий]%
                std::string formatStr = fmt::format("{:.0f}% - {}%", startPercent, currentPercent);
                
                // Силой заменяем стандартный текст игры на наш кастомный сверху экрана
                m_progressLabel->setString(formatStr.c_str());
            }
        }
    }
};
