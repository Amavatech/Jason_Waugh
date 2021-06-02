#include <SPI.h>
#include <Adafruit_MAX31865.h>
#include <Arduino.h>
#define SAFETY_PROXY_PIN 50
#define HOLDING_LOW_PIN 49
#define COOKING_LOW_PIN 48
#define HOLDING_HIGH_PIN 47

#define VALVE_OUTPUT_1 40 // switched
#define VALVE_OUTPUT_2 31
#define VALVE_OUTPUT_3 27 //V3
#define VALVE_OUTPUT_4 28 //V4
#define VALVE_OUTPUT_5 25
#define VALVE_OUTPUT_6 36 //V12
#define VALVE_OUTPUT_7 44
#define VALVE_OUTPUT_8 30  //V6
#define VALVE_OUTPUT_9 29  //V5
#define VALVE_OUTPUT_10 32 //V8
#define VALVE_OUTPUT_11 45
#define VALVE_OUTPUT_12 41 //V17
#define VALVE_OUTPUT_13 34 //V10
#define VALVE_OUTPUT_14 43 //V19
#define VALVE_OUTPUT_15 35 //V11
#define VALVE_OUTPUT_16 37 //V13
#define VALVE_OUTPUT_17 33
#define VALVE_OUTPUT_18 26      //V2
#define VALVE_OUTPUT_19 39      //V15
#define STEAM_VALVE_OUTPUT_1 38 //V14
#define STEAM_VALVE_OUTPUT_2 42 //V18

#define PULSE_RATE_COEFFICIENT 23.1

#define WATERFILL_BAGLOADING_VALVE_OUTPUT 40
enum fillType
{
    CIP_FILL,
    BAGLOADING_FILL
};

class SensorController
{

public:
    //Temperature
    Adafruit_MAX31865 &cookingTempSensor();
    Adafruit_MAX31865 &holdingTempSensor();

    void initTempSensors(Adafruit_MAX31865 cookingTempSensor_,
                         Adafruit_MAX31865 holdingTempSensor_);

    float getCookingTemp(Adafruit_MAX31865 tempSensor);
    float getHoldingTemp(Adafruit_MAX31865 tempSensor);

    //Level Sensing
    int getLevelStatus(int pin_);

    bool getProxiSensor();

    // Flow Meter
    void initFlowMeterIntterupt(fillType fillType_);
    void detachFlowIntterupt();
    uint32_t flowMeterPulseCount_;
    bool tallyFillPulse(uint32_t fillVolume_);
};
bool SensorController::tallyFillPulse(uint32_t fillVolume_)
{
    uint32_t waterFilled = this->flowMeterPulseCount_ * PULSE_RATE_COEFFICIENT;
    if (waterFilled >= fillVolume_ * 1000)
    {
        return true;
    }
    return false;
}
void SensorController::initFlowMeterIntterupt(fillType fillType_)
{

    

    switch (fillType_)
    {
    case BAGLOADING_FILL:

        break;

    default:
        break;
    }
}
Adafruit_MAX31865 &SensorController::cookingTempSensor()
{
    static Adafruit_MAX31865 sensor{8, 11, 12, 13}; // ss, mosi, miso, sck
    return sensor;
}
Adafruit_MAX31865 &SensorController::holdingTempSensor()
{
    static Adafruit_MAX31865 sensor{9, 11, 12, 13}; // ss, mosi, miso, sck
    return sensor;
}
void SensorController::initTempSensors(Adafruit_MAX31865 cookingTempSensor_, Adafruit_MAX31865 holdingTempSensor_)
{
    cookingTempSensor_.begin(MAX31865_2WIRE);
    holdingTempSensor_.begin(MAX31865_2WIRE);
}
float SensorController::getCookingTemp(Adafruit_MAX31865 tempSensor)
{
    return tempSensor.temperature(100.0, 430.0);
}
float SensorController::getHoldingTemp(Adafruit_MAX31865 tempSensor)
{
    return tempSensor.temperature(100.0, 430.0);
}
int SensorController::getLevelStatus(int pin_)
{
    return digitalRead(pin_);
}
bool SensorController::getProxiSensor()
{
    return digitalRead(SAFETY_PROXY_PIN);
}

void SensorController::detachFlowIntterupt(){
    detachInterrupt(digitalPinToInterrupt(2));
}
