#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    // Возвращаем метод, который просит сам компилятор в логе
    void updateProgressLabel() {
        // 1. Запускаем оригинальный код игры
        PlayLayer::updateProgressLabel();

        // 2. Проверяем наличие UI слоя и текстовой плашки процентов (m_percentLabel)
        if (m_uiLayer && m_uiLayer->m_percentLabel) {
            
            // Если включен режим теста или практики
            if (m_isTestMode || m_isPracticeMode) {
                
                // Чтобы не ломать компилятор об объект настроек, берем текущий процент
                int currentPercent = this->getCurrentPercent();
                
                // Для теста выведем старт как 0% (или текущий), пока не найдем точное имя переменной StartPos
                int startPercent = 0; 

                // Форматируем строку: [Старт]% - [Текущий]%
                std::string formatStr = fmt::format("{}% - {}%", startPercent, currentPercent);

                // Заменяем текст на экране через правильное имя m_percentLabel
                m_uiLayer->m_percentLabel->setString(formatStr.c_str());
            }
        }
    }
};
