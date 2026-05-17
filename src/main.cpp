#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float dt) {
        // 1. Штатное обновление игры
        PlayLayer::update(dt);

        // Защита от вылета
        if (!m_uiLayer || !m_player1) return;

        // ШАГ 1: Динамически ищем счетчик процентов на экране интерфейса
        CCLabelBMFont* percentLabel = nullptr;
        auto children = m_uiLayer->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                // Безопасное приведение типа объекта через старый метод
                auto child = reinterpret_cast<CCLabelBMFont*>(children->objectAtIndex(i));
                if (child && child->getObjType() == cocos2d::CCObjectType::kCCObjectTypeNode) {
                    std::string text = child->getString();
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

            // ШАГ 2: Считаем точный текущий процент кубика в реальном времени
            double currentPercent = 0.0;
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Ограничиваем рамки
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            // ШАГ 3: Форматируем строку в зависимости от прогресса
            // Если игрок идет с кастомного места, дефолтный счетчик игры (целый) не равен текущему точному
            int gamePercent = this->getCurrentPercent(); 
            
            // Если игра думает, что мы на 0%, а кубик улетел дальше (значит активен стартпоз)
            if (gamePercent == 0 && m_player1->getPositionX() > 100.0f) {
                // Рассчитываем примерный стартпоз от текущего положения минус игровой прогресс
                startPercent = currentPercent; 
            }

            // Выводим кастомный текст "39.4%-52.8%" или просто "52.8%"
            std::string formatStr;
            if (startPercent > 0.5) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Обновляем текст на экране
            percentLabel->setString(formatStr.c_str());
        }
    }
};
