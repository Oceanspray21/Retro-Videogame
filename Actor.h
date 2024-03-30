#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject {
public:
    Actor(int imageID, int startX, int startY, StudentWorld* world, int hp, int startDir) : GraphObject(imageID, startX, startY, startDir), m_worldPtr(world), hp(hp){
        setVisible(true);//might have it by default
    }
    bool isActive() const;
    virtual void doSomething() = 0; //pure virutal as no actor can be created and forces all others do make a do somthing which they need
    virtual bool goThroughActor() {
        return false;
    }
    inline virtual bool allowMarble() { //used in inline for the short code
        return false;
    }
    //    virtual bool bePushedBy(Agent* a, int x, int y);
    virtual void beingAttacked(){}
    inline StudentWorld* getWorld() const { //allows us to call the studentworld where it knows where eveyrthing is
        return m_worldPtr;
    }
    inline virtual bool isPushable() {
        return false;
    }
    inline virtual bool opensExit() {
        return false;
    }
    inline virtual int getHp() const {
        return hp;
    }
    inline virtual void setHp(int amt) {
        hp = hp + amt;
    }
    inline virtual void restoreHpPlayer() {
        hp = 20;
    }
    // Can this actor be damaged by peas?
    inline virtual bool isDamageable() const {
        return false;
    }
    // Does this actor stop peas from continuing?
    inline virtual bool stopsPea() const {
        return true;
    }
    inline virtual bool stopsRobotPea() const{
        return true;
    }
    inline virtual bool stopsRobot() const{
        return true;
    }
    inline void setDead() {
        hp = 0;
        setVisible(false); //cant see anymore
    }
    inline virtual bool movingBot() {
        return false;
    }
    inline virtual bool isStealable() const{
        return false;
    }
    inline virtual bool canSteal() const{
        return false;
    }
    inline virtual bool isShootingRobot() const{
        return false;
    }
    inline virtual bool makesRobots() const{
        return false;
    }
    inline virtual int returnScore() {
        return 0;
    }
    inline virtual bool returnThiefPickedUp() {
        return false;
    }
    inline virtual int returnSaveGoodie() {
        return 0;
    }
private:
    StudentWorld* m_worldPtr;
    int hp;
};
class Agent : public Actor {
public:
    Agent(int imageID, int startX, int startY, StudentWorld* world, int hp, int startDir) : Actor(imageID, startX, startY, world, hp, startDir) {
    }
    inline virtual bool canPushMarbles() const {
        return false;
    }
    inline virtual void doSomething(){return;}
    //add more for shooting
    // Can this actor be damaged by peas?
    inline virtual bool isDamageable() const{
        return true;
    }
    inline virtual bool needsClearShot() const {
        return false;
    }
    
};

class Wall: public Actor{
public:
    Wall(int startX, int startY, StudentWorld* world) : Actor(IID_WALL, startX, startY, world, 1000, -1) {
    }
    virtual void doSomething() {
    }
    
};

class Avatar: public Agent {
public:
    Avatar(int startX, int startY, StudentWorld* world) : Agent(IID_PLAYER, startX, startY, world, 20, -1), m_peas(20){
        setDirection(right);
    }
    
    inline int getPeas() const {
        return m_peas;
    }
    inline int reducePeas() {
        return m_peas--;
    }
    inline void setPeas(int amount) {
        m_peas += amount;
    }
    virtual void doSomething();
    inline virtual bool canPushMarbles() const {
        return true;
    }
    
private:
    int m_peas;
};

class Marble: public Actor {
public:
    Marble(int startX, int startY, StudentWorld* world) : Actor(IID_MARBLE, startX, startY, world, 10, -1){}
    virtual void doSomething() {
    }
    virtual bool isActive() const;
    inline virtual void beingAttacked() {
        setHp(-2);
    }
    inline virtual bool isPushable() {
        return true;
    }
    // Can this actor be damaged by peas?
    inline virtual bool isDamageable() const {
        return true;
    }
};
class Peas : public Actor {
public:
    Peas(int startX, int startY, StudentWorld* world, int startDir) : Actor(IID_PEA, startX, startY, world, 1000, startDir), firstIn(false){}
    virtual void doSomething();
    // Does this actor stop peas from continuing?
    inline virtual bool stopsPea() const {
        return false;
    }
private:
    bool firstIn;
};

class Robot : public Agent {
    public :
    Robot(int imageID, int startX, int startY, StudentWorld* world, int hp, int startDir) : Agent(imageID, startX, startY, world, hp, startDir), ticks(0) {}
    inline virtual bool isDamageable() const {
        return true;
    }
    inline virtual bool needsClearShot() const {
        return true;
    }
    inline virtual bool movingBot() {
        return true;
    }
    inline void incTicks() {
        ticks++;
    }
    inline int getTicks() {
        return ticks;
    }
    int tickChecker();
    
private:
   int ticks;
};

class Ragebot : public Robot {
public:
    Ragebot(int startX, int startY, StudentWorld* world, int startDir) : Robot(IID_RAGEBOT, startX, startY, world, 10, startDir) {}
    inline virtual bool isShootingRobot() const {
        return true;
    }
    virtual void doSomething();
};


class ThiefBot : public Robot {
public:
    ThiefBot(int imageID, int startX, int startY, StudentWorld* world, int startDir, int hp)  : Robot(imageID, startX, startY, world, hp, startDir), distanceBeforeTurning(randInt(1, 6)), ThiefPickedUp(false), saveGoodie(0) {}
    virtual void doSomething();
    virtual bool doSpecfic() = 0;
    inline virtual bool canSteal() const{
        return true;
    }
    void triedOtherDirections();
    inline virtual bool returnThiefPickedUp() {
        return ThiefPickedUp;
    }
    inline virtual int returnSaveGoodie() {
        return saveGoodie;
    }
    void thiefBotMove();

private:
    int distanceBeforeTurning;
    bool ThiefPickedUp;
    int saveGoodie;
};

class RegularThiefBot : public ThiefBot {
public:
    RegularThiefBot(int startX, int startY, StudentWorld* world) : ThiefBot(IID_THIEFBOT, startX, startY, world, 0, 5) {}
    inline virtual bool doSpecfic() {
        return false;
    }
};

class MeanThiefBot : public ThiefBot {
public:
    MeanThiefBot(int startX, int startY, StudentWorld* world) : ThiefBot(IID_MEAN_THIEFBOT, startX, startY, world, 0, 8) {}
    inline virtual bool isShootingRobot() const{
        return true;
    }
    virtual bool doSpecfic();
};

class ThiefBotFactory : public Actor {
public:
    ThiefBotFactory(int startX, int startY, StudentWorld* world, int type) : Actor(IID_ROBOT_FACTORY, startX, startY, world, 999, -1), m_type(type), FlagForBots(false) {}
    // Can this actor be damaged by peas?
    inline virtual bool isDamageable() const {
        return false;
    }
    virtual void doSomething();

    inline virtual bool makesRobots() const{
        return true;
    }
    
private:
    int m_type;
    bool FlagForBots;
    };

class Pit : public Actor {
public:
    Pit(int startX, int startY, StudentWorld* world ) : Actor(IID_PIT, startX, startY, world, 1000, -1) {}
    virtual void doSomething();
    inline virtual bool allowMarble() {
        return true;
    }
    inline virtual bool stopsPea() const {
        return false;
    }
    inline virtual bool stopsRobotPea() const{
        return false;
    }
};

class PickupableItem : public Actor
{
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID) : Actor(imageID, startX, startY, world, 1000, -1) {}
    virtual void doSomething();
    inline virtual bool goThroughActor() {
        return true;
    }
    virtual void doSpecificActivity() = 0;
    inline virtual bool stopsPea() const {
        return false;
    }
    inline virtual bool stopsRobotPea() const{
        return false;
    }
    inline virtual bool stopsRobot() const{
        return false;
    }
   
};

class Crystal : public PickupableItem {
public:
    Crystal(int startX, int startY, StudentWorld* world) : PickupableItem(world, startX, startY, IID_CRYSTAL) {}
    virtual void doSpecificActivity();
    inline virtual bool opensExit() {
        return true;
    }
    
};

class Exit : public Actor {
public:
    Exit(int startX, int startY, StudentWorld* world) : Actor(IID_EXIT, startX, startY, world, 1000, -1), exitRevealed(false) {
        setVisible(false);
    }
    inline virtual bool goThroughActor() {
        return true;
    }
    virtual void doSomething();
    inline virtual bool stopsPea() const {
        return false;
    }
private:
    bool exitRevealed;
};

class Goodie : public PickupableItem {
public:
    Goodie(int startX, int startY, StudentWorld* world, int imageID, int score) : PickupableItem(world, startX, startY, imageID), m_score(score) {}
    inline virtual bool isStealable() const {
        return true;
    }
   
    inline virtual int returnScore() {
        return m_score;
    }
    
private:
   int m_score;

};

class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(int startX, int startY, StudentWorld* world, int score) : Goodie(startX, startY, world, IID_EXTRA_LIFE, score) {}
    virtual void doSpecificActivity();
    
};

class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(int startX, int startY, StudentWorld* world, int score) : Goodie(startX, startY, world, IID_RESTORE_HEALTH, score) {}
    virtual void doSpecificActivity();
};

class AmmoGoodie : public Goodie {
public:
    AmmoGoodie(int startX, int startY, StudentWorld* world, int score) : Goodie(startX, startY, world, IID_AMMO, score) {}
    virtual void doSpecificActivity();
};


#endif // ACTOR_H_


