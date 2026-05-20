#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool isStartPos = false;
        double startPercent = 0.0;
        bool hasLockedStart = false; // Флаг, чтобы поймать стартпоз ровно ОДИН раз
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        // Полный сброс при каждой смерти — возвращаем всё в исходное состояние
        m_fields->isStartPos = false;
        m_fields->startPercent = 0.0;
        m_fields->hasLockedStart = false;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!m_uiLayer || !m_player1) return;

        // 1. Считаем точный текущий процент кубика прямо сейчас
        double currentPercent = 0.0;
        if (m_levelLength > 0.0f) {
            currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
        }

        if (currentPercent < 0.0) currentPercent = 0.0;
        if (currentPercent > 100.0) currentPercent = 100.0;

        // ТВОЙ ЛЕГКИЙ ПУТЬ: Если кубик сдвинулся с абсолютного нуля (процент > 0.1%)
        // и мы еще НЕ сохраняли стартовую точку для этой попытки
        if (!m_fields->hasLockedStart && currentPercent > 0.1) {
            m_fields->hasLockedStart = true; // Закрываем за собой дверь

            // Если в этот момент дефолтный процент игры равен 0 — значит, мы реально в начале уровня (просто летим с 0%).
            // Но если дефолтный процент игры > 0 ИЛИ точный процент кубика уже большой — значит, это выбранный стартпоз!
            if (this->getCurrentPercent() > 0 || currentPercent > 0.5) {
                m_fields->startPercent = currentPercent; // Намертво фиксируем ПЕРВУЮ часть
                m_fields->isStartPos = true;             // Включаем двойной счетчик
            }
        }

        // 2. Быстрый поиск оригинального текстового лейбла процентов по значку '%'
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

        // 3. Выводим текст на экран
        if (percentLabel) {
            std::string formatStr;
            if (m_fields->isStartPos) {
                // Тот самый идеальный двойной формат, где первая часть залочена, а вторая бежит
                formatStr = fmt::format("{:.1f}%-{:.1f}%", m_fields->startPercent, currentPercent);
            } else {
                // Обычный точный процент, если стартпоза нет
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }
            percentLabel->setString(formatStr.c_str());
        }
    }
};
