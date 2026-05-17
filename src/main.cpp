#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void updateProgressLabel() {
        // 1. Сначала даем игре обновиться в штатном режиме
        PlayLayer::updateProgressLabel();

        // 2. Безопасно проверяем UI-слой и счетчик процентов
        if (m_uiLayer && m_uiLayer->m_percentLabel) {
            
            // ЖЕСТКАЯ ПРОВЕРКА: Проверяем, существует ли физический объект StartPos на уровне прямо сейчас.
            // Если игрок идет с обычного начала уровня, m_startPosition будет равен nullptr!
            if (m_startPosition != nullptr) {
                
                double startPercent = 0.0;
                
                // Берем координату X из Cocos-позиции физического объекта стартпоза
                float startX = m_startPosition->getPositionX();

                // Высчитываем реальный процент старта от длины уровня
                if (m_levelLength > 0.0f) {
                    startPercent = (startX / m_levelLength) * 100.0;
                }

                // Защита границ, чтобы не вылезло -0% или 101%
                if (startPercent < 0.0) startPercent = 0.0;
                if (startPercent > 100.0) startPercent = 100.0;

                // Получаем текущий процент рана игрока
                int currentPercent = this->getCurrentPercent();

                // Форматируем строку: [Реальный Старт]% - [Текущий]%
                std::string formatStr = fmt::format("{:.0f}% - {}%", startPercent, currentPercent);

                // Заменяем текст СТРОГО на главном счетчике игры верхнего слоя
                m_uiLayer->m_percentLabel->setString(formatStr.c_str());
            }
            // ЕСЛИ СТАРТПОЗА НЕТ: код просто засыпает, и игра выводит дефолтные проценты без нуля!
        }
    }
};
