#include <SPI.h>
#include <Adafruit_MAX31865.h>
#include <Arduino.h>



class SensorController
{
private:
    /* data */
public:
    Adafruit_MAX31865 &soilTempSensor();
    Adafruit_MAX31865 &steamTempSensor();
    int levelInputs[2];
    void initTempSensors(Adafruit_MAX31865 soilTempSensor_,
                         Adafruit_MAX31865 steamTempSensor_);

    float getSoilTemp(Adafruit_MAX31865 tempSensor);
    float getSteamTemp(Adafruit_MAX31865 tempSensor);

    //Level Sensing
    void initLevelSensors(int inputs[]);
    bool getLevelA();
    bool getLevelB();

};
bool SensorController::getLevelA(){
    return digitalRead(32);
}

bool SensorController::getLevelB(){
    return digitalRead(33);
}
void SensorController::initLevelSensors(int inputs[]){
    
    for(int i = 0; i< 2;i++){
        pinMode(inputs[i],INPUT);
        Serial.println(inputs[i]);
        this->levelInputs[i] = inputs[i];
    }
}
Adafruit_MAX31865 &SensorController::soilTempSensor()
{
    static Adafruit_MAX31865 sensor{9, 11, 12, 13}; // ss, mosi, miso, sck
    return sensor;
}
Adafruit_MAX31865 &SensorController::steamTempSensor()
{
    static Adafruit_MAX31865 sensor{10, 11, 12, 13}; // ss, mosi, miso, sck
    return sensor;
}
void SensorController::initTempSensors(Adafruit_MAX31865 cookingTempSensor_, Adafruit_MAX31865 holdingTempSensor_)
{
    cookingTempSensor_.begin(MAX31865_3WIRE);
    holdingTempSensor_.begin(MAX31865_3WIRE);
}
float SensorController::getSoilTemp(Adafruit_MAX31865 tempSensor)
{
    return tempSensor.temperature(100.0, 430.0);
}
float SensorController::getSteamTemp(Adafruit_MAX31865 tempSensor)
{
    return tempSensor.temperature(100.0, 430.0);
}
