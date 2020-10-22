#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Socrates;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    int getLevel();
    void addActor(Actor* add);
    
    // Generate a random coordinate within the bounds of the game.
    void generateWithinBounds(int& x, int& y);
    
    // Generate a coordinate around the circumference of the grid of the game.
    void generateAroundCircum(int& good_x, int& good_y);
    
    // Return true if there is overlap between the given location and the actor that is passed in. Also specify whether checking for regular or movement overlap. Return false otherwise.
    bool isOverlap(double curr_x, double curr_y, Actor* comp, std::string type) const;
    
    // Is bacterium a blocked from moving to the indicated location?
    bool isBacteriumMovementBlockedAt(Actor* a, double x, double y) const;
    
    // Damage/eat the first object that overlaps with the actor and is able to be damaged/eaten. If damage > 0 then the function will damage the first object, if damage == 0, then the function will eat the first edible object. Will return false if cannot do the intended action.
    bool ActOnFirstActable(Actor* curr, int damage);
    
    // Return true if there is a living edible object within the indicated
      // distance from actor a; otherwise false.  If true, angle will be set
      // to the direction from actor a to the edible object nearest to it.
    bool getAngleToNearestNearbyEdible(Actor* a, int dist, int& angle) const;
    
    // Return true if this world's socrates is within the indicated distance
      // of actor a; otherwise false.  If true, angle will be set to the
      // direction from actor a to the socrates.
    bool getAngleToNearbySocrates(Actor* a, int dist, int& angle) const;
    
    // Return the socrates pointer if the coordinates given overlap with socrates. Otherwise return nullptr.
    Socrates* socOverlap(int curr_x, int curr_y);
  
    

private:
    int m_level;
    bool start_new_level;
    Socrates* m_soc;
    std::list<Actor*> m_container;
};

#endif // STUDENTWORLD_H_


