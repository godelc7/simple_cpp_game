#ifndef FIGHTER_H
#define FIGHTER_H

//#ifdef _OPENMP
//    #include <omp.h>
//#endif

#if defined(__GNUC__) || defined(__clang__)
    #define ATTRIBUTE_NO_DISCARD [[nodiscard]]

    using ROLE_t = enum ROLE {
        ROLE_UNDEFINED = -1,
        ROLE_HERO,
        ROLE_ORC,
        ROLE_DRAGON,
    } ;

    using START_HEALTH_t = enum START_HEALTH {
        HEALTH_UNDEFINED = -1,
        HEALTH_DEAD = 0,
        HEALTH_ORC = 7,
        HEALTH_DRAGON = 20,
        HEALTH_HERO = 40,
    };
#else
    #define ATTRIBUTE_NO_DISCARD

    typedef enum ROLE {
        ROLE_UNDEFINED = -1,
        ROLE_HERO,
        ROLE_ORC,
        ROLE_DRAGON,
    } ROLE_t;

    typedef enum START_HEALTH {
        HEALTH_UNDEFINED = -1,
        HEALTH_DEAD = 0,
        HEALTH_ORC = 7,
        HEALTH_DRAGON = 20,
        HEALTH_HERO = 40,
    } START_HEALTH_t;
#endif


/**
 * @brief Class fighter
 *
 * This class implements a simple fighter. 
 * It is meant to be a base class that other classes can inherit from.
 */
class Fighter {
public:
    /**
     * @brief Default Constructor
     *
     * A constructor for creating a fighter.
     */
    explicit Fighter() noexcept = default;

    /**
     * @brief Constructor from role
     *
     * A constructor for creating a fighter from its role.
     *
     * @param role the role of the fighter to be created
     */
    explicit Fighter(ROLE_t role) noexcept;

    /**
     * @brief Constructor from role
     *
     * A constructor for creating a fighter from its role.
     *
     * @param role_int the role of the fighter to be created
     */
    explicit Fighter(const int role_int) noexcept 
    : Fighter{ IntToRole(role_int) }{};

    /**
     * @brief Copy Constructor
     *
     * A copy constructor for initializing a fighter from another.
     *
     * @param other the fighter from which values are copied
     */
    Fighter(const Fighter& other) noexcept;

    /**
     * @brief Move Constructor
     *
     * A move constructor for initializing a fighter from another.
     * Note that the other is reset after move operation
     *
     * @param other the fighter from which values are moved
     */
    Fighter(Fighter&& other) noexcept;

    //TODO(Godel): make the destructor pure virtual (~Fighter()=0), 
    // so this class can really be an abstract class
    /**
     * @brief The destructor
     */
    virtual ~Fighter()=default;

    /**
     * @brief Operator=
     *
     * The copy assignment operator
     *
     * @param other the fighter to be copied from
     * @return A reference to the new created fighter
     */
    Fighter& operator=(const Fighter& other) noexcept;

    /**
     * @brief Operator=
     *
     * The move assignment operator
     *
     * @param other the fighter to be moved from
     * @return A reference to the new created fighter
     */
    Fighter& operator=(Fighter&& other) noexcept;

    /**
     * @brief A getter
     *
     * Queries the role of the fighter
     *
     * @return The role of the fighter
     */
    ATTRIBUTE_NO_DISCARD inline constexpr ROLE_t GetRole() const noexcept { 
        return this->m_role; 
    }

    /**
     * @brief A getter
     *
     * Queries the actual number of health points for the fighter
     *
     * @return The health points of the fighter
     */
    ATTRIBUTE_NO_DISCARD inline int GetHealth() const noexcept { 
        return this->m_health; 
    }

    /**
     * @brief A setter
     *
     * Set the role of the fighter to a given value
     *
     * @param role the role to set
     */
    void SetRole(ROLE_t role) noexcept;

    /**
     * @brief A setter
     *
     * Set the role of the fighter to a given value
     *
     * @param role_int the role to set
     */
    void SetRole(int role_int) noexcept;


    /**
     * @brief A setter
     *
     * Set the number of health points for the fighter to the given number
     *
     * @param health_points the health points to be set
     */
    inline void SetHealth(const int health_points) noexcept { 
        this->m_health = health_points; 
    }

    /**
     * @brief IsAlive
     *
     * Check if the fighter is alive
     *
     * @return true if the fighter is alive or false otherwise
     */
    ATTRIBUTE_NO_DISCARD inline bool IsAlive() const noexcept{ 
        return this->m_health > HEALTH_DEAD; 
    }

    /**
     * @brief CanAttack
     *
     * Check if the fighter can attack another one
     *
     * @param other the fighter to be checked
     * @return true or false depending on the check if it is an enemy
     */
    ATTRIBUTE_NO_DISCARD bool IsEnemy(const Fighter& other) const noexcept;

    /**
     * @brief CanAttack
     *
     * Check if the fighter can attack another one
     *
     * @param other the fighter to be checked
     * @return true if the other and is alive, or false otherwise
     */
    ATTRIBUTE_NO_DISCARD bool CanAttack(const Fighter& other) const noexcept;

    /**
     * @brief Print
     *
     * Print information about the fighter
     *
     */
    void Print() const noexcept;

//private:
    /**
     * @brief Reset
     *
     * Reset the fighter by setting its role and 
     * health points to undefined values
     */
    void Reset() noexcept;

    /**
     * @brief RoleToString
     *
     * Return the role of fighter as a C string
     *
     * @return The role of the fighter
     */
    ATTRIBUTE_NO_DISCARD const char* RoleToString() const noexcept;

    /**
     * @brief IntToRole
     *
     * Return the role of fighter as a C string
     *
     * @param role the role which corresponding ROLE_t value is required
     * @return The role of the fighter
     */
    static ROLE_t IntToRole(int role) noexcept;

    /**
     * @brief Attack()
     *
     * The main method to attack an enemy
     *
     * @param other the fighter to be attacked
     */
    virtual void Attack(Fighter& other) const noexcept;

private:
    ROLE_t m_role{ROLE::ROLE_UNDEFINED};
    int m_health{START_HEALTH::HEALTH_UNDEFINED};
};



/**
 * @brief class Hero
 *
 * This class implements an Hero controlled by a player of the game.
 */
class Hero : public Fighter {
public:

    using Fighter::Fighter;

    /**
     * @brief The destructor
     */
    //~Hero() override = default;

    /**
     * @brief Attack()
     *
     * The main method to attack an enemy
     *
     * @param other the fighter to be attacked
     */
    void Attack(Fighter& other) const noexcept override;

    //TODO(Godel): implement some kind of protection to avoid an Hero 
    //to be initialized with the role of an orc or dragon
};



/**
 * @brief class Monster
 *
 * This class implements special methods for monsters.
 */
class Monster : public Fighter {
public:

    using Fighter::Fighter;

    /**
     * @brief The destructor
     */
    //~Monster() override = default;

    /**
     * @brief Attack()
     *
     * The main method to attack an enemy
     *
     * @param other the fighter to be attacked
     */
    void Attack(Fighter& other) const noexcept override;

    //TODO(Godel): implement some kind of protection to avoid 
    //a Monster to be initialized with the role of an Hero
};



/*
 * TODO(Godel): implement special classes for Orcs and Dragons. 
                This is not strictly required now, since the only difference
                between them is the time when they are allowed 
                to attack the Hero. But more special features can be added 
                to them in the future.
 */

/**
 * @brief class Orc
 *
 * This class implements special methods for Orcs.
 */
class Orc : public Monster {
public:
    using Monster::Monster;
    /*void Attack(Fighter& other) const noexcept final{ 
        other.Print(); // Not yet implemented
    };*/
};

/**
 * @brief class Dragon
 *
 * This class implements special methods for Dragon.
 */
class Dragon : public Monster {
public:
    using Monster::Monster;
    /*void Attack(Fighter& other) const noexcept final{ 
        other.Print(); // Not yet implemented
    };*/
};


#endif // FIGHTER_H
