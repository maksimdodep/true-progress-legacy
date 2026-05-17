#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float dt) {
        // 1. Штатное обновление игры
        PlayLayer::update(dt);

        // Защита от вылета (проверяем базовые объекты)
        if (!m_uiLayer || !m_player1) return;

        // Ищем текстовый счетчик процентов на экране интерфейса
        CCLabelBMFont* percentLabel = nullptr;
        
        auto children = m_uiLayer->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                // Безопасное динамическое приведение к типу текстового лейбла
                auto child = dynamic_cast<CCLabelBMFont*>(children->objectAtIndex(i));
                if (child) {
                    std::string text = child->getString();
                    // Ищем элемент, текст которого заканчивается на значок '%'
                    if (!text.empty() && text.back() == '%') {
                        percentLabel = child;
                        break;
                    }
                }
            }
        }

        // Если текстовое поле на экране успешно найдено
        if (percentLabel) {
            double startPercent = 0.0;

            // Считаем точный текущий процент кубика в реальном времени
            double currentPercent = 0.0;
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Ограничиваем рамки процентов от 0 до 100
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            // Проверяем, идет ли игрок с начала уровня или со стартпоза
            int gamePercent = this->getCurrentPercent(); 
            
            // Если дефолтная игра думает, что прогресс 0%, но кубик уже пролетел вперед, 
            // значит, на уровне активирован стартпоз
            if (gamePercent == 0 && m_player1->getPositionX() > 100.0f) {
                startPercent = currentPercent; 
            }

            // Собираем кастомный текст строки
            std::string formatStr;
            if (startPercent > 0.5) {
                // Если активен стартпоз: "39.4%-52.8%"
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                // Если стартпоза нет, просто выводим точный текущий процент: "52.8%"
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Обновляем текст на экране игры
            percentLabel->setString(formatStr.c_str());
        }
    }
};
