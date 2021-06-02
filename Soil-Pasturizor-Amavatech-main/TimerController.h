class TimerController
{
private:
    /* data */
public:
    uint32_t cookTimeCounter_;
    uint32_t cookTimeLeft;
    uint32_t cookStartTime;
    bool cookTimerActive = false;
    void startTimer(uint32_t SET_TIME);
    bool timerUpdate(uint32_t SET_TIME);
};

void TimerController::startTimer(uint32_t SET_TIME)
{
    this->cookTimeCounter_ = millis();
    this->cookStartTime = millis();
    this->cookTimeLeft = SET_TIME;
    this->cookTimerActive = true;
}

bool TimerController::timerUpdate(uint32_t SET_TIME)
{
    this->cookTimeLeft = (SET_TIME * 1000 - ((millis() - this->cookStartTime))) / 1000; // seconds
    
    if (millis() - this->cookTimeCounter_ >= (SET_TIME * 1000))
    {
        this->cookTimerActive = false;
        return true;
    }
    return false;
}
