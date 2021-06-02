#include "Arduino.h"

struct Config{
    int32_t TEMP_SET; // degrees
    int32_t TIME_SET; // minutes
    int32_t BAND_GAP; // Width *2
    int32_t WIDTH; // width in 0.1 increment, but stored as ints - byte values for EEPROM
    int32_t BAND_OFFSET;
    int32_t DELAY_TIME;
    int32_t TEMP_CALIBRATE;
};

class configController
{
private:
   
public:
    
    Config config_;
    void saveConfigEEPROM(Config config_);
    bool loadConfigEEPROM();
    void saveDefaultEEPROM(Config config);
    
};
