#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <list>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Agent;
class Avatar;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
  virtual void cleanUp();
    bool validWalk(int x, int y) const;
    ~StudentWorld();
    void updateDisplayText();
    bool canMarbleMoveTo(int x, int y) const;
    void createPea(int x, int y, int dir);
    int returnDirection(int dir);
    // Try to cause damage to something at a's location.  (a is only ever
         // going to be a pea.)  Return true if something stops a -- something
         // at this location prevents a pea from continuing.
    bool damageSomething(Actor* a);
    // If a pea were at x,y moving in direction dx,dy, could it hit the
        // player without encountering any obstructions?
    bool existsClearShotToPlayer(int x, int y, int dx, int dy) const;
    // Swallow any swallowable object at a's location.  (a is only ever
         // going to be a pit.)
    bool swallowSwallowable(Actor* a);
    bool isPlayerColocatedWith(int x, int y) const;
    // Are there any crystals left on this level?
    bool anyCrystals() const;

          // Reduce the count of crystals on this level by 1.
    void decCrystals();

   inline void changeLevelCompleted() {
        levelCompleted = true;
    }
    inline bool getLevelCompleted() {
        return levelCompleted;
    }
   inline bool returnCrystalFlag() {
        return crystalFlag;
    }
    // Restore player's health to the full amount.
    void restorePlayerHealth();

          // Increase the amount of ammunition the player has
    void increaseAmmo();
    inline int getLevel() {
        return m_currentLevel;
    }
    bool validRobotWalk(int x, int y);
    // If an item that can be stolen is at x,y, return a pointer to it;
          // otherwise, return a null pointer.  (Stealable items are only ever
          // going be goodies.)
    Actor* getColocatedStealable(int x, int y) const;
    // If a factory is at x,y, how many items of the type that should be
         // counted are in the rectangle bounded by x-distance,y-distance and
         // x+distance,y+distance?  Set count to that number and return true,
         // unless an item is on the factory itself, in which case return false
         // and don't care about count.  (The items counted are only ever going
         // ThiefBots.)
    bool doFactoryCensus(int x, int y, int distance, int& count) const;
    bool thiefFactoryColocation(int x, int y) const;
    void makeThiefBots(int x, int y, int type);

private:
    list<Actor*> m_actors;
    Avatar* m_avatar;
    int m_currentLevel;
    int bonus;
    int crystalCount;
    bool levelCompleted; //flag
    bool crystalFlag;// boolean flag
};

#endif // STUDENTWORLD_H_

