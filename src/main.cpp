#include <iostream>

#include "Game.h"


int main() {
    try {

        std::cout << "🌾 Запуск БУЛЬБИК: ДО КОРЕНЯ ЗЛА" << std::endl;

        Game game{"🌾 БУЛЬБИК: ДО КОРЕНЯ ЗЛА"};
        game.run();

        std::cout << "✅ Гра завершена успішно!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "❌ Помилка: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Невідома помилка!" << std::endl;
        return 1;
    }

    return 0;
}
