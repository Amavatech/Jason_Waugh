enum buttonMap{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ENTER
};

struct buttonPinout
{
   int UP;
   int DOWN;
   int LEFT;
   int RIGHT;
   int ENTER;
};


class buttonController
{

public:
    buttonPinout buttonPinout_;
    int checkInput();
    void initButtons(int buttonPinout_[]);

};

void buttonController::initButtons(int buttonPinout_[]){
        

           for(int i = 0;i<5;i++){
            pinMode(buttonPinout_[i],INPUT);
           }
          
        
        this->buttonPinout_.UP = buttonPinout_[0];

        this->buttonPinout_.DOWN = buttonPinout_[1];

      this->buttonPinout_.LEFT = buttonPinout_[2];

        this->buttonPinout_.RIGHT = buttonPinout_[3];

        this->buttonPinout_.ENTER = buttonPinout_[4];

       


    
}

int buttonController::checkInput(){
  
    if(digitalRead(this->buttonPinout_.DOWN) == HIGH){
      Serial.println("DOWN");
      return DOWN;
      
      }
      else if(digitalRead(this->buttonPinout_.ENTER) == HIGH){
        Serial.println("ENTER");
      return ENTER;
      }else if(digitalRead(this->buttonPinout_.LEFT) == HIGH){
        Serial.println("LEFT");
      return LEFT;
      }else if(digitalRead(this->buttonPinout_.RIGHT) == HIGH){
        Serial.println("RIGHT");
      return RIGHT;
      }else if(digitalRead(this->buttonPinout_.UP) == HIGH){
        Serial.println("UP");
      return UP;
      }
     
     
    return -1;
}
