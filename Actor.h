#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


class StudentWorld;

class Actor : public GraphObject
{
    public:
    Actor(int im_id, int startX, int startY, int startDir, int depth, bool alive, bool damageable, bool bacterium, StudentWorld* stworld) : GraphObject(im_id, startX, startY, startDir, depth), m_alive(alive), m_damageable(damageable), m_bacterium(bacterium), m_stworld(stworld)
    {
    }
    virtual void doSomething() = 0;
    
    // Is this object edible?
    virtual bool isEdible() const
    {
        return false;
    }
    
    // Does this object block bacterium movement?
    virtual bool blocksBacteriumMovement() const
    {
        return false;
    }
    
    // Does the existence of this object indicate bacterium still alive?
    bool isBacterium(){
        return m_bacterium;
    }
    
    bool isAlive(){
        return m_alive;
    }
    
    // Is this actor damageable by overlap with a weapon?
    bool isDamageable(){
        return m_damageable;
    }
    
    void setDead(){
        m_alive = false;
    }
    
    // Damage an object with the intended amount of damage.
    virtual void damageObject(int damage) = 0;
    
    
    StudentWorld* getPointerToWorld(){
        return m_stworld;
    }
    
    private:
        
        bool m_alive;
        bool m_damageable;
        bool m_bacterium;
        StudentWorld* m_stworld;
};



// start of all the classes with health (Socrates and Bacterium):

class Health : public Actor
{
public:
    Health(int im_id, int x, int y, int dir, int health, bool bacterium, StudentWorld* world): Actor(im_id, x, y, dir, 0, true, true, bacterium, world), m_hp(health)
    {
    }
    virtual void doSomething() = 0;
    void damageObject(int damage);
    
    virtual int soundWhenHurt() const = 0;
    virtual int soundWhenDead() const = 0;
    
    // does the death of this actor cause a score increase? (i.e. Bacterium)
    virtual bool scoreIncrease() const {
        return true;
    }
    
    int getHealth()
    {
        return m_hp;
    }
    void restoreHealth()
    {
        m_hp = 100;
    }
    
private:
    int m_hp;
};

class Socrates : public Health
{
    public:
    Socrates(StudentWorld* world): Health(IID_PLAYER, 0, VIEW_HEIGHT/2, 0, 100, false, world), m_num_spray(20), m_num_flame(5)
    {
    }
    
    virtual int soundWhenHurt() const
    {
        return SOUND_PLAYER_HURT;
    }
    
    virtual int soundWhenDead() const
    {
        return SOUND_PLAYER_DIE;
    }
    
    virtual bool scoreIncrease() const
    {
        return false;
    }
    
    void increaseFlame()
    {
        m_num_flame += 5;
    }
    
    int getSpray()
    {
        return m_num_spray;
    }
    
    int getFlame()
    {
        return m_num_flame;
    }
    
    void doSomething();
    
    private:
    int m_num_spray;
    int m_num_flame;
};

class Bacterium : public Health
{
public:
    Bacterium (int im_id, int x, int y, int health, StudentWorld* world): Health(im_id,x,y,90,health,true,world), m_move_plan(0), m_food_eaten(0)
    {
    }
    virtual void doSomething();
    
    // perform step 2 that is specific to aggressive salmonella; return by default except in aggressive salmonella class
    virtual void doAggressive(bool& skip_step)
    {
        return;
    }
    
    // part of doSomething that is specific to each bacterium type
    virtual void doBacteriumSpecific() = 0;
    
    // Add a new type of the bacterium to the game:
    virtual void addNewOfType(int x, int y) = 0;
    
    virtual int soundWhenHurt() const = 0;
    virtual int soundWhenDead() const = 0;
    
    //How much is socrates damaged by overlap?
    virtual int damageToSoc() const = 0;
    
    int movePlanValue(){
        return m_move_plan;
    }
    
    void decreaseMovePlan(){
        m_move_plan--;
        return;
    }
    
    void increaseMovePlan(int amt){
        m_move_plan += amt;
        return;
    }
    int foodEaten(){
        return m_food_eaten;
    }
    void resetFood(){
        m_food_eaten = 0;
        return;
    }
    void addFood(){
        m_food_eaten++;
        return;
    }
    
private:
    int m_move_plan;
    int m_food_eaten;
};

class Salmonella : public Bacterium
{
public:
    Salmonella(int x, int y, int health, StudentWorld* world): Bacterium(IID_SALMONELLA, x, y, health, world)
    {
    }
    
    virtual void doBacteriumSpecific();
    
    virtual int soundWhenHurt() const
    {
        return SOUND_SALMONELLA_HURT;
    }
    virtual int soundWhenDead() const
    {
        return SOUND_SALMONELLA_DIE;
    }
  
    
};

class RegularSalmonella : public Salmonella
{
public:
    RegularSalmonella(int x, int y, StudentWorld* world): Salmonella(x, y, 4, world)
    {
    }
    
    virtual void addNewOfType(int x, int y); // add new Reg Salmonella
    
    virtual int damageToSoc() const{
        return 1;
    }
};

class AggressiveSalmonella : public Salmonella
{
public:
    AggressiveSalmonella(int x, int y, StudentWorld* world): Salmonella(x, y, 10, world)
    {
    }
    
    virtual void addNewOfType(int x, int y);  // add new Agg Salmonella
    
    virtual int damageToSoc() const{
           return 2;
    }
    
    virtual void doAggressive(bool& skip_step);
};

class EColi : public Bacterium
{
public:
    EColi (int x, int y, StudentWorld* world): Bacterium (IID_ECOLI, x, y, 5, world)
    {
    }
    
    virtual void doBacteriumSpecific();
    
    virtual void addNewOfType(int x, int y); // add new EColi
    
    virtual int soundWhenHurt() const
    {
        return SOUND_ECOLI_HURT;
    }
    
    virtual int soundWhenDead() const
    {
        return SOUND_ECOLI_DIE;
    }
    
    virtual int damageToSoc() const{
           return 4;
       }
};

// start of classes with only alive/dead status (everything other than Bacterium and Socrates):

class Binary : public Actor
{
public:
    Binary(int im_id, int x, int y, int dir, bool damageable, bool bacterium, StudentWorld* world): Actor(im_id, x, y, dir, 1, true, damageable, bacterium, world)
    {
    }
    
    virtual void doSomething() = 0;
    void damageObject(int damage); // just set dead
};


class Dirt : public Binary
{
    public:
        Dirt(int x, int y, StudentWorld* world) : Binary (IID_DIRT, x, y, 0, true, false, world)
    {
    }
    
    void doSomething()
    {
        return;
    }
    
    virtual bool blocksBacteriumMovement() const
    {
        return true;
    }
    
    private:
};

class Food : public Binary
{
public:
    Food(int x, int y, StudentWorld* world) : Binary(IID_FOOD, x, y , 90, false, false, world)
    {
    }
    
    void doSomething()
    {
        return;
    }
    
    virtual bool isEdible() const
    {
        return true;
    }

};

// base class for Flame and Spray
class Weapon : public Binary
{
public:
    Weapon(int im_id, int x, int y, int dir, int max_dist, int damage, StudentWorld* world) : Binary(im_id, x, y, dir, false, false, world), m_maxdist(max_dist), m_damage_to_others(damage)
    {
    }
    
    void doSomething();
    void weaponMoveForward();
    
private:
    int m_maxdist;
    int m_damage_to_others;
};

class Flame : public Weapon
{
public:
    Flame(int x, int y, int dir, StudentWorld* world) : Weapon(IID_FLAME, x, y, dir, 32, 5, world)
    {
    }
    
private:
    
};

class Spray : public Weapon
{
public:
    Spray(int x, int y, int dir, StudentWorld* world) : Weapon(IID_SPRAY,x, y, dir, 112, 2, world)
    {
    }
};

// GOODIES:

class Goodie: public Binary
{
public:
    Goodie(int im_id, int x, int y, int points, bool sound, StudentWorld* world);
    
    void doSomething();
    void pickUp(Socrates* s, int points);
    
    // Do the action that is specific to each goodie:
    virtual void doGoodieSpecific(Socrates* s) = 0;
    
private:
    int m_lifetime;
    int m_points_caused;
    bool m_play_sound;
};

class RestoreHealth : public Goodie
{
public:
    RestoreHealth(int x, int y, StudentWorld* world): Goodie(IID_RESTORE_HEALTH_GOODIE, x, y, 250, true, world)
    {
    }
    
    void doGoodieSpecific(Socrates* s);
    
};

class FlamethrowerGoodie : public Goodie
{
public:
    FlamethrowerGoodie(int x, int y, StudentWorld* world): Goodie(IID_FLAME_THROWER_GOODIE, x, y, 300, true, world)
    {
    }
    
    void doGoodieSpecific(Socrates* s);
    
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(int x, int y, StudentWorld* world): Goodie(IID_EXTRA_LIFE_GOODIE, x, y, 500, true, world)
    {
    }
    
    void doGoodieSpecific(Socrates* s);
    
};

class Fungus : public Goodie
{
public:
    Fungus(int x, int y, StudentWorld* world): Goodie(IID_FUNGUS, x, y, -50, false, world)
    {
    }
    
    void doGoodieSpecific(Socrates* s);
    
};

class Pit : public Binary
{
public:
    Pit (int x, int y, StudentWorld* world): Binary(IID_PIT, x, y, 0, false, true, world), m_num_total(10), m_num_regsam(5), m_num_aggsam(3), m_num_ecoli(2)
    {
    }
    
    void doSomething();
    
    
private:
    int m_num_total;
    int m_num_regsam;
    int m_num_aggsam;
    int m_num_ecoli;
};

#endif // ACTOR_H_
