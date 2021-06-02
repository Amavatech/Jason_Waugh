#include <Arduino.h>

class CookTimerController
{
private:
    /* data */
public:
    uint32_t cookTimeCounter_;
    uint32_t cookTimeLeft_;
    uint32_t cookStartTime_;
    uint32_t cipTimer;
    bool cookTimerActive = false;
    void startCookTimer(uint32_t SET_TIME);
    bool updateCookTimer(uint32_t SET_TIME);
    bool cipTimerUpdate(int CIPType_, int CIPSelect_, ConfigController ConfigController_);
};
bool CookTimerController::cipTimerUpdate(int CIPType_, int CIPSelect_, ConfigController ConfigController_)
{
    switch (CIPSelect_)
    {
    case 1: // rinse
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP1RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP1STIME_SET)
            {
                return true;
            }
        }
        break;
    case 2: //Sani
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP2RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP2STIME_SET)
            {
                return true;
            }
        }
        break;
    case 3: // rinse
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP3RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP3STIME_SET)
            {
                return true;
            }
        }
        break;
    case 4: //Sani
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP4RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP4STIME_SET)
            {
                return true;
            }
        }
        break;
    case 5: // rinse
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP5RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP5STIME_SET)
            {
                return true;
            }
        }
        break;
    case 6: //Sani
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP6RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP6STIME_SET)
            {
                return true;
            }
        }
        break;
    case 7: // rinse
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP7RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP7STIME_SET)
            {
                return true;
            }
        }
        break;
    case 8: //Sani
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP8RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP8STIME_SET)
            {
                return true;
            }
        }
        break;
    case 9: //Sani
        if (CIPType_ == 1)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP9RTIME_SET)
            {
                return true;
            }
        }
        else if (CIPType_ == 2)
        {
            if (millis() - this->cipTimer >= ConfigController_.cipConfig_.CIP9STIME_SET)
            {
                return true;
            }
        }
        break;
    default:
        break;
    }
    return false;
}
void CookTimerController::startCookTimer(uint32_t SET_TIME)
{
    this->cookTimeCounter_ = millis();
    this->cookStartTime_ = millis();
    this->cookTimeLeft_ = SET_TIME;
    this->cookTimerActive = true;
}

bool CookTimerController::updateCookTimer(uint32_t SET_TIME)
{
    this->cookTimeLeft_ = (SET_TIME * 60000 - ((millis() - this->cookStartTime_))) / 1000; // seconds

    Serial.println(this->cookTimeLeft_ % 60);
    if (millis() - this->cookTimeCounter_ >= (SET_TIME * 60000))
    {
        this->cookTimerActive = false;
        return true;
    }
    return false;
}
