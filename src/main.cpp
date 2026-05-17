#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>

using namespace geode::prelude;

class $modify(CCLabelBMFont) {
    void setString(const char* text) {
        std::string str(text);

        // Проверяем, что строка заканчивается на % и не содержит дефис
        if (!str.empty() && str.back() == '%' && str.find('-') == std::string::npos) {
            
            // Фильтр по размеру! Верхний главный счетчик имеет масштаб 1.0 (или около того), 
            // а мелкие плашки модов в углах — меньше 0.4.
            if (this->getScale() > 0.4f) {
                
                // Для теста пишем старт как 0%
                std::string newText = "0% - " + str;
                
                CCLabelBMFont::setString(newText.c_str());
                return;
            }
        }

        // Все остальные надписи и мелкие моды пропускаем без изменений
        CCLabelBMFont::setString(text);
    }
};
