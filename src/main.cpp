#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_uiLayer || !m_player1) return;

        double startPercent = 0.0;
        bool hasStartPos = false;

        // Железобетонный способ для 2.2074: проверяем настройки уровня
        if (m_levelSettings) {
            // В этой ревизии биндингов активный стартпоз можно достать, 
            // просто найдя объект класса StartPosObject среди дочерних нод самого PlayLayer
            auto children = this->getChildren();
            if (children) {
                for (int i = 0; i < children->count(); ++i) {
                    auto child = children->objectAtIndex(i);
                    if (child) {
                        std::string className = typeid(*child).name();
                        // Если имя класса содержит StartPosObject
                        if (className.find("StartPosObject") != std::string::npos) {
                            auto node = dynamic_cast<CCNode*>(child);
                            // Проверяем, совпадает ли позиция кубика в начале рана с этим стартпозом,
                            // либо просто берем его координату X, если игрок улетел дальше нее
                            if (node && m_player1->getPositionX() >= node->getPositionX() - 10.0f) {
                                float startX = node->getPositionX();
                                if (m_levelLength > 0.0f) {
                                    startPercent = (startX / m_levelLength) * 100.0;
                                    // Если мы улетели дальше начала уровня, значит стартпоз активен!
                                    if (startPercent > 0.1) {
                                        hasStartPos = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Считаем точный текущий процент кубика в реальном времени
        double currentPercent = 0.0;
        if (m_levelLength > 0.0f) {
            currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
        }

        if (currentPercent < 0.0) currentPercent = 0.0;
        if (currentPercent > 100.0) currentPercent = 100.0;

        // Поиск оригинального текстового лейбла процентов по значку '%'
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

        // Перерисовываем текст на экране
        if (percentLabel) {
            std::string formatStr;
            if (hasStartPos) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }
            percentLabel->setString(formatStr.c_str());
        }
    }
};
