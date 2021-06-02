#include <Arduino.h>
#include "EEPROM.h"
struct pastConfig
{
    uint32_t TEMP_SET;
    uint32_t TIME_SET;
};

struct maintConfig
{
    uint32_t BAND_GAP;       // auto geerated ,,, shouldnt be allowed to toggle
      // 1 - 12, 0.1 increments degrees
    int32_t BAND_OFFSET;    //  -2 to 2 in 1 increm -- needs to be handled !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    uint32_t DELAY_TIME;     // 1 - 60, 1 sec
    int32_t TEMP_CALIBRATE; // -10 to 10 in 0.1 increm  -- needs to be handled !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    uint32_t STOP_2;
    uint32_t STOP_4;
    int32_t STOP_6;
    int32_t CALIBRATE2;
};


class ConfigController
{

    
public:
   pastConfig pastConfig_;
   maintConfig maintConfig_;
   void saveConfigEEPROM();
   void loadConfigEEPROM();
};

void ConfigController::saveConfigEEPROM(){
    EEPROM.write(0, this->maintConfig_.BAND_GAP);
    EEPROM.write(1, this->maintConfig_.BAND_OFFSET);
    EEPROM.write(2, this->maintConfig_.DELAY_TIME);
    EEPROM.write(3, this->maintConfig_.TEMP_CALIBRATE);
    EEPROM.write(4, this->maintConfig_.STOP_2);
    EEPROM.write(5, this->maintConfig_.STOP_4);
    EEPROM.write(6, this->maintConfig_.STOP_6);

    EEPROM.write(7, this->pastConfig_.TEMP_SET);
    EEPROM.write(8, this->pastConfig_.TIME_SET);
    EEPROM.write(9,this->maintConfig_.CALIBRATE2);
}
void ConfigController::loadConfigEEPROM(){
    this->maintConfig_.BAND_GAP = EEPROM.read(0);


    this->maintConfig_.BAND_OFFSET = EEPROM.read(1);

    if (this->maintConfig_.BAND_OFFSET >= 100)
    {
        this->maintConfig_.BAND_OFFSET = this->maintConfig_.BAND_OFFSET - 256; // EEPROM - 8 bit uint32_t - config ranges below 0
    }
    

    this->maintConfig_.DELAY_TIME = EEPROM.read(2);
    this->maintConfig_.TEMP_CALIBRATE = EEPROM.read(3);

    if (this->maintConfig_.TEMP_CALIBRATE >= 100)
    {
        this->maintConfig_.TEMP_CALIBRATE = this->maintConfig_.TEMP_CALIBRATE - 256; // EEPROM - 8 bit uint32_t - config ranges below 0
    }


    
    this->maintConfig_.STOP_2 = EEPROM.read(4);
    this->maintConfig_.STOP_4 = EEPROM.read(5);
    this->maintConfig_.STOP_6 = EEPROM.read(6);

    this->pastConfig_.TEMP_SET = EEPROM.read(7);
    this->pastConfig_.TIME_SET = ((uint32_t)EEPROM.read(8))*60;

    this->maintConfig_.CALIBRATE2 = EEPROM.read(9);
    if(this->maintConfig_.CALIBRATE2 > 100){
        this->maintConfig_.CALIBRATE2 = this->maintConfig_.CALIBRATE2 -256;
    }
}
