
#include <Arduino.h>

class OutputController
{
private:
    
public:
int outputPins[3];
bool outputStates[3];
   void initOutputs(int outputs_[]);
   void standby();
   void turnOn(int element,int output);
   void turnOff(int element,int output);
};

void OutputController::initOutputs(int outputs_[]){
    for(int i = 0; i< 3;i++){
        pinMode(outputs_[i],OUTPUT);
        digitalWrite(outputs_[i],LOW);
        Serial.println(outputs_[i]);
        this->outputPins[i] = outputs_[i];
        this->outputStates[i]=false;
    }
}

void OutputController::standby(){
    for(int i = 0; i < 3 ; i++){
        digitalWrite(this->outputPins[i],LOW);
        this->outputStates[i]=false;
        
    }
}

void OutputController::turnOn(int element,int output){
    digitalWrite(output,HIGH);
    this->outputStates[element]=true;
}
void OutputController::turnOff(int element,int output){
    digitalWrite(output,LOW);
    this->outputStates[element]=false;
}
