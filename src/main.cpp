#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    // Используем m_fields для хранения данных, чтобы не терять их между кадрами
    struct Fields {
        bool hasStartPos = false;
        float startX = 0.0f;
    };

    // Перехватываем добавление объектов, чтобы поймать StartPos при загрузке уровня
    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        
        if (obj) {
            std::string className = typeid(*obj).name();
            // Если имя класса содержит StartPosObject, запоминаем его координату
            if (className.find("StartPosObject") != std::string::npos) {
                m_fields->startX = obj->getPositionX();
                m_fields->hasStartPos = true;
            }
        }
    }

    void update(float dt) {
        // 1. Штатное обновление игры
        PlayLayer::update(dt);

        // Защита от вылета
        if (!m_uiLayer || !m_player1) return;

        // Ищем текстовый счетчик процентов на экране интерфейса
        CCLabelBMFont* percentLabel = nullptr;
        auto children = m_uiLayer->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                auto child = dynamic_cast<CCLabelBMFont*>(children->objectAtIndex(i));
                if (child) {
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

            // Считаем точный текущий процент кубика в реальном времени
            double currentPercent = 0.0;
            if (m_levelLength > 0.0f) {
                currentPercent = (m_player1->getPositionX() / m_levelLength) * 100.0;
            }

            // Ограничиваем рамки процентов от 0 до 100
            if (currentPercent < 0.0) currentPercent = 0.0;
            if (currentPercent > 100.0) currentPercent = 100.0;

            // Если при загрузке уровня мы зафиксировали стартпоз
            if (m_fields->hasStartPos && m_levelLength > 0.0f) {
                startPercent = (m_fields->startX / m_levelLength) * 100.0;
            }

            if (startPercent < 0.0) startPercent = 0.0;
            if (startPercent > 100.0) startPercent = 100.0;

            // Собираем итоговую строку интерфейса
            std::string formatStr;
            // Если стартпоз активен и игрок появился дальше, чем самое начало уровня (отступ > 0.5%)
            if (m_fields->hasStartPos && startPercent > 0.5f && m_player1->getPositionX() >= m_fields->startX) {
                formatStr = fmt::format("{:.1f}%-{:.1f}%", startPercent, currentPercent);
            } else {
                // Если стартпоза нет или мы идем с 0%, выводим просто текущий точный процент
                formatStr = fmt::format("{:.1f}%", currentPercent);
            }

            // Обновляем текст на экране игры
            percentLabel->setString(formatStr.c_str());
        }
    }
};
