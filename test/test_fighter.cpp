#include "fighter.h"
#include <gtest/gtest.h>

TEST(Fighter, DefaultConstructor) /* NOLINT */
{
    Fighter f;

    EXPECT_EQ(f.GetRole(), ROLE_UNDEFINED);
    EXPECT_EQ(f.GetHealth(), HEALTH_UNDEFINED);
}

TEST(Fighter, ConstructorFromRole) /* NOLINT */
{
    //note: by calling Fighter(int) Fighter(ROLE_t) is also called, so both constructors are tested here
    //note: Getter methods are also tested here
    auto fu = Fighter(-1);
    auto fh = Fighter(0);
    auto fo = Fighter(1);
    auto fd = Fighter(2);

    EXPECT_EQ(fu.GetRole(), ROLE_UNDEFINED);
    EXPECT_EQ(fu.GetHealth(), HEALTH_UNDEFINED);

    EXPECT_EQ(fh.GetRole(), ROLE_HERO);
    EXPECT_EQ(fh.GetHealth(), HEALTH_HERO);

    EXPECT_EQ(fo.GetRole(), ROLE_ORC);
    EXPECT_EQ(fo.GetHealth(), HEALTH_ORC);

    EXPECT_EQ(fd.GetRole(), ROLE_DRAGON);
    EXPECT_EQ(fd.GetHealth(), HEALTH_DRAGON);
}

TEST(Fighter, CopyConstructor) /* NOLINT */
{
    auto f = Fighter(0);
    auto fcopy(f);

    EXPECT_EQ(f.GetRole(), fcopy.GetRole());
    EXPECT_EQ(f.GetHealth(), fcopy.GetHealth());
    EXPECT_NE(&f, &fcopy);
}

TEST(Fighter, MoveConstructor) /* NOLINT */
{
    //auto f = Fighter(0);
    auto fmove( std::move(Fighter(0) ) );

    EXPECT_EQ(fmove.GetRole(), ROLE_HERO);
    EXPECT_EQ(fmove.GetHealth(), HEALTH_HERO);
}

TEST(Fighter, CopyAssignmentOperator) /* NOLINT */
{
    auto f = Fighter(0);
    auto fcopy = f;

    EXPECT_EQ(fcopy.GetRole(), ROLE_HERO);
    EXPECT_EQ(fcopy.GetHealth(), HEALTH_HERO);
    EXPECT_NE(&f, &fcopy);
}

TEST(Fighter, MoveAssignmentOperator) /* NOLINT */
{
    auto fmove = std::move(Fighter(0) );

    EXPECT_EQ(fmove.GetRole(), ROLE_HERO);
    EXPECT_EQ(fmove.GetHealth(), HEALTH_HERO);
}

TEST(Fighter, SetterMethods) /* NOLINT */
{
    Fighter f; //undefined role and health points

    f.SetRole(ROLE_DRAGON); f.SetHealth(HEALTH_DEAD);

    EXPECT_EQ(f.GetRole(), ROLE_DRAGON);
    EXPECT_EQ(f.GetHealth(), HEALTH_DEAD);

    f.SetRole(ROLE_ORC); f.SetHealth(HEALTH_ORC);
    EXPECT_EQ(f.GetRole(), ROLE_ORC);
    EXPECT_EQ(f.GetHealth(), HEALTH_ORC);

    f.SetRole(-2);
    EXPECT_EQ(f.GetRole(), ROLE_UNDEFINED);
    EXPECT_EQ(f.GetHealth(), HEALTH_ORC);

    f.SetRole(2);
    EXPECT_EQ(f.GetRole(), ROLE_DRAGON);
    EXPECT_EQ(f.GetHealth(), HEALTH_ORC);

    //TODO @kamdoum: set the role to any value larger than ROLE_DRAGON in order to trigger an error and catch it
    //f.SetRole(3);
    //EXPECT_EQ(f.GetRole(), ROLE_DRAGON);
    //EXPECT_EQ(f.GetHealth(), HEALTH_ORC);
}

TEST(Fighter, IsAlive) /* NOLINT */
{
    auto f = Fighter(ROLE_HERO);
    Fighter f2; //undefined

    EXPECT_EQ(f.IsAlive(), true);
    EXPECT_EQ(f2.IsAlive(), false);
}

TEST(Fighter, IsEnemy) /* NOLINT */
{
    auto f = Fighter(ROLE_HERO);
    auto ff = Fighter(ROLE_HERO);
    auto f1 = Fighter(ROLE_ORC);
    auto f2 = Fighter(ROLE_DRAGON);
    Fighter fu; //undefined

    EXPECT_EQ(f.IsEnemy(ff), false);
    EXPECT_EQ(f.IsEnemy(fu), false);
    EXPECT_EQ(f.IsEnemy(f1), true);
    EXPECT_EQ(f.IsEnemy(f2), true);
    EXPECT_EQ(f1.IsEnemy(fu), false);
    EXPECT_EQ(f2.IsEnemy(fu), false);
    EXPECT_EQ(f1.IsEnemy(f2), false);
}

TEST(Fighter, CanAttack) /* NOLINT */
{
    auto f = Fighter(ROLE_HERO);
    auto ff = Fighter(ROLE_HERO);
    auto f1 = Fighter(ROLE_ORC);
    auto f2 = Fighter(ROLE_DRAGON);
    Fighter fu; //undefined

    EXPECT_EQ(f.CanAttack(ff), false);
    EXPECT_EQ(f.CanAttack(fu), false);
    EXPECT_EQ(f.CanAttack(f1), true);
    EXPECT_EQ(f.CanAttack(f2), true);
    EXPECT_EQ(f1.CanAttack(fu), false);
    EXPECT_EQ(f2.CanAttack(fu), false);
    EXPECT_EQ(f1.CanAttack(f2), false);
}

TEST(Fighter, Reset) /* NOLINT */
{
    auto f = Fighter(ROLE_HERO);
    f.Reset();

    EXPECT_EQ(f.GetHealth(), HEALTH_UNDEFINED);
    EXPECT_EQ(f.GetRole(), ROLE_UNDEFINED);
}

TEST(Fighter, RoleToString) /* NOLINT */
{
    auto fu = Fighter(-1);
    auto fh = Fighter(0);
    auto fo = Fighter(1);
    auto fd = Fighter(2);

    EXPECT_EQ(fu.RoleToString(), "Undefined");
    EXPECT_EQ(fh.RoleToString(), "Hero");
    EXPECT_EQ(fo.RoleToString(), "Orc");
    EXPECT_EQ(fd.RoleToString(), "Dragon");

    //TODO @kamdoum: check role with any value larger than ROLE_DRAGON in order to trigger an error and catch it
}

TEST(Fighter, IntToRole) /* NOLINT */
{
    Fighter f;

    EXPECT_EQ(f.IntToRole(-2), ROLE_UNDEFINED);
    EXPECT_EQ(f.IntToRole(-1), ROLE_UNDEFINED);
    EXPECT_EQ(f.IntToRole(0),  ROLE_HERO);
    EXPECT_EQ(f.IntToRole(1),  ROLE_ORC);
    EXPECT_EQ(f.IntToRole(2),  ROLE_DRAGON);

    //TODO @kamdoum: check role with any value larger than ROLE_DRAGON in order to trigger an error and catch it
}


TEST(Fighter, Attack) /* NOLINT */
{
    testing::internal::CaptureStdout();

    auto fh = Fighter(ROLE_HERO);
    auto fo = Fighter(ROLE_ORC);

    fh.Attack(fo);
    std::string term_out = testing::internal::GetCapturedStdout();
    std::string to_test = "\033[32mHero hits Orc. \n\033[0m";

    EXPECT_EQ(term_out, to_test);

    //TODO @Kamdoum: check an attack which can not occur ==> no terminal output
}

TEST(Hero, Attack) /* NOLINT */
{
    testing::internal::CaptureStdout();

    Hero h1 = Hero(ROLE_HERO), h2 = Hero(ROLE_HERO);
    auto orc = Monster(ROLE_ORC), dragon = Monster(ROLE_DRAGON);

    h1.Attack(orc); h1.Attack(dragon); h1.Attack(h2);

    EXPECT_EQ(orc.GetHealth(), static_cast<int>(HEALTH_ORC - 2) );
    EXPECT_EQ(dragon.GetHealth(), static_cast<int>(HEALTH_DRAGON - 2) );
    EXPECT_EQ(h2.GetHealth(), static_cast<int>(HEALTH_HERO)); // no attack

    h1.SetHealth(HEALTH_DEAD);
    h1.Attack(orc);

    EXPECT_EQ(orc.GetHealth(), static_cast<int>(HEALTH_ORC - 2) ); // dead fighter can't attack
    std::string term_out = testing::internal::GetCapturedStdout(); //flushes an releases the captured stdout to avoid segfaults
}

TEST(Monster, Attack) /* NOLINT */
{
    testing::internal::CaptureStdout();

    Hero h = Hero(ROLE_HERO);
    auto orc = Monster(ROLE_ORC), dragon = Monster(ROLE_DRAGON);

    orc.Attack(dragon); dragon.Attack(orc);

    EXPECT_EQ(orc.GetHealth(), static_cast<int>(HEALTH_ORC) ); //no attack
    EXPECT_EQ(dragon.GetHealth(), static_cast<int>(HEALTH_DRAGON) ); //no attack

    orc.Attack(h);
    EXPECT_EQ(h.GetHealth(), static_cast<int>(HEALTH_HERO - 1) );

    dragon.Attack(h);
    EXPECT_EQ(h.GetHealth(), static_cast<int>(HEALTH_HERO - 4) );


    orc.SetHealth(HEALTH_DEAD); dragon.SetHealth(HEALTH_DEAD);
    orc.Attack(h); dragon.Attack(h);

    EXPECT_EQ(h.GetHealth(), static_cast<int>(HEALTH_HERO - 4) ); // dead fighter can't attack
    std::string term_out = testing::internal::GetCapturedStdout(); //flushes an releases the captured stdout to avoid segfaults
}



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}