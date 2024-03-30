#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

bool Actor::isActive() const { //do smth like requires oilsound
    if( getHp() <= 0) {
        return false;
    }
    return true;
}


bool Marble::isActive() const{
    if (getHp() < 0) {
        return false;
    }
    return true;
}

void Avatar::doSomething() {
    if (getHp() <= 0) {
        return;
    }
    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key this tick!
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                setDirection(left);
                //ask studentworld if there is a actor there if there isnt we move if there is we gotta check if we can go in it
                if (getWorld()->validWalk(getX()-1, getY())) {
                    moveTo(getX()-1, getY());
                }
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getWorld()->validWalk(getX()+1, getY())) {
                    moveTo(getX()+1, getY());
                }
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if (getWorld()->validWalk(getX(), getY()+1)) {
                    moveTo(getX(), getY()+1);
                }
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (getWorld()->validWalk(getX(), getY()-1)) {
                    moveTo(getX(), getY()-1);
                }
                break;
            case KEY_PRESS_ESCAPE:
                setDead();
                getWorld()->decLives();
                break;
            case KEY_PRESS_TAB: //find out what tab and enter is supposed to do
                setDirection(right);
                break;
            case KEY_PRESS_ENTER:
                setDirection(right);
                break;
            case KEY_PRESS_SPACE:
                if (getPeas() > 0) {
                    getWorld()->createPea(this->getX(), this->getY(), getDirection());
                    //how do I decrement pea ammo for player this funcdtion should be in player class right?
                    //what does move if possible do? I get that both robots and player moves but isnt the implemntation diff ones looking for key press. So should this function be in player do smth
                    // why does stopPea is false for pit but it still stops it
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    reducePeas();
                }
                break;
        }
    }
}
void Peas::doSomething() {
    int dir = getDirection();
    if (firstIn) { //stops it from moving the same tick its constructed
        switch (dir) {
            case 0: //right
                this->moveTo(getX()+1, getY()); //moves pea
                break;
            case 90: //up
                this->moveTo(getX(), getY()+1);
                break;
                
            case 270: //down
                this->moveTo(getX(), getY()-1);
                break;
            case 180: //left
                this->moveTo(getX()-1, getY());
                break;
            default:
                break;
        }
    }
    firstIn = true;
    getWorld()->damageSomething(this);
}
void Pit::doSomething() {
    if (getHp()== 0) {
        return;
    }
    getWorld()->swallowSwallowable(this);
    
}


void PickupableItem::doSomething() {
    if (this->getHp() <= 0) {
        return;
    }
    if (getWorld()->isPlayerColocatedWith(this->getX(), this->getY())) {
        doSpecificActivity();
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

void Crystal::doSpecificActivity() {
    getWorld()->increaseScore(50);
    getWorld()->decCrystals();
}

void Exit::doSomething() {
    
    if (!getWorld()->anyCrystals() && getWorld()->returnCrystalFlag() && !exitRevealed) { //i have the flag so the sound isnt constantly made
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        exitRevealed = true;
        
    }
    if (getWorld()->isPlayerColocatedWith(getX(), getY()) && !getWorld()->anyCrystals()) {
        getWorld()->changeLevelCompleted();
        setVisible(false);
    }
}
void ExtraLifeGoodie::doSpecificActivity() {
    getWorld()->increaseScore(returnScore());
    getWorld()->incLives();
}
void RestoreHealthGoodie::doSpecificActivity() {
    getWorld()->increaseScore(returnScore());
    getWorld()->restorePlayerHealth();
}
void AmmoGoodie::doSpecificActivity() {
    getWorld()->increaseScore(returnScore());
    getWorld()->increaseAmmo();
}

int Robot::tickChecker() {
    int ticks = (28 - (getWorld()->getLevel())) / 4;
    // level number (0, 1, 2, etc.)
    if (ticks < 3)
        ticks = 3; // no RageBot moves more frequently than this
    return ticks+1;
}

void Ragebot::doSomething() {
    if (getHp() <= 0) {
        return;
    }
    int dir = getDirection();
    int dx = 0;
    int dy = 0;
    if (getTicks() % tickChecker() == 0) { // if its that specific target tick only whewn its that do u do smth
        incTicks();
        switch (dir) { // do set direction using switch statement for shorter code instead of repeating
            case 0: //right
                dx = 1;
                break;
            case 90: //up
                dy = 1;
                break;
            case 180: //left
                dx = -1;
                break;
            case 270: //down
                dy = -1;
                break;
            default:
                break;
        }
        if (getWorld()->existsClearShotToPlayer(getX(), getY(), dx, dy)) {
            getWorld()->createPea(getX(), getY(), getDirection());
            getWorld()->playSound(SOUND_ENEMY_FIRE);
        }
        else {
            switch (dir) {
                case 0: //right
                    if (getWorld()->validRobotWalk(getX()+1, getY())) {
                        moveTo(getX()+1, getY());
                    }
                    else {
                        setDirection(left);
                    }
                    break;
                case 90: //up
                    if (getWorld()->validRobotWalk(getX(), getY()+1)) {
                        moveTo(getX(), getY()+1);
                    }
                    else {
                        setDirection(down);
                    }
                    break;
                case 180: //left
                    if (getWorld()->validRobotWalk(getX()-1, getY())) {
                        moveTo(getX()-1, getY());
                    }
                    else {
                        setDirection(right);
                    }
                    break;
                case 270: //down
                    if (getWorld()->validRobotWalk(getX(), getY()-1)) {
                        moveTo(getX(), getY()-1);
                    }
                    else {
                        setDirection(up);
                    }
                    break;
                default:
                    break;
            }
        }
        
    }
    incTicks();
}
void ThiefBot::doSomething() {
    if (getHp() <= 0) {
        return;
    }
    bool tryOnce = false;
    if (getTicks() % tickChecker() == 0) { // if its that specific target tick only whewn its that do u do smth
        incTicks();
        Actor* goodie = getWorld()->getColocatedStealable(getX(), getY());
        if (doSpecfic()){ //for the mean theifbot pracitng good oop code instead of repeating all this I make a function for the differnces
        }
        else if (goodie != nullptr) {
            if (getX() == goodie->getX() && getY() == goodie->getY() && !ThiefPickedUp) {
                if (!tryOnce) {
                    tryOnce = true;
                    if (randInt(1, 10) == 1) {
                        saveGoodie = goodie->returnScore(); //check if its saving the corrrect value
                        goodie->setVisible(false);
                        goodie->setDead();
                        getWorld()->playSound(SOUND_ROBOT_MUNCH);
                        ThiefPickedUp = true;
                    }
                    else {
                        thiefBotMove();
                    }
                    
                }
            }
        }
        else {
            thiefBotMove();
        }
    }
    incTicks();
}

void ThiefBot::thiefBotMove() {
    int dir = getDirection();
    switch (dir) {
        case 0: //right
            if (getWorld()->validRobotWalk(getX()+1, getY()) && distanceBeforeTurning > 0) {
                moveTo(getX()+1, getY());
                distanceBeforeTurning--;
            }
            else {
                triedOtherDirections();
            }
            break;
        case 90: //up
            if (getWorld()->validRobotWalk(getX(), getY()+1) && distanceBeforeTurning > 0) {
                moveTo(getX(), getY()+1);
                distanceBeforeTurning--;
                
            }
            else {
                triedOtherDirections(); //pulled it into the seperate function to less repeat codde
            }
            break;
        case 180: //left
            if (getWorld()->validRobotWalk(getX()-1, getY()) && distanceBeforeTurning > 0) {
                moveTo(getX()-1, getY());
                distanceBeforeTurning--;
                
            }
            else {
                triedOtherDirections();
            }
            break;
        case 270: //down
            if (getWorld()->validRobotWalk(getX(), getY()-1) && distanceBeforeTurning > 0) {
                moveTo(getX(), getY()-1);
                distanceBeforeTurning--;
                
            }
            else {
                triedOtherDirections();
            }
            break;
        default:
            break;
    }
    
}
void ThiefBot::triedOtherDirections() {
    distanceBeforeTurning = randInt(1, 6);
    bool triedLeft = false;
    bool triedRight = false;
    bool triedUp = false;
    bool triedDown = false;
    while((triedUp == false) && (triedDown == false) && (triedLeft == false) && (triedRight == false)) { //checks all directions as all needs to be set to true
        int randomDirection = randInt(1, 4);
        switch (randomDirection) {
            case 1: //left
                if (getWorld()->validRobotWalk(getX()-1, getY())) {
                    setDirection(left);
                    moveTo(getX()-1, getY());
                    distanceBeforeTurning--;
                    return;
                }
                triedLeft = true;
                break;
            case 2: //right
                if (getWorld()->validRobotWalk(getX()+1, getY())) {
                    setDirection(right);
                    moveTo(getX()+1, getY());
                    distanceBeforeTurning--;
                    return;
                }
                triedRight = true;
                break;
            case 3: //up
                if (getWorld()->validRobotWalk(getX(), getY()+1)) {
                    setDirection(up);
                    moveTo(getX(), getY()+1);
                    distanceBeforeTurning--;
                    return;
                }
                triedUp = true;
                break;
            case 4:
                if (getWorld()->validRobotWalk(getX(), getY()-1)) {
                    setDirection(down);
                    moveTo(getX(), getY()-1);
                    distanceBeforeTurning--;
                    return;
                }
                triedDown = true;
                break;
            default:
                break;
        }
    }
}

void ThiefBotFactory::doSomething() {
    int count = 0;
    getWorld()->doFactoryCensus(getX(), getY(), 3, count); // number is 3 becasue the distance is a 3x3 grid
    if (count < 3 && !FlagForBots) {
        if(randInt(1, 50) == 1) {
            getWorld()->makeThiefBots(getX(), getY(), m_type);
            FlagForBots = true;
            return;
        }
    }
    if(FlagForBots){
        if (!getWorld()->thiefFactoryColocation(getX(), getY()) && FlagForBots && count < 3) {
            if(randInt(1, 50) == 1) {
                getWorld()->makeThiefBots(getX(), getY(), m_type);
            }
        }
    }
}


bool MeanThiefBot::doSpecfic() {
    int dir = getDirection(); //similar to the ragebot
    int dx = 0;
    int dy = 0;
    switch (dir) {
        case 0: //right
            dx = 1;
            break;
        case 90: //up
            dy = 1;
            break;
        case 180: //left
            dx = -1;
            break;
        case 270: //down
            dy = -1;
            break;
        default:
            break;
    }
    if (getWorld()->existsClearShotToPlayer(getX(), getY(), dx, dy)) {
        getWorld()->createPea(getX(), getY(), getDirection());
        getWorld()->playSound(SOUND_ENEMY_FIRE);
        return true;
    }
    return false;
}
