#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void updateProgressLabel() {
        // 1. Даем игре обновиться штатно
        PlayLayer::updateProgressLabel();

        // 2. Проверяем наличие интерфейса и игрока
        if (!m_uiLayer || !m_player1) return;

        // Ищем стандартный текстовый счетчик процентов на экране
        CCLabelBMFont* percentLabel = nullptr;
        
        // Перебираем все элементы на UI-слое, чтобы найти текстовое поле процентов
        auto children = m_uiLayer->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                auto child = card_cast<CCLabelBMFont*>(children->objectAtIndex(i));
                // Текстовые проценты в GD обычно имеют определенный тег или позицию, 
                // но самый надежный способ — проверить, заканчивается ли текст на '%'
                if (child) {
                    std::string text = child->getString();
                    if (!text.empty() && text.back() == '%') {
                        percentLabel = child;
                        break;
                    }
                }
            }
        }

        // Если счетчик на экране найден
        if (percentLabel) {
            double startPercent = 0.0;
            bool hasStartPos = false;
            float startX = 0.0f;

            // Ищем StartPos объекты на самом уровне среди всех объектов слоя
            auto objects = this->getChildren();
            if (objects) {
                for (int i = 0; i < objects->count(); ++i) {
                    auto obj = objects->objectAtIndex(i);
                    if (obj) {
                        // В движке GD объекты StartPos имеют определенный тип (обычно StartPosObject)
                        // Проверим имя класса объекта, чтобы точно найти стартпоз
                        std::string className = typeid(*obj).name();
                        if (className.find("StartPosObject") != std::string::npos) {
                            auto node = reinterpret_cast<CCNode*>(obj);
                            // Для простоты берем первый найденный или активный стартпоз
                            startX = node->getPositionX();
                            hasStartPos = true;
                            break; 
                        }
                    }
                }
            }

            // Если на уровне реально используется стартпоз
            if (hasStartPos) {
                if (m_levelLength > 0.0f) {
                    startPercent = (startX / m_levelLength) * 100.0;
                }

                if (startPercent < 0.0) startPercent = 0.0;
                if (startPercent > 100.0) startPercent = 100.0;

                // Считаем точный текущий процент игрока
                double currentPercent = 0.0;
                if (m_levelLength > 0.0f) {
                    currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
                }
                if (currentPercent < 0.0) currentPercent = 0.0;
                if (currentPercent > 100.0) currentPercent = 100.0;

                // Собираем строку с десятыми долями: "39.4%-52.8%"
                std::string formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);

                // Меняем текст на экране
                percentLabel->setString(formatStr.c_str());
            }
        }
    }
};
