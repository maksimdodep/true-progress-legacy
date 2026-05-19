#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool isStartPos = false;
        double startPercent = 0.0;
        bool hasLockedStart = false;
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        // Сброс данных перед началом новой попытки
        m_fields->isStartPos = false;
        m_fields->startPercent = 0.0;
        m_fields->hasLockedStart = false;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_uiLayer || !m_player1) return;

        // 1. Считаем точный текущий прогресс кубика в реальном времени
        double currentPercent = 0.0;
        if (m_levelLength > 0.0f) {
            currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
        }

        if (currentPercent < 0.0) currentPercent = 0.0;
        if (currentPercent > 100.0) currentPercent = 100.0;

        // 2. Захват стартовой позиции из настроек уровня
        if (!m_fields->hasLockedStart && currentPercent > 0.01) {
            m_fields->hasLockedStart = true; // Выполняем строго один раз за попытку

            if (m_levelSettings && m_levelLength > 0.0f) {
                // Исправлено: используем m_obPosition.x, как подсказал компилятор
                float exactStartX = m_levelSettings->m_obPosition.x;
                double exactStartPercent = (exactStartX / m_levelLength) * 100.0;

                // Если стартпоз находится дальше начала уровня — активируем двойной счетчик
                if (exactStartPercent > 0.01) {
                    m_fields->startPercent = exactStartPercent;
                    m_fields->isStartPos = true;
                }
            }
        }

        // 3. Поиск оригинального текстового лейбла процентов по значку '%'
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

        // 4. Окончательно перерисовываем текст на экране
        if (percentLabel) {
            std::string formatStr;
            if (m_fields->isStartPos) {
                // Первая часть взята из точных координат, вторая часть динамически бежит вперед
                formatStr = fmt::format("{:.1f}%-{:.1f}%", m_fields->startPercent, currentPercent);
            } else {
                // Если стартпоза нет (идем с 0%), выводим обычный точный процент
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }
            percentLabel->setString(formatStr.c_str());
        }
    }
};
