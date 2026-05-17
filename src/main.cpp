#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    // В старых версиях 2.2074 хукаем именно этот метод обновления процентов
    void updatePercentLabel() {
        // 1. Сначала даем игре обновиться в штатном режиме
        PlayLayer::updatePercentLabel();

        // 2. Проверяем, существует ли интерфейс UI и сам счетчик процентов на экране
        if (m_uiLayer && m_uiLayer->m_percentageLabel) {
            
            // Если игрок запустил уровень со StartPos
            if (m_isTestMode || m_isPracticeMode) {
                
                // В старых версиях игры стартовый процент рассчитывается встроенной функцией
                float startPercent = 0.0f;
                if (m_levelSettings) {
                    // Используем безопасный дефолтный расчет или старую переменную позиции
                    startPercent = m_levelSettings->m_startPos.x; 
                    if (m_levelLength > 0.0f) {
                        startPercent = (startPercent / m_levelLength) * 100.0f;
                    }
                }
                
                // Ограничиваем рамками
                if (startPercent < 0.0f) startPercent = 0.0f;
                if (startPercent > 100.0f) startPercent = 100.0f;

                // Получаем текущий процент игрока
                int currentPercent = this->getCurrentPercent();

                // Форматируем строку в нужный вид: [Старт]% - [Текущий]%
                std::string formatStr = fmt::format("{:.0f}% - {}%", startPercent, currentPercent);

                // Заменяем текст старого счетчика в UI слое игры
                m_uiLayer->m_percentageLabel->setString(formatStr.c_str());
            }
        }
    }
};
