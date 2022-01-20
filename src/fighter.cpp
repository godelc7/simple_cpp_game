#include "fighter.h"


//-----------------------------------------------------------------------------
//
//  Constructor
//
Fighter::Fighter() noexcept
    : m_role( ROLE::ROLE_UNDEFINED ),
      m_health( START_HEALTH::HEALTH_UNDEFINED )
{
    //std::cout << "\nDefault constructor!!!\n";
}

//-----------------------------------------------------------------------------
//
//  Constructor
//
Fighter::Fighter(const ROLE_t role) noexcept
        : m_role( role )
{
    switch(m_role)
    {
        case ROLE_UNDEFINED: m_health = HEALTH_UNDEFINED; break;
        case ROLE_HERO:      m_health = HEALTH_HERO;      break;
        case ROLE_ORC:       m_health = HEALTH_ORC;       break;
        case ROLE_DRAGON:    m_health = HEALTH_DRAGON;    break;
    }
}


//-----------------------------------------------------------------------------
//
//  Copy Constructor
//
Fighter::Fighter(const Fighter& other) noexcept
        : m_role( other.GetRole() ),
          m_health( other.GetHealth() )
{
    //std::cout << "\nCopy constructor!!!\n";
}


//-----------------------------------------------------------------------------
//
//  move Constructor
//
Fighter::Fighter(Fighter&& other) noexcept
        : m_role( other.GetRole() ),
          m_health( other.GetHealth() )
{
    //std::cout << "\nMove constructor!!!\n";
    other.Reset();
}


//-----------------------------------------------------------------------------
//
//  Copy assignment operator=
//
Fighter& Fighter::operator=(const Fighter& other) noexcept
{
    //std::cout << "\nCopy Assignment Operator!!!\n";
    m_role = other.GetRole();
    m_health = other.GetHealth();
    return *this;
}

//-----------------------------------------------------------------------------
//
//  Move assignment operator=
//
Fighter& Fighter::operator=(Fighter&& other) noexcept
{
    //std::cout << "\nMove Assignment Operator!!!\n";
    m_role = other.GetRole() ;
    m_health = other.GetHealth();
    other.Reset();
    return *this;
}


//-----------------------------------------------------------------------------
//
//  Fighter::Reset()
//
void Fighter::Reset() noexcept
{
    m_role = ROLE_UNDEFINED;
    m_health = static_cast<int>(HEALTH_UNDEFINED);
}


//-----------------------------------------------------------------------------
//
//  Fighter::RoleToString
//
const char* Fighter::RoleToString() const noexcept
{
    switch(m_role)
    {
        case ROLE_UNDEFINED: return "Undefined";
        case ROLE_HERO:      return "Hero";
        case ROLE_ORC:       return "Orc";
        case ROLE_DRAGON:    return "Dragon";
        default:  std::cout << "\033[31m\nError in file '" << __FILE__ << "' line " << __LINE__
                            << ": unknown fighter role!\n\033[0m";
                  exit( EXIT_FAILURE );
    }
}


//-----------------------------------------------------------------------------
//
//  Fighter::IntToRole
//
ROLE_t Fighter::IntToRole(const int role) noexcept
{
    if(role < -1)
        return ROLE_UNDEFINED;

    switch(role)
    {
        case -1: return ROLE_UNDEFINED;
        case 0:  return ROLE_HERO;
        case 1:  return ROLE_ORC;
        case 2:  return ROLE_DRAGON;
        default: std::cout << "\033[31m\nError in file '" << __FILE__ << "' line " << __LINE__
                            << ": unknown fighter role!\n\033[0m";
                 exit( EXIT_FAILURE );
    }
}


//-----------------------------------------------------------------------------
//
//  Fighter::Print()
//
void Fighter::Print() const noexcept
{
    std::cout << "Fighter information:\n\tRole: '" << this->RoleToString()
              << "'\n\tRemaining health: " << this->GetHealth(); // << std::endl;

    if( this->GetHealth() == HEALTH_DEAD )
        std::cout << "\033[31m  -->  Fighter death!!!\033[0m";
    if(this->m_role == ROLE_UNDEFINED || this->m_health < 0)
        std::cout << "\033[31m  -->  Fighter not initialized\033[0m";

    std::cout << std::endl;
}


//-----------------------------------------------------------------------------
//
//  Fighter::SetRole()
//
void Fighter::SetRole(const ROLE_t role) noexcept
{
    if(role > ROLE_DRAGON)
    {
        std::cout << "\033[31m\nError in file '" << __FILE__ << "' line " << __LINE__
                  << ": unknown fighter role!\n\033[0m";
        exit( EXIT_FAILURE );
    }

    this->m_role = role <= ROLE_UNDEFINED ? ROLE_UNDEFINED :  role;
}

void Fighter::SetRole(const int role_int) noexcept
{
    if(role_int < -1)
        return SetRole(ROLE_UNDEFINED);
    else
        return SetRole( this->IntToRole(role_int) );

}



//-----------------------------------------------------------------------------
//
//  Fighter::IsEnemy()
//
bool Fighter::IsEnemy(const Fighter& other) const noexcept
{
    if(this->m_role == ROLE_HERO)
        return other.GetRole() == ROLE_ORC || other.GetRole() == ROLE_DRAGON;
    else if(this->m_role == ROLE_ORC || this->m_role == ROLE_DRAGON)
        return other.GetRole() == ROLE_HERO;

    return false; // undefined fighter
}


//-----------------------------------------------------------------------------
//
//  Fighter::CanAttack()
//
bool Fighter::CanAttack(const Fighter& other) const noexcept
{
    return this->IsAlive() && other.IsAlive() && this->IsEnemy(other);
}


//-----------------------------------------------------------------------------
//
//  Fighter::Attack()
//
void Fighter::Attack(Fighter& other) const noexcept
{
    if( this->CanAttack(other) )
    {
        const char *myName( this->RoleToString() ), *enemy_name( other.RoleToString() );
        if(this->GetRole() == ROLE_HERO)
            std::cout  << "\033[32m" << myName << " hits " << enemy_name << ". \n\033[0m";
        else
            std::cout  << "\033[31m" << myName << " hits " << enemy_name << ". \n\033[0m";
    }
}






//=============================================================================
//
//                    Implementations for the class Hero
//
//=============================================================================


//-----------------------------------------------------------------------------
//
//  Hero::Attack()
//
void Hero::Attack(Fighter& other) const noexcept
{
    if( this->CanAttack(other) )
    {
        const char *myName( this->RoleToString() ), *enemy_name( other.RoleToString() );
        int damage(2);
        other.SetHealth( other.GetHealth() - damage );
        std::cout  << "\033[32m" << myName << " hits " << enemy_name << ". "
                   << enemy_name << " health is " << other.GetHealth() << "\n\033[0m";

        if( !other.IsAlive() )
            other.Reset();
    }
}



//=============================================================================
//
//                    Implementations for the class Monster
//
//=============================================================================


//-----------------------------------------------------------------------------
//
//  Monster::Attack()
//
void Monster::Attack(Fighter& other) const noexcept
{
    if( this->CanAttack(other) )
    {
        const char *myName(this->RoleToString()), *enemy_name(other.RoleToString());
        int damage = (this->GetRole() == ROLE_ORC) ? 1 : 3;
        other.SetHealth(other.GetHealth() - damage);
        std::cout << "\033[31m" << myName << " hits " << enemy_name << ". "
                  << enemy_name << " health is " << other.GetHealth() << "\n\033[0m";

        if (!other.IsAlive())
            other.Reset();
    }
}


