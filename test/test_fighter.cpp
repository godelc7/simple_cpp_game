#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include "gtest/gtest.h"
#include "fighter.h"


TEST(Fighter, DefaultConstructor)
{
    const Fighter f11;

    EXPECT_EQ(f11.GetRole(), ROLE_UNDEFINED);
    EXPECT_EQ(f11.GetHealth(), HEALTH_UNDEFINED);
}

TEST(Fighter, ConstructorFromRole)
{
    //note: by calling Fighter(int) Fighter(ROLE_t) 
    //is also called, so both constructors are tested here
    //note: Getter methods are also tested here
    auto f_un = Fighter(-1);
    auto f_h = Fighter(0);
    auto f_o = Fighter(1);
    auto f_d = Fighter(2);

    EXPECT_EQ(f_un.GetRole(), ROLE_UNDEFINED);
    EXPECT_EQ(f_un.GetHealth(), HEALTH_UNDEFINED);

    EXPECT_EQ(f_h.GetRole(), ROLE_HERO);
    EXPECT_EQ(f_h.GetHealth(), HEALTH_HERO);

    EXPECT_EQ(f_o.GetRole(), ROLE_ORC);
    EXPECT_EQ(f_o.GetHealth(), HEALTH_ORC);

    EXPECT_EQ(f_d.GetRole(), ROLE_DRAGON);
    EXPECT_EQ(f_d.GetHealth(), HEALTH_DRAGON);
}

TEST(Fighter, CopyConstructor)
{
    auto f11 = Fighter(0);
    auto f_copy(f11);

    EXPECT_EQ(f11.GetRole(), f_copy.GetRole());
    EXPECT_EQ(f11.GetHealth(), f_copy.GetHealth());
    EXPECT_NE(&f11, &f_copy);
}

TEST(Fighter, MoveConstructor)
{
    auto f00 = Fighter(0);
    auto f_move(std::move(f00));

    EXPECT_EQ(f_move.GetRole(), ROLE_HERO);
    EXPECT_EQ(f_move.GetHealth(), HEALTH_HERO);
}

TEST(Fighter, CopyAssignmentOperator)
{
    auto f11 = Fighter(0);
    f11 = f11; // NOLINT copy on myself should return *this
    Fighter f_copy;
    f_copy = f11;

    EXPECT_EQ(f_copy.GetRole(), ROLE_HERO);
    EXPECT_EQ(f_copy.GetHealth(), HEALTH_HERO);
    EXPECT_NE(&f11, &f_copy);
}

TEST(Fighter, MoveAssignmentOperator)
{
    auto f00 = Fighter(0);
    Fighter f_move;
    f_move = std::move(f00);

    EXPECT_EQ(f_move.GetRole(), ROLE_HERO);
    EXPECT_EQ(f_move.GetHealth(), HEALTH_HERO);
}

TEST(Fighter, SetterMethods)
{
    Fighter f11; //undefined role and health points

    f11.SetRole(ROLE_DRAGON); f11.SetHealth(HEALTH_DEAD);

    EXPECT_EQ(f11.GetRole(), ROLE_DRAGON);
    EXPECT_EQ(f11.GetHealth(), HEALTH_DEAD);

    f11.SetRole(ROLE_ORC); f11.SetHealth(HEALTH_ORC);
    EXPECT_EQ(f11.GetRole(), ROLE_ORC);
    EXPECT_EQ(f11.GetHealth(), HEALTH_ORC);

    f11.SetRole(-2);
    EXPECT_EQ(f11.GetRole(), ROLE_UNDEFINED);
    EXPECT_EQ(f11.GetHealth(), HEALTH_ORC);

    f11.SetRole(2);
    EXPECT_EQ(f11.GetRole(), ROLE_DRAGON);
    EXPECT_EQ(f11.GetHealth(), HEALTH_ORC);
}

TEST(Fighter, IsAlive)
{
    const auto f11 = Fighter(ROLE_HERO);
    const Fighter f22; //undefined

    EXPECT_EQ(f11.IsAlive(), true);
    EXPECT_EQ(f22.IsAlive(), false);
}

TEST(Fighter, IsEnemy)
{
    const auto f11 = Fighter(ROLE_HERO);
    const auto ff1 = Fighter(ROLE_HERO);
    const auto f12 = Fighter(ROLE_ORC);
    const auto f22 = Fighter(ROLE_DRAGON);
    const Fighter fu1; //undefined

    EXPECT_EQ(f11.IsEnemy(ff1), false);
    EXPECT_EQ(f11.IsEnemy(fu1), false);
    EXPECT_EQ(f11.IsEnemy(f12), true);
    EXPECT_EQ(f11.IsEnemy(f22), true);
    EXPECT_EQ(f12.IsEnemy(fu1), false);
    EXPECT_EQ(f22.IsEnemy(fu1), false);
    EXPECT_EQ(f12.IsEnemy(f22), false);
}

TEST(Fighter, CanAttack)
{
    const auto f_1 = Fighter(ROLE_HERO);
    const auto ff_1 = Fighter(ROLE_HERO);
    const auto f1_1 = Fighter(ROLE_ORC);
    const auto f2_1 = Fighter(ROLE_DRAGON);
    const Fighter fu_1; //undefined

    EXPECT_EQ(f_1.CanAttack(ff_1), false);
    EXPECT_EQ(f_1.CanAttack(fu_1), false);
    EXPECT_EQ(f_1.CanAttack(f1_1), true);
    EXPECT_EQ(f_1.CanAttack(f2_1), true);
    EXPECT_EQ(f1_1.CanAttack(fu_1), false);
    EXPECT_EQ(f2_1.CanAttack(fu_1), false);
    EXPECT_EQ(f1_1.CanAttack(f2_1), false);
}

TEST(Fighter, Reset)
{
    auto f_1 = Fighter(ROLE_HERO);
    f_1.Reset();

    EXPECT_EQ(f_1.GetHealth(), HEALTH_UNDEFINED);
    EXPECT_EQ(f_1.GetRole(), ROLE_UNDEFINED);
}

TEST(Fighter, RoleToString)
{
    auto fu_1 = Fighter(-1);
    auto fh_1 = Fighter(0);
    auto fo_1 = Fighter(1);
    auto fd_1 = Fighter(2);

    EXPECT_EQ(std::strcmp(fu_1.RoleToString(), "Undefined"), 0);
    EXPECT_EQ(std::strcmp(fh_1.RoleToString(), "Hero"), 0);
    EXPECT_EQ(std::strcmp(fo_1.RoleToString(), "Orc"), 0);
    EXPECT_EQ(std::strcmp(fd_1.RoleToString(), "Dragon"), 0);
}

TEST(Fighter, IntToRole)
{
    testing::internal::CaptureStdout();
    const Fighter f_1;

    EXPECT_EQ(f_1.IntToRole(-2), ROLE_UNDEFINED);
    EXPECT_EQ(f_1.IntToRole(-1), ROLE_UNDEFINED);
    EXPECT_EQ(f_1.IntToRole(0),  ROLE_HERO);
    EXPECT_EQ(f_1.IntToRole(1),  ROLE_ORC);
    EXPECT_EQ(f_1.IntToRole(2),  ROLE_DRAGON);

    EXPECT_EXIT(
        f_1.IntToRole(3), 
        testing::ExitedWithCode(EXIT_FAILURE),
        "" // I don't care about the actual error message beeing generated
    );
    testing::internal::GetCapturedStdout();
}

TEST(Fighter, Print){
    auto f11 = Fighter(ROLE::ROLE_HERO);
    f11.SetHealth(START_HEALTH::HEALTH_DEAD);
    //auto f2 = Fighter(ROLE::ROLE_DRAGON);

    testing::internal::CaptureStdout();
    f11.Print();
    auto out = testing::internal::GetCapturedStdout();
    std::string expected{"Fighter information:\n\tRole: 'Hero'\n\t"}; // NOLINT
    expected += "Remaining health: 0\033[31m  -->  Fighter death!!!\033[0m\n";
    EXPECT_EQ(out, expected);

    testing::internal::CaptureStdout();
    auto f22 = Fighter(ROLE::ROLE_UNDEFINED);
    f22.Print();
    out = testing::internal::GetCapturedStdout();
    expected = "Fighter information:\n\tRole: 'Undefined'\n\t";
    expected += "Remaining health: -1";
    expected += "\033[31m  -->  Fighter not initialized\033[0m\n";
    EXPECT_EQ(out, expected);
}


TEST(Fighter, Attack)
{
    testing::internal::CaptureStdout();

    const auto fh_1 = Fighter(ROLE_HERO);
    auto fo_1 = Fighter(ROLE_ORC);

    fh_1.Attack(fo_1);
    const std::string term_out = testing::internal::GetCapturedStdout();

    // Clang-tidy would complain about initializing a string without explicitly 
    // passing all arguments, including the default ones. 
    // In case of std::string, the default template argument 
    // is the allocator for string: std::allocator<char>(). Here is the 
    // complete declaration that clang-tidy would like to see:
    // std::string to_test = {
    //     "\033[32mHero hits Orc. \n\033[0m",
    //     std::allocator<char>()
    // }
    std::string to_test = "\033[32mHero hits Orc. \n\033[0m"; // NOLINT

    EXPECT_EQ(term_out, to_test);

    //TODO(Godel): check an attack which can not occur ==> no terminal output
}

TEST(Hero, Attack)
{
    testing::internal::CaptureStdout();

    Hero h1_1 = Hero(ROLE_HERO);
    Hero h2_1 = Hero(ROLE_HERO);
    auto orc = Monster(ROLE_ORC);
    auto dragon = Monster(ROLE_DRAGON);

    h1_1.Attack(orc); h1_1.Attack(dragon); h1_1.Attack(h2_1);

    EXPECT_EQ(orc.GetHealth(), HEALTH_ORC - 2);
    EXPECT_EQ(dragon.GetHealth(), HEALTH_DRAGON - 2);
    EXPECT_EQ(h2_1.GetHealth(), HEALTH_HERO); // no attack

    h1_1.SetHealth(HEALTH_DEAD);
    h1_1.Attack(orc);

    EXPECT_EQ(orc.GetHealth(), HEALTH_ORC - 2); // dead fighter can't attack
    testing::internal::GetCapturedStdout();
}


TEST(Monster, Attack)
{
    testing::internal::CaptureStdout();

    Hero h_1 = Hero(ROLE_HERO);
    auto orc = Monster(ROLE_ORC);
    auto dragon = Monster(ROLE_DRAGON);

    orc.Attack(dragon); dragon.Attack(orc);

    EXPECT_EQ(orc.GetHealth(), static_cast<int>(HEALTH_ORC) ); //no attack
    EXPECT_EQ(dragon.GetHealth(), static_cast<int>(HEALTH_DRAGON) ); //no attack

    orc.Attack(h_1);
    EXPECT_EQ(h_1.GetHealth(), HEALTH_HERO - 1);

    dragon.Attack(h_1);
    EXPECT_EQ(h_1.GetHealth(), HEALTH_HERO - 4);


    orc.SetHealth(HEALTH_DEAD); 
    dragon.SetHealth(HEALTH_DEAD);
    orc.Attack(h_1); dragon.Attack(h_1);

    EXPECT_EQ(h_1.GetHealth(), HEALTH_HERO - 4); // dead fighter can't attack

    const Monster m1_1 = Monster(ROLE_ORC);
    m1_1.Attack(h_1);
    //flushes and releases the captured stdout to avoid segfaults
    testing::internal::GetCapturedStdout(); 
}



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}