#include "fighter.h"

extern std::mutex g_mutex; // NOLINT --> deactivate all clang-tidy for this line
bool g_game_running{false}; // NOLINT
const int ORC_ATTACK_INTERVAL = 1500;
const int DRAGON_ATTACK_INTERVAL = 2000;


/**
 * @brief Execute monster actions
 * 
 * This function is called from the main to handle all hero actions, 
 * e.g hitting the monsters. The hero receive its command given by the player
 * of the game via the command line. 
 * 
 * @param hero the Hero of the game
 * @param orc a monster that fight against the Hero
 * @param dragon a monster that fight against the Hero
 */
void execute_hero_actions(const Hero &hero, 
                          Orc &orc, 
                          Dragon &dragon)
{
    std::string command_in; // NOLINT
    std::string command;

    while (g_game_running)
    {
        std::cout << "Enter an attack command: ";
        std::getline(std::cin, command_in);
        command.clear();

        for(auto item : command_in){
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


/**
 * @brief Execute monster actions
 * 
 * This function is called from the main to handle all enemy actions, 
 * e.g enemy hitting the hero. 
 * It's implemented as template function that uses the c++11 
 * trailing return type notation (-> return_type) and c++11 
 * type traits (enable_if, is_base_of) to restrict the template to function
 * calls where the second parameter is a 'Monster', 'Orc' or 'Dragon' type.
 * 
 * @tparam T type of the template parameter, either 'Monster', 'Dragon' or 'Orc'
 * @param hero the Hero of the game
 * @param enemy a monster that fight against the Hero
 * @return std::enable_if_t<std::is_base_of_v<Monster, T>> 
 */
template<typename T>
auto execute_monster_actions(Hero &hero, const T &enemy) 
-> std::enable_if_t<std::is_base_of_v<Monster, T>>
{
    int ATTACK_INTERVAL{0};

    if(enemy.GetRole() == ROLE_ORC){
        ATTACK_INTERVAL = ORC_ATTACK_INTERVAL;
    }
    else if(enemy.GetRole() == ROLE_DRAGON){
        ATTACK_INTERVAL = DRAGON_ATTACK_INTERVAL;
    }

    //const auto start = std::chrono::high_resolution_clock::now();
    //std::chrono::_V2::system_clock::time_point end;
    //long long elapsed{0}; 

    while(g_game_running){
        /*
         * TODO(Godel): is it possible to trigger an event with a time 
                        accuracy in milliseconds ? This might perhaps 
                        require a real time kernel or some special hardware!! 
                        Workaround: Ignore this for now
         */

        std::this_thread::sleep_for(
            std::chrono::milliseconds(ATTACK_INTERVAL)
        );

        //end = std::chrono::high_resolution_clock::now();
        //elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        #pragma omp critical
        enemy.Attack(hero);

        if( !hero.IsAlive() ) {
            #pragma omp critical
            g_game_running = false;
        }
        //std::cout << enemy.RoleToString() << " hits the Hero at " << elapsed << " ms\n";
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


int main(/*int argc, char** argv*/)
{
    auto hero = Hero(ROLE_HERO);
    auto orc = Orc(ROLE_ORC);
    auto dragon = Dragon(ROLE_DRAGON);

    g_game_running = true;

    std::thread hero_thread{
        execute_hero_actions, 
        std::cref(hero), 
        std::ref(orc), 
        std::ref(dragon)
    };
    std::thread orc_thread{
        execute_monster_actions<Orc>, 
        std::ref(hero), 
        std::cref(orc)
    };
    std::thread dragon_thread{
        execute_monster_actions<Dragon>, 
        std::ref(hero), 
        std::cref(dragon)
    };

    hero_thread.join();
    orc_thread.join();
    dragon_thread.join();

    return EXIT_SUCCESS;
}

