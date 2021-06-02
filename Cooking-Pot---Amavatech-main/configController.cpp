#include "configController.h"
#include "EEPROM.h"

void configController::saveConfigEEPROM(Config config_){
    EEPROM.write(0, config_.TEMP_SET);
    EEPROM.write(1, config_.BAND_GAP);
    EEPROM.write(2, config_.WIDTH);
    EEPROM.write(3, config_.BAND_OFFSET);
    EEPROM.write(4, config_.DELAY_TIME);
    EEPROM.write(5, config_.TEMP_CALIBRATE);
    EEPROM.write(6, config_.TIME_SET);
    Serial.println("Saving Production Config Data");
}
bool configController::loadConfigEEPROM(){
    this->config_.TEMP_SET= (int)EEPROM.read(0);
    this->config_.BAND_GAP= (int)EEPROM.read(1);
    this->config_.WIDTH= (int)EEPROM.read(2);
    this->config_.BAND_OFFSET= (int)EEPROM.read(3);
    this->config_.DELAY_TIME= (int)EEPROM.read(4);
    this->config_.TEMP_CALIBRATE= (int)EEPROM.read(5);
    this->config_.TIME_SET =(int) EEPROM.read(6);
    Serial.println("Loading Production Config Data");

    if(this->config_.TEMP_CALIBRATE >= 100){
      this->config_.TEMP_CALIBRATE=this->config_.TEMP_CALIBRATE-256;
    }
    if(this->config_.BAND_OFFSET>=100){
      this->config_.BAND_OFFSET=this->config_.BAND_OFFSET-256;
    }
    
    if(this->config_.TEMP_SET < 100 && this->config_.TEMP_SET > 10 ){
        if(this->config_.TIME_SET < 180 &&  this->config_.TIME_SET > 10){
            return true;
        }else{
            return false;
        }
    }
    return false;


    
    
}
void configController::saveDefaultEEPROM(Config config){
    EEPROM.write(0, (uint32_t)config.TEMP_SET);
    EEPROM.write(1, (uint32_t)config.BAND_GAP);
    EEPROM.write(2, (uint32_t)config.WIDTH);
    EEPROM.write(3, (uint32_t)config.BAND_OFFSET);
    EEPROM.write(4, (uint32_t)config.DELAY_TIME);
    EEPROM.write(5, (uint32_t)config.TEMP_CALIBRATE);
    EEPROM.write(6, (uint32_t)config.TIME_SET);
}
