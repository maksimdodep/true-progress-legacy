#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool isStartPos = false;
        double startPercent = 0.0;
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        m_fields->isStartPos = false;
        m_fields->startPercent = 0.0;

        // Железобетонный поиск: перебираем объекты уровня ТОЛЬКО ОДИН РАЗ при рестарте.
        // Ищем объект StartPosObject, который игра активировала для этой попытки.
        auto children = this->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                auto child = children->objectAtIndex(i);
                if (child) {
                    std::string className = typeid(*child).name();
                    // Ищем оригинальный игровой класс точки старта
                    if (className.find("StartPosObject") != std::string::npos) {
                        auto node = dynamic_cast<CCNode*>(child);
                        // Проверяем, что этот стартпоз находится в рабочей зоне уровня
                        if (node && m_levelLength > 0.0f) {
                            float startX = node->getPositionX();
                            double calculated = (startX / m_levelLength) * 100.0;
                            
                            // Если координата стартпоза дальше начала уровня — это наш целевой стартпоз!
                            if (calculated > 0.1) {
                                m_fields->startPercent = calculated;
                                m_fields->isStartPos = true;
                                break; 
                            }
                        }
                    }
                }
            }
        }
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_uiLayer || !m_player1) return;

        CCLabelBMFont* percentLabel = nullptr;
        auto uiChildren = m_uiLayer->getChildren();
        if (uiChildren) {
            for (int i = 0; i < uiChildren->count(); ++i) {
                auto child = dynamic_cast<CCLabelBMFont*>(uiChildren->objectAtIndex(i));
                if (child) {
                    std::string text = child->getString();
                    if (!text.empty() && text.back() == '%') {
                        percentLabel = child;
                        break;
                    }
                }
            }
        }

        if (percentLabel) {
            double currentPercent = 0.0;
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            std::string formatStr;
            // Теперь данные стартпоза берутся напрямую из объекта при рестарте кадра
            if (m_fields->isStartPos) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", m_fields->startPercent, currentPercent);
            } else {
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            percentLabel->setString(formatStr.c_str());
        }
    }
};
