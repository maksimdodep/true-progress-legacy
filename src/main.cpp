#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>

using namespace geode::prelude;

class $modify(CCLabelBMFont) {
    void setString(const char* text) {
        std::string str(text);

        // Если строка не пустая и заканчивается на знак процента (это наш счетчик!)
        if (!str.empty() && str.back() == '%') {
            // Проверяем, что это просто число процентов, а не левый текст
            // (в старых версиях строка имеет вид "11%" или "11.3%")
            if (str.find('-') == std::string::npos) { 
                
                // Для теста пишем старт как 0%
                std::string newText = "0% - " + str;
                
                // Вызываем оригинальный метод с нашим новым текстом
                CCLabelBMFont::setString(newText.c_str());
                return;
            }
        }

        // Во всех остальных случаях отдаем текст игре в штатном режиме
        CCLabelBMFont::setString(text);
    }
};
