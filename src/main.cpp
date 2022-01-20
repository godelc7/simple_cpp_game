#include "fighter.h"

int main()
{
    Hero hero = Hero(ROLE_HERO);
    auto orc = Monster(ROLE_ORC), dragon = Monster(ROLE_DRAGON);

    std::string command;
    bool game_running = true;

#pragma omp parallel num_threads(2) default(none) shared(game_running, std::cout, std::cin, hero, orc, dragon) private(command)
{
    if (omp_get_thread_num() == 0)
    {
        while (game_running)
        {
            std::cout << "Enter an attack command: ";
            std::getline(std::cin, command);

            for (auto itr = command.begin(); itr != command.end(); ++itr)
                *itr = static_cast<char>( tolower(*itr));

            if (command == "attack orc") {
                #pragma omp critical
                hero.Attack(orc);
            }
            if (command == "attack dragon") {
                #pragma omp critical
                hero.Attack(dragon);
            }

            if (!hero.IsAlive() || (!dragon.IsAlive() && !orc.IsAlive())) {
                #pragma omp critical
                game_running = false;
            }
        }

        std::cout << "\n-----------------------------------------";
        std::cout << "\n|        THREAD 0: GAME OVER            |";
        std::cout << "\n-----------------------------------------\n\n";

        exit(EXIT_SUCCESS);
    }
    else
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        while (game_running)
        {
            /*
             * TODO @Kamdoum: is it possible to trigger an action with a time accuracy in milliseconds ?
             *                This might perhaps require a real time kernel or some special hardware !!
             *                Workaround: For now reduce the time accuracy.
             */

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            auto end_time = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            if (elapsed % 1500 < 20 && elapsed > 1000) {
                #pragma omp critical
                orc.Attack(hero);
            }

            if (elapsed % 2000 < 20 && elapsed > 1000) {
                #pragma omp critical
                dragon.Attack(hero);
            }

            if (!hero.IsAlive() || (!dragon.IsAlive() && !orc.IsAlive())) {
                #pragma omp critical
                game_running = false;
            }
        }

        std::cout << "\n-----------------------------------------";
        std::cout << "\n|        THREAD 1: GAME OVER            |";
        std::cout << "\n-----------------------------------------\n\n";

        exit(EXIT_SUCCESS);
    }
}

    //return EXIT_SUCCESS;
}


//attack orc
//attack dragon









