#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_level(0), start_new_level(true)
{
}

int StudentWorld::init()
{
    if (start_new_level)
        m_level++; // new level!
    m_soc = new Socrates(this);
    
    // add in pits:
    
    for (int k = 0; k < m_level; k++)
    {
        bool overlap = false;
        int rand_X,rand_Y;
        generateWithinBounds(rand_X,rand_Y);
        
        for (list<Actor*>::iterator p = m_container.begin(); p != m_container.end(); p++)
        {
            if (isOverlap(rand_X,rand_Y,(*p),"reg")) // check if there is overlap with any previously placed objects
            {
                overlap = true;
                break;
            }
        }
        if (overlap) // don't add if it overlaps
            k--;
        else
            m_container.push_back(new Pit(rand_X,rand_Y,this));
    }
    
    // add in food objects:
    
    for (int j = 0; j < min(5 * m_level, 25); j++)
    {
        bool overlap = false;
        int rand_X,rand_Y;
        generateWithinBounds(rand_X, rand_Y);
        for (list<Actor*>::iterator p = m_container.begin(); p != m_container.end(); p++)
        {
            if (isOverlap(rand_X,rand_Y,(*p),"reg")) // check if there is overlap with any previously placed objects
            {
                overlap = true;
                break;
            }
        }
        if (overlap) // don't add if it overlaps
            j--;
        else
            m_container.push_back(new Food(rand_X,rand_Y,this));
    }
    
    //add in dirt piles
    long list_size = m_container.size();
    for (int i = 0; i < max(180 - (20 * m_level), 20); i++)
    {
        bool overlap = false;
        int randX,randY;
        generateWithinBounds(randX,randY);
        int iter = 0;
        for (list<Actor*>::iterator p = m_container.begin(); p != m_container.end(); p++)
        {
            if (iter >= list_size)
                break;
            if (isOverlap(randX,randY,(*p),"reg")) // check if there is overlap with any previously placed objects
            {
                overlap = true;
                break;
            }
            iter++;
        }
        if (overlap) // don't add if it overlaps
            i--;
        else
            m_container.push_back(new Dirt(randX,randY,this));
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_soc-> doSomething(); // let socrates do something
    
    bool bacteriaAlive = false; // to test if there is still bacteria alive
    
    for (list<Actor*>::iterator p = m_container.begin(); p != m_container.end(); p++)
    {
        if ((*p)->isAlive())
        {
     // tell that actor to do something (e.g. move)
            (*p)->doSomething();
            if (!(m_soc->isAlive())) // Socrates is dead!
            {
                decLives();
                start_new_level = false;
                return GWSTATUS_PLAYER_DIED;
            }
            if ((*p)->isBacterium())  // if there is a bacteria alive, keep going
                bacteriaAlive = true;
        }
    }
    
    
    if (!bacteriaAlive) // no more bacteria means we have finished
    {
        start_new_level = true;
        return GWSTATUS_FINISHED_LEVEL;
    }
        
    
    // delete all dead actors
    for (list<Actor*>::iterator p = m_container.begin(); p!= m_container.end(); p++)
    {
        if (!(*p)->isAlive())
        {
            delete (*p);
            m_container.erase(p);
        }
    }
    
    // add in goodies according to chance:
    int ChanceFungus = max(510 - (m_level * 10), 200);
    int rand = randInt(0, ChanceFungus-1);
    if (rand == 0)
    {
        int fung_x, fung_y;
        generateAroundCircum(fung_x, fung_y);
        m_container.push_back(new Fungus(fung_x,fung_y,this));
    }
    int ChanceGoodie = max(510 - (m_level * 10), 250);
    int rand2 = randInt(0, ChanceGoodie -1);
    if (rand2 == 0)
    {
        /*
            A. There is a 60% chance the goodie will be a restore health goodie.
            B. There is a 30% chance the goodie will be a flamethrower goodie.
            C. There is a 10% chance the goodie will be an extra-life goodie.
        */
        int rand3 = randInt(1,10);
        int good_x, good_y;
        generateAroundCircum(good_x,good_y);
        if (rand3 <= 6)
            m_container.push_back (new RestoreHealth(good_x,good_y,this));
        else if (rand3 <= 9)
            m_container.push_back (new FlamethrowerGoodie(good_x,good_y,this));
        else
            m_container.push_back (new ExtraLifeGoodie(good_x,good_y,this));
    }
    
    // set the top line of text:
    setGameStatText("Score: " + to_string(getScore()) + "  Level: " +  to_string(m_level) + "  Lives: " + to_string(getLives()) + "  Health: " + to_string(m_soc->getHealth()) + "  Sprays: " + to_string(m_soc->getSpray())+ "  Flames: " + to_string(m_soc->getFlame()));
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_soc;
    for (list<Actor*>::iterator p = m_container.begin(); p != m_container.end(); p++)
    {
        delete *p;
        m_container.erase(p);
    }
}

StudentWorld::~StudentWorld()
{
    if(!isGameOver() && m_level != 0)
        cleanUp();
}

int StudentWorld::getLevel()
{
    return m_level;
}

void StudentWorld::addActor(Actor* add)
{
    m_container.push_back(add);
}

void StudentWorld::generateWithinBounds(int& x, int& y)
{
    x = randInt(8,248);
    y = randInt(8,248);
    double distance = sqrt(pow((128 - x),2) + pow((128 - y),2));
    while(distance > 120)
    {
        y = randInt(8,248);
        distance = sqrt(pow((128 - x),2) + pow((128 - y),2));
    }
    return;
}

void StudentWorld::generateAroundCircum(int& good_x, int& good_y)
{
    const double PI = 4 * atan(1);
    int rand = randInt(0,359);
    good_x = (VIEW_WIDTH/2 + VIEW_RADIUS * cos(rand*1.0 / 360 * 2 * PI));
    good_y = (VIEW_HEIGHT/2 + VIEW_RADIUS * sin(rand*1.0 / 360 * 2 * PI));
}

bool StudentWorld::getAngleToNearestNearbyEdible(Actor* a, int dist, int& angle) const
{
    int actor_x = a->getX();
    int actor_y = a->getY();
    
    Actor* closest_food = nullptr;
    int lowest_dist = dist;
    for (list<Actor*>::const_iterator p = m_container.begin(); p != m_container.end(); p++)
    {
        if ((*p) != a)
        {
            if((*p)->isEdible())
            {
                int food_x = (*p)->getX();
                int food_y = (*p)->getY();
                double distance = sqrt(pow((food_x - actor_x),2) + pow((food_y - actor_y),2));
                if (distance < lowest_dist) // if distance is smaller than anything before or is smaller than 128, then set this as the new lowest distance and set this food item as the one to find the angle from
                {
                    closest_food = (*p);
                    lowest_dist = dist;
                }
            }
        }
    }
    if (closest_food)
    {
        const double PI = 4 * atan(1);
        double diff_x = closest_food->getX();
        
        double diff_y = closest_food->getY();
        diff_x -= actor_x;
        diff_y -= actor_y;
        angle = 180*atan(diff_y/diff_x)/PI;
        if (diff_x < 0)
            angle += 180;
        return true;
    }
    return false;
}

bool StudentWorld::getAngleToNearbySocrates(Actor* a, int dist, int& angle) const
{
    int soc_x = m_soc->getX();
    int soc_y = m_soc->getY();
    double diff_x = soc_x - a->getX();
    double diff_y = soc_y - a->getY();
    double distance = sqrt(pow(diff_x,2) + pow(diff_y,2));
    if (distance < dist)
    {
        const double PI = 4 * atan(1);
        angle = 180*atan(diff_y/diff_x)/PI;
        if (diff_x < 0)
            angle += 180;
        return true;
    }
    return false;
}

bool StudentWorld::ActOnFirstActable(Actor* curr, int damage)
{
    int curr_x = curr->getX();
    int curr_y = curr->getY();
    for (list<Actor*>::iterator p = m_container.begin(); p != m_container.end(); p++)
    {
        if ((*p) != curr)
        {
            if (damage != 0)
            {
                if ((*p)->isDamageable() && isOverlap(curr_x, curr_y,(*p), "reg"))
                {
                    (*p)->damageObject(damage);
                    return true;
                }
            }
            else // since damage == 0, we are looking for edible objects
            {
                if ((*p)->isEdible() && isOverlap(curr_x, curr_y,(*p), "reg"))
                {
                    (*p)->damageObject(damage);
                    return true;
                }
            }
        }
    }
    return false;
}

bool StudentWorld::isOverlap(double curr_x, double curr_y, Actor* comp, string type) const
{
    double diff_x = comp->getX();
    double diff_y = comp->getY();
    diff_x -= curr_x;
    diff_y -= curr_y;
    double dist = sqrt(pow(diff_x,2) + pow(diff_y,2));
    // check for regular overlap
    if (type == "reg")
    {
        if (dist <= SPRITE_WIDTH)
            return true;
    }
    // check for movement overlap
    else if (type == "move")
    {
        if (dist <= SPRITE_WIDTH/2)
            return true;
    }
    return false;
}

bool StudentWorld::isBacteriumMovementBlockedAt(Actor* a, double x, double y) const
{
    for (list<Actor*>::const_iterator p = m_container.begin(); p != m_container.end(); p++)
    {
        if ((*p) != a)
        {
            if ((*p)->blocksBacteriumMovement() && isOverlap(x,y,(*p),"move"))
                return true;
        }
    }
    return false;
}

Socrates* StudentWorld::socOverlap(int curr_x, int curr_y)
{
    if (isOverlap(curr_x,curr_y,m_soc,"reg"))
        return m_soc;
    return nullptr;
}


