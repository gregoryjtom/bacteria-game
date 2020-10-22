#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Socrates::doSomething()
{
     if (!isAlive())
         return;
     int ch;
    StudentWorld* curr_world = getPointerToWorld();
     if (curr_world->getKey(ch))
     {
         const double PI = 4 * atan(1);
         double x_rel = getX() - 128;
         double y_rel = getY() - 128;
         double pos_angle = 180*atan(y_rel/x_rel)/PI;
         if (x_rel < 0)
             pos_angle += 180;
     // user hit a key during this tick!
         double new_x,new_y;
         switch (ch)
         {
             case KEY_PRESS_LEFT:
             //... move Socrates counterclockwise ...;
             {
                 pos_angle += 5;
                 new_x = VIEW_RADIUS*cos(pos_angle*PI/180)+128;
                 new_y = VIEW_RADIUS*sin(pos_angle*PI/180)+128;
                 moveTo(new_x,new_y);
                 setDirection(getDirection()+5);
                 break;
             }
             case KEY_PRESS_RIGHT:
             //... move Socrates clockwise...;
             {
                 pos_angle -= 5;
                 new_x = VIEW_RADIUS*cos(pos_angle*PI/180)+128;
                 new_y = VIEW_RADIUS*sin(pos_angle*PI/180)+128;
                 moveTo(new_x,new_y);
                 setDirection(getDirection()-5);
                 break;
             }
             case KEY_PRESS_SPACE:
                 /*
                  ADD IN: if no sprays, then do nothing
                  i. Add a spray object SPRITE_WIDTH pixels directly in front of
                  himself (in the same direction Socrates is facing, which is always
                  toward the center of the Petri dish).
                  ii. Socrates’s spray count must decrease by 1.
                  iii. Socrates must play the SOUND_PLAYER_SPRAY sound effect
                  (see the StudentWorld section of this document for details on how
                  to play a sound).
                  */
             {
                 if (m_num_spray == 0)
                     break;
                 double dx, dy;
                 getPositionInThisDirection(getDirection(), SPRITE_WIDTH, dx, dy);
                 Spray* new_spray = new Spray(dx,dy,getDirection(),curr_world);
                 curr_world->addActor(new_spray);
                 curr_world->playSound(SOUND_PLAYER_SPRAY);
                 m_num_spray--;
                 break;
             }
             case KEY_PRESS_ENTER:
             {
                 if (m_num_flame == 0)
                     break;
                 for (int i = 0; i < 16; i++)
                 {
                     double dx, dy;
                     double new_dir = 22*i + getDirection();
                     getPositionInThisDirection(new_dir, SPRITE_WIDTH,dx,dy);
                     Flame* new_flame = new Flame(dx,dy,new_dir,curr_world);
                     curr_world->addActor(new_flame);
                     curr_world->playSound(SOUND_PLAYER_FIRE);
                 }
                 m_num_flame--;
                 break;
             }
             default:
                 break;
         }
    }
    if(m_num_spray < 20)
        m_num_spray++;
    return;
}

void Health::damageObject(int damage)
{
    m_hp -= damage;
    if (m_hp <= 0)
    {
        setDead();
        getPointerToWorld()->playSound(soundWhenDead()); // play respective sound
        if (scoreIncrease())
        {
            getPointerToWorld()->increaseScore(100);
            int rand_food = randInt(1,2);
            if (rand_food == 1)
            {
                Food* new_food = new Food(getX(),getY(),getPointerToWorld());
                getPointerToWorld()->addActor(new_food);
            }
        }
    }
    else
        getPointerToWorld()->playSound(soundWhenHurt()); // play respective sound
    return;
}

void Bacterium::doSomething()
{
    if (!isAlive())
        return;
    
    bool should_skip = false;
    doAggressive(should_skip); // do step 2 of aggressive salmonella
    
    // Damage Socrates if there is overlap with him:
    Socrates* soc = getPointerToWorld()->socOverlap(getX(),getY());
    if (soc)
    {
        soc->damageObject(damageToSoc());
    }
    else
    {
        if (foodEaten() == 3) // multiply if food eaten is equal to 3 and compute appropriate new x and y coordinate for the new Bacterium
        {
            int newx, newy;
            if (getX() < VIEW_WIDTH/2)
                newx = getX() + SPRITE_WIDTH/2;
            else if (getX() > VIEW_WIDTH/2)
                newx = getX() - SPRITE_WIDTH/2;
            else
                newx = getX();
            
            if (getY() < VIEW_WIDTH/2)
                newy = getY() + SPRITE_WIDTH/2;
            else if (getY() > VIEW_WIDTH/2)
                newy = getY() - SPRITE_WIDTH/2;
            else
                newy = getY();
            
            addNewOfType(newx,newy); // adds new of the Bacterium type
            resetFood();
        }
        else
        {
            StudentWorld* world_pointer = getPointerToWorld();
            if (world_pointer->ActOnFirstActable(this, 0)) //check if overlap with food objects and eat it if so, setting that food to dead
            {
                addFood();
            }
        }
    }
    if (!should_skip) // Do the specific actions as long as not skipped as determined by aggressive salmonella action.
        doBacteriumSpecific();
    
    return;
    
}

void RegularSalmonella::addNewOfType(int x, int y)
{
    RegularSalmonella* new_regsam = new RegularSalmonella(x,y,getPointerToWorld());
    getPointerToWorld()->addActor(new_regsam);
}

void AggressiveSalmonella::addNewOfType(int x, int y)
{
    AggressiveSalmonella* new_aggsam = new AggressiveSalmonella(x,y,getPointerToWorld());
    getPointerToWorld()->addActor(new_aggsam);
}

void EColi::addNewOfType(int x, int y)
{
    EColi* new_ecoli = new EColi(x,y,getPointerToWorld());
    getPointerToWorld() -> addActor(new_ecoli);
}

void Salmonella::doBacteriumSpecific()
{
    if (movePlanValue() > 0) // attempt to move in current direction
    {
        decreaseMovePlan();
        double x_dir,y_dir;
        getPositionInThisDirection(getDirection(),3,x_dir,y_dir);
        double dist_from_center = sqrt(pow((128 - x_dir),2) + pow((128 - y_dir),2));
        if (!getPointerToWorld()->isBacteriumMovementBlockedAt(this,x_dir,y_dir) && dist_from_center < VIEW_RADIUS)
        {
            moveAngle(getDirection(),3);
        }
        else
        {
            setDirection(randInt(0,359));
            increaseMovePlan(10);
        }
        return;
    }
    
    // Try to move towards the nearest food item if possible
    int angle_to_food = 0;
    if (getPointerToWorld()->getAngleToNearestNearbyEdible(this, 128, angle_to_food)) // check for nearby edible
    {
        double x_to_food,y_to_food;
        getPositionInThisDirection(angle_to_food,3,x_to_food,y_to_food);
        if (!getPointerToWorld()->isBacteriumMovementBlockedAt(this,x_to_food,y_to_food)) // check for dirt pile
        {
            moveAngle(angle_to_food,3);
        }
        else
        {
            setDirection(randInt(0,359));
            increaseMovePlan(10);
            return;
        }
    }
    else
    {
        setDirection(randInt(0,359));
        increaseMovePlan(10);
        return;
    }
}

void AggressiveSalmonella::doAggressive(bool& skip_step)
{
    int angle_to_socrates = 0;
    if (getPointerToWorld()->getAngleToNearbySocrates(this,72,angle_to_socrates)) // check if within dist of socrates
    {
        skip_step = true; // skip steps 6 and beyond later on
        double x_dir,y_dir;
        getPositionInThisDirection(angle_to_socrates,3,x_dir,y_dir);
        if(!getPointerToWorld()->isBacteriumMovementBlockedAt(this, x_dir, y_dir))
        {
            moveAngle(angle_to_socrates,3);
        }
    }
    return;
}

void EColi::doBacteriumSpecific()
{
    // Step 5: Determine if it can move
    int angle_to_socrates = 0;
    if (getPointerToWorld()->getAngleToNearbySocrates(this,256,angle_to_socrates)) // check if within dist of socrates
    {
        double x_dir,y_dir;
        getPositionInThisDirection(angle_to_socrates,2,x_dir,y_dir);
        double dist_from_center = sqrt(pow((128 - x_dir),2) + pow((128 - y_dir),2));
        for (int i = 0; i < 10; i++) // try ten times to move, increase angle 10 degrees each time
        {
            if(!getPointerToWorld()->isBacteriumMovementBlockedAt(this, x_dir, y_dir) && dist_from_center < VIEW_RADIUS) // check if blocked by dirt and if move is within the bounds
            {
                moveAngle(angle_to_socrates,2); // move 2 pixels in direction
                return;
            }
            else // add 10 degrees to angle and recalculate new coordinates
            {
                angle_to_socrates += 10;
                getPositionInThisDirection(angle_to_socrates,2,x_dir,y_dir);
                dist_from_center = sqrt(pow((128 - x_dir),2) + pow((128 - y_dir),2));
            }
        }
    }
}
void Binary::damageObject(int damage)
{
    setDead(); // no health so just set to dead
    return;
}

void Weapon::weaponMoveForward()
{
    moveForward(SPRITE_WIDTH);
    m_maxdist -= SPRITE_WIDTH;
}

void Weapon::doSomething()
{
    if (!isAlive())
        return;
    
    // note that nullptr is equal to false
    StudentWorld* world_pointer = getPointerToWorld();
    if (world_pointer->ActOnFirstActable(this, m_damage_to_others))
    {
        setDead();
        return;
    }
    
    weaponMoveForward();
    if (m_maxdist == 0)
        setDead();
    return;
    
}

Goodie::Goodie(int im_id, int x, int y, int points, bool sound, StudentWorld* world)
: Binary(im_id, x, y, 0, true, false, world), m_lifetime(max(randInt(0, 300 - 10 * getPointerToWorld()->getLevel() - 1), 50)), m_points_caused(points), m_play_sound(sound)
{
}

void Goodie::doSomething()
{
    if (!isAlive())
        return;
    double pass_x, pass_y;
    pass_x = getX();
    pass_y = getY();
    
    // check if overlap with socrates
    Socrates* soc = getPointerToWorld()->socOverlap(pass_x,pass_y);
    if (soc)
    {
        pickUp(soc,m_points_caused); // do the action that respective Goodie should do
        return;
    }
    m_lifetime--;
    if (m_lifetime <= 0)
        setDead();
    return;
}

void Goodie::pickUp(Socrates* s, int points)
{
    // tell student world to increase points!
    getPointerToWorld()->increaseScore(points);
    setDead();
    // play sound effect!
    if (m_play_sound)
        getPointerToWorld()->playSound(SOUND_GOT_GOODIE);
    // do the specific goodie action (e.g. restore health, add flames)
    doGoodieSpecific(s);
    return;
}

void RestoreHealth::doGoodieSpecific(Socrates* s)
{
    s->restoreHealth();
}

void FlamethrowerGoodie::doGoodieSpecific(Socrates* s)
{
    s->increaseFlame();
}

void ExtraLifeGoodie::doGoodieSpecific(Socrates* s)
{
    getPointerToWorld()->incLives();
}

void Fungus::doGoodieSpecific(Socrates* s)
{
    s->damageObject(20);
}

void Pit::doSomething()
{
    if (m_num_total == 0) // determine whether or not it has emitted all of its bacteria
    {
        setDead();
        return;
    }
    
    int rand = randInt(1,50);
    if (rand == 1) // 1/50 chance
    { // complicated way of ensuring that the type is randomly chosen
        int rand2 = 0;
        if (m_num_regsam != 0 && m_num_aggsam != 0 && m_num_ecoli != 0)
            rand2 = randInt(1,3);
        else if (m_num_aggsam != 0 && m_num_ecoli != 0)
            rand2 = randInt(2,3);
        else if (m_num_regsam != 0 && m_num_aggsam != 0)
            rand2 = randInt(1,2);
        else if (m_num_regsam != 0 && m_num_ecoli != 0)
        {
            int rand3 = randInt(1,2);
            if (rand3 == 1)
                rand2 = 1;
            if (rand3 == 2)
                rand2 = 3;
        }
        else if (m_num_regsam != 0)
            rand2 = 1;
        else if (m_num_aggsam != 0)
            rand2 = 2;
        else
            rand2 = 3;
        
        if (rand2 == 1) // add new regular salmonella
        {
            RegularSalmonella* new_regsam = new RegularSalmonella(getX(),getY(),getPointerToWorld());
            getPointerToWorld()->addActor(new_regsam);
            m_num_regsam--;
            m_num_total--;
            getPointerToWorld()->playSound(SOUND_BACTERIUM_BORN);
            return;
        }
        
        if (rand2 == 2) // add new aggressive salmonella
        {
            AggressiveSalmonella* new_aggsam = new AggressiveSalmonella(getX(),getY(),getPointerToWorld());
            getPointerToWorld()->addActor(new_aggsam);
            m_num_aggsam--;
            m_num_total--;
            getPointerToWorld()->playSound(SOUND_BACTERIUM_BORN);
            return;
        }
        
        if (rand2 == 3) // add new ecoli
        {
            EColi* new_ecoli = new EColi(getX(),getY(),getPointerToWorld()); // change this later for each specific type
            getPointerToWorld()->addActor(new_ecoli);
            m_num_ecoli--;
            m_num_total--;
            getPointerToWorld()->playSound(SOUND_BACTERIUM_BORN);
            return;
        }
         
    }
    return;
}

