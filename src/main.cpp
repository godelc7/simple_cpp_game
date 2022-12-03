#include "fighter.h"


extern std::mutex g_mutex; // NOLINT --> deactivate all clang-tidy on this line
bool g_game_running{false}; // NOLINT


void execute_hero_actions(const Hero &hero, 
                          Monster &orc, 
                          Monster &dragon)
{
    std::string command_in; // NOLINT
    std::string command;

    while (g_game_running)
    {
        std::cout << "Enter an attack command: ";
        std::getline(std::cin, command_in);
        command.clear();

        for(auto item : command_in){ // NOLINT #pragma unroll
            command += static_cast<char>( tolower(item) );
        }

        if (command == "attack orc") {
            #pragma omp critical
            hero.Attack(orc);
        }
        else if (command == "attack dragon") {
            #pragma omp critical
            hero.Attack(dragon);
        }

        if (!hero.IsAlive() || (!dragon.IsAlive() && !orc.IsAlive())) {
            #pragma omp critical
            g_game_running = false;
        }
    }

    std::cout << "\033[32m";
    std::cout << "\n-----------------------------------------";
    std::cout << "\n|        THREAD 0: GAME OVER            |";
    std::cout << "\n|              YOU WIN                  |";
    std::cout << "\n-----------------------------------------\n\n";
    std::cout << "\033[0m";

    std::lock_guard<std::mutex> lock1(g_mutex); // NOLINT
    exit(EXIT_SUCCESS); // NOLINT : thread unsafe
}


void execute_monster_actions(Hero &hero, 
                             const Monster &orc,
                             const Monster &dragon)
{
    constexpr int SLEEP_TIME{500};
    constexpr int ORC_ATTACK_INTERVAL{1500};
    constexpr int DRAGON_ATTACK_INTERVAL{2000};
    constexpr int TIME_TOLERANCE{20};
    constexpr int START_FIGHT_TIME{1000};
    const auto start = std::chrono::high_resolution_clock::now();

    while (g_game_running) // NOLINT #pragma unroll
    {
        /*
         * TODO(Godel): is it possible to trigger an action with a time 
                        accuracy in milliseconds ? This might perhaps 
                        require a real time kernel or some special 
                        hardware !! Workaround: For now reduce the 
                        time accuracy.
         */
        std::this_thread::sleep_for(
            std::chrono::milliseconds(SLEEP_TIME)
        );

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::\
        duration_cast<std::chrono::milliseconds>(end - start).count();

        if (elapsed % ORC_ATTACK_INTERVAL < TIME_TOLERANCE && 
            elapsed > START_FIGHT_TIME) {
            #pragma omp critical
            orc.Attack(hero);
        }
        if (elapsed % DRAGON_ATTACK_INTERVAL < TIME_TOLERANCE && 
            elapsed > START_FIGHT_TIME) {
            #pragma omp critical
            dragon.Attack(hero);
        }
        if (!hero.IsAlive() || (!dragon.IsAlive() && !orc.IsAlive())) {
            #pragma omp critical
            g_game_running = false;
        }
    }

    std::cout << "\033[31m";
    std::cout << "\n-----------------------------------------";
    std::cout << "\n|        THREAD 1: GAME OVER            |";
    std::cout << "\n|             YOU LOOSE                 |";
    std::cout << "\n-----------------------------------------\n\n";
    std::cout << "\033[0m";

    std::lock_guard<std::mutex> lock2(g_mutex); // NOLINT
    exit(EXIT_SUCCESS); // NOLINT
}


int main()
{
    auto hero = Hero(ROLE_HERO);
    auto orc = Monster(ROLE_ORC);
    auto dragon = Monster(ROLE_DRAGON);

    #pragma omp parallel num_threads(2) default(none) \
    shared(std::cout, std::cin, hero, orc, dragon, g_mutex, g_game_running)
    {
        g_game_running = true;

        if (omp_get_thread_num() == 0){
            execute_hero_actions(hero, orc, dragon);
        }
        else{
            execute_monster_actions(hero, orc, dragon);
        }
    }

    //return EXIT_SUCCESS;
}


//attack orc
//attack dragon









