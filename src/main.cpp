#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float dt) {
        // 1. Даем игре штатно обновиться каждый кадр
        PlayLayer::update(dt);

        // 2. Проверяем наличие интерфейса и игрока
        if (!m_uiLayer || !m_player1) return;

        // Ищем стандартный текстовый счетчик процентов на экране
        CCLabelBMFont* percentLabel = nullptr;
        
        // Перебираем элементы UI-слоя
        auto children = m_uiLayer->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                // Исправлено: используем правильный typeinfo_cast вместо опечатки
                auto child = typeinfo_cast<CCLabelBMFont*>(children->objectAtIndex(i));
                if (child) {
                    std::string text = child->getString();
                    // Защита: ищем текстовый блок, который заканчивается на значок '%'
                    if (!text.empty() && text.back() == '%') {
                        percentLabel = child;
                        break;
                    }
                }
            }
        }

        // Если счетчик на экране найден, рассчитываем наши кастомные проценты
        if (percentLabel) {
            double startPercent = 0.0;
            bool hasStartPos = false;
            float startX = 0.0f;

            // Ищем объекты стартпоза на уровне через базовые ноды сцены
            auto objects = this->getChildren();
            if (objects) {
                for (int i = 0; i < objects->count(); ++i) {
                    auto obj = objects->objectAtIndex(i);
                    if (obj) {
                        std::string className = typeid(*obj).name();
                        // Проверяем, относится ли объект к типу StartPos
                        if (className.find("StartPosObject") != std::string::npos) {
                            auto node = reinterpret_cast<CCNode*>(obj);
                            startX = node->getPositionX();
                            hasStartPos = true;
                            break; 
                        }
                    }
                }
            }

            // Если на уровне реально стоит стартпоз, перезаписываем отображение
            if (hasStartPos) {
                if (m_levelLength > 0.0f) {
                    startPercent = (startX / m_levelLength) * 100.0;
                }

                if (startPercent < 0.0) startPercent = 0.0;
                if (startPercent > 100.0) startPercent = 100.0;

                // Считаем точный текущий прогресс кубика
                double currentPercent = 0.0;
                if (m_levelLength > 0.0f) {
                    currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
                }
                if (currentPercent < 0.0) currentPercent = 0.0;
                if (currentPercent > 100.0) currentPercent = 100.0;

                // Собираем нужный формат строки "39.4%-52.8%"
                std::string formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);

                // Выводим измененный текст на экран
                percentLabel->setString(formatStr.c_str());
            }
        }
    }
};
