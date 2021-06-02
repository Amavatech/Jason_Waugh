
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

#define WATERFILL_BAGLOADING_VALVE_OUTPUT 40

int CIP_1_VALVES[] = {VALVE_OUTPUT_13, VALVE_OUTPUT_15, VALVE_OUTPUT_16, VALVE_OUTPUT_19};
int CIP_2_VALVES[] = {VALVE_OUTPUT_3, VALVE_OUTPUT_4, VALVE_OUTPUT_5, VALVE_OUTPUT_8, VALVE_OUTPUT_14, VALVE_OUTPUT_15, VALVE_OUTPUT_16};
int CIP_3_VALVES[] = {VALVE_OUTPUT_3, VALVE_OUTPUT_4, VALVE_OUTPUT_8, VALVE_OUTPUT_12};
int CIP_4_VALVES[] = {VALVE_OUTPUT_7, VALVE_OUTPUT_9, VALVE_OUTPUT_13, VALVE_OUTPUT_15};
int CIP_5_VALVES[] = {VALVE_OUTPUT_3, VALVE_OUTPUT_4, VALVE_OUTPUT_6, VALVE_OUTPUT_8, VALVE_OUTPUT_10};
int CIP_6_VALVES[] = {VALVE_OUTPUT_4, VALVE_OUTPUT_9, VALVE_OUTPUT_13, VALVE_OUTPUT_15, VALVE_OUTPUT_18};
int CIP_7_VALVES[] = {VALVE_OUTPUT_12, VALVE_OUTPUT_13, VALVE_OUTPUT_15};
int CIP_8_VALVES[] = {VALVE_OUTPUT_6, VALVE_OUTPUT_10, VALVE_OUTPUT_13, VALVE_OUTPUT_15};
int CIP_9_VALVES[] = {VALVE_OUTPUT_3, VALVE_OUTPUT_4, VALVE_OUTPUT_9, VALVE_OUTPUT_13, VALVE_OUTPUT_15};

struct valveStates
{
    bool V1;
    bool V2;
    bool V3;
    bool V4;
    bool V5;
    bool V6;
    bool V7;
    bool V8;
    bool V9;
    bool V10;
    bool V11;
    bool V12;
    bool V13;
    bool V14;
    bool V15;
    bool V16;
    bool V17;
    bool V18;
    bool V19;
    bool steamValve1;
    bool steamValve2;
};
class GpioController
{

public:
    valveStates valveStates_;
    void closeValve(int pin_, bool &currentValveState_);
    void openValve(int pin_, bool &currentValveState_);
    void standbyValveOutputs();
    void initOutputs();
    void initInputs();
    void initCipPath(int cipSelect);
    void closeCipPath(int cipSelect);
    void drainCip(int cipSelect);
};
void GpioController::closeCipPath(int cipSelect){

}
void GpioController::drainCip(int cipSelect){
  
}

void GpioController::initCipPath(int cipSelect)
{
    // switch (cipSelect)
    // {
    // case 1:
    //     for (int i = 0; i < sizeof(CIP_1_VALVES); i++)
    //     {
    //         this->closeValve(CIP_1_VALVES[i], this->valveStates_[CIP_1_VALVES[i]]);
    //     }
    //     break;
    // case 2:
    //     for (int i = 0; i < sizeof(CIP_2_VALVES); i++)
    //     {
    //         this->closeValve(CIP_2_VALVES[i], this->valveStates_[CIP_2_VALVES[i]]);
    //     }
    //     break;
    // case 3:
    //     for (int i = 0; i < sizeof(CIP_3_VALVES); i++)
    //     {
    //         this->closeValve(CIP_3_VALVES[i], this->valveStates_[CIP_3_VALVES[i]]);
    //     }
    //     break;
    // case 4:
    //     for (int i = 0; i < sizeof(CIP_4_VALVES); i++)
    //     {
    //         this->closeValve(CIP_4_VALVES[i], this->valveStates_[CIP_4_VALVES[i]]);
    //     }
    //     break;
    // case 5:
    //     for (int i = 0; i < sizeof(CIP_5_VALVES); i++)
    //     {
    //         this->closeValve(CIP_5_VALVES[i], this->valveStates_[CIP_5_VALVES[i]]);
    //     }
    //     break;
    // case 6:
    //     for (int i = 0; i < sizeof(CIP_6_VALVES); i++)
    //     {
    //         this->closeValve(CIP_6_VALVES[i], this->valveStates_[CIP_6_VALVES[i]]);
    //     }
    //     break;
    // case 7:
    //     for (int i = 0; i < sizeof(CIP_7_VALVES); i++)
    //     {
    //         this->closeValve(CIP_7_VALVES[i], this->valveStates_[CIP_7_VALVES[i]]);
    //     }
    //     break;
    // case 8:
    //     for (int i = 0; i < sizeof(CIP_8_VALVES); i++)
    //     {
    //         this->closeValve(CIP_8_VALVES[i], this->valveStates_[CIP_8_VALVES[i]]);
    //     }
    //     break;
    // case 9:
    //     for (int i = 0; i < sizeof(CIP_9_VALVES); i++)
    //     {
    //         this->closeValve(CIP_9_VALVES[i], this->valveStates_[CIP_9_VALVES[i]]);
    //     }
    //     break;

    // default:
    //     break;
    // }
}
void GpioController::closeValve(int pin_, bool &currentValveState_)
{
    digitalWrite(pin_, LOW);
    currentValveState_ = false;
}

void GpioController::openValve(int pin_, bool &currentValveState_)
{
    digitalWrite(pin_, HIGH);
    currentValveState_ = true;
}

void GpioController::standbyValveOutputs()
{
    this->closeValve(VALVE_OUTPUT_1, this->valveStates_.V1);                // V16
    this->closeValve(VALVE_OUTPUT_2, this->valveStates_.V2);                // V7
    this->closeValve(VALVE_OUTPUT_3, this->valveStates_.V3);                // V3
    this->closeValve(VALVE_OUTPUT_4, this->valveStates_.V4);                // V4
    this->closeValve(VALVE_OUTPUT_5, this->valveStates_.V5);                // V1
    this->closeValve(VALVE_OUTPUT_6, this->valveStates_.V6);                // V12
    this->closeValve(VALVE_OUTPUT_7, this->valveStates_.V7);                // VSteam 1
    this->closeValve(VALVE_OUTPUT_8, this->valveStates_.V8);                // V6
    this->closeValve(VALVE_OUTPUT_9, this->valveStates_.V9);                // V5
    this->closeValve(VALVE_OUTPUT_10, this->valveStates_.V10);              // V8
    this->closeValve(VALVE_OUTPUT_11, this->valveStates_.V11);              // Steam 2
    this->closeValve(VALVE_OUTPUT_12, this->valveStates_.V12);              // V17
    this->closeValve(VALVE_OUTPUT_13, this->valveStates_.V13);              // V 10
    this->closeValve(VALVE_OUTPUT_14, this->valveStates_.V14);              // V19
    this->closeValve(VALVE_OUTPUT_15, this->valveStates_.V15);              // V11
    this->closeValve(VALVE_OUTPUT_16, this->valveStates_.V16);              // V13
    this->closeValve(VALVE_OUTPUT_17, this->valveStates_.V17);              // V 9
    this->closeValve(VALVE_OUTPUT_18, this->valveStates_.V18);              // V2
    this->closeValve(VALVE_OUTPUT_19, this->valveStates_.V19);              // V15
    this->closeValve(STEAM_VALVE_OUTPUT_1, this->valveStates_.steamValve1); // V14
    this->closeValve(STEAM_VALVE_OUTPUT_2, this->valveStates_.steamValve2); //V18
}
void GpioController::initInputs()
{
    pinMode(SAFETY_PROXY_PIN, INPUT);
    pinMode(HOLDING_LOW_PIN, INPUT);
    pinMode(COOKING_LOW_PIN, INPUT);
    pinMode(HOLDING_HIGH_PIN, INPUT);
}
void GpioController::initOutputs()
{
    pinMode(VALVE_OUTPUT_1, OUTPUT);
    pinMode(VALVE_OUTPUT_2, OUTPUT);
    pinMode(VALVE_OUTPUT_3, OUTPUT);
    pinMode(VALVE_OUTPUT_4, OUTPUT);
    pinMode(VALVE_OUTPUT_5, OUTPUT);
    pinMode(VALVE_OUTPUT_6, OUTPUT);
    pinMode(VALVE_OUTPUT_7, OUTPUT);
    pinMode(VALVE_OUTPUT_8, OUTPUT);
    pinMode(VALVE_OUTPUT_9, OUTPUT);
    pinMode(VALVE_OUTPUT_10, OUTPUT);
    pinMode(VALVE_OUTPUT_11, OUTPUT);
    pinMode(VALVE_OUTPUT_12, OUTPUT);
    pinMode(VALVE_OUTPUT_13, OUTPUT);
    pinMode(VALVE_OUTPUT_14, OUTPUT);
    pinMode(VALVE_OUTPUT_15, OUTPUT);
    pinMode(VALVE_OUTPUT_16, OUTPUT);
    pinMode(VALVE_OUTPUT_17, OUTPUT);
    pinMode(VALVE_OUTPUT_18, OUTPUT);
    pinMode(VALVE_OUTPUT_19, OUTPUT);
    pinMode(STEAM_VALVE_OUTPUT_1, OUTPUT);
    pinMode(STEAM_VALVE_OUTPUT_2, OUTPUT);
}
