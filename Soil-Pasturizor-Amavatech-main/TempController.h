bool DELAY_START1 = false;
uint32_t delayTimer1 = 0;
bool DELAY_START2 = false;
uint32_t delayTimer2 = 0;
bool DELAY_START3 = false;
uint32_t delayTimer3 = 0;
class TempController
{

public:
    void controlTemp(ConfigController &confingController_, OutputController &outputController_, int prodTemp);
};

void TempController::controlTemp(ConfigController &confingController_, OutputController &outputController_, int prodTemp)
{

    int width = confingController_.maintConfig_.BAND_GAP / 2;
    Serial.println(width);
    
    int elecoff1 = (confingController_.pastConfig_.TEMP_SET - confingController_.maintConfig_.STOP_2 + confingController_.maintConfig_.BAND_OFFSET + width);
   // Serial.println(elecoff1);
    int elecoff2 = (confingController_.pastConfig_.TEMP_SET - confingController_.maintConfig_.STOP_4 + confingController_.maintConfig_.BAND_OFFSET + width);
  //  Serial.println(elecoff2);
    int elecoff3 = (confingController_.pastConfig_.TEMP_SET - confingController_.maintConfig_.STOP_6 + confingController_.maintConfig_.BAND_OFFSET + width);
  //  Serial.println(elecoff3);

    int elecon1 = (confingController_.pastConfig_.TEMP_SET-confingController_.maintConfig_.STOP_2 + confingController_.maintConfig_.BAND_OFFSET - width);
   // Serial.println(elecon1);
    int elecon2 = (confingController_.pastConfig_.TEMP_SET-confingController_.maintConfig_.STOP_4 + confingController_.maintConfig_.BAND_OFFSET - width);
   // Serial.println(elecon2);
    int elecon3 = (confingController_.pastConfig_.TEMP_SET-confingController_.maintConfig_.STOP_6 + confingController_.maintConfig_.BAND_OFFSET - width);
  //  Serial.println(elecon3);
    if (prodTemp >= elecoff1)
    {
        
        if (outputController_.outputStates[0] == true && DELAY_START1 == false)
        {

            DELAY_START1 = true;
            delayTimer1 = millis();
        }
        else if (DELAY_START1 == true && (millis() - delayTimer1 >= confingController_.maintConfig_.DELAY_TIME*1000))
        {
            outputController_.turnOff(0, 40);
            
            DELAY_START1 = false;
        }
    }
    else if (prodTemp < elecon1 )
    {
        Serial.println(prodTemp);
        if (outputController_.outputStates[0] == false && DELAY_START1 == false)
        {

            DELAY_START1 = true;
            delayTimer1 = millis();
        }
        else if (DELAY_START1 == true && (millis() - delayTimer1 >= confingController_.maintConfig_.DELAY_TIME*1000))
        {
            outputController_.turnOn(0, 40);
            DELAY_START1 = false;
        }
    }

    if (prodTemp >= elecoff2)
    {
        if (outputController_.outputStates[1] == true && DELAY_START2 == false)
        {

            DELAY_START2 = true;
            delayTimer2 = millis();
        }
        else if (DELAY_START2 == true && (millis() - delayTimer2 >= confingController_.maintConfig_.DELAY_TIME*1000))
        {
            outputController_.turnOff(1, 41);
            DELAY_START2 = false;
        }
    }
    else if (prodTemp < elecon2 )
    {
        if (outputController_.outputStates[1] == false && DELAY_START2 == false)
        {

            DELAY_START2 = true;
            delayTimer2 = millis();
        }
        else if (DELAY_START2 == true && (millis() - delayTimer2 >= confingController_.maintConfig_.DELAY_TIME*1000))
        {
            outputController_.turnOn(1, 41);
            DELAY_START2 = false;
        }
    }

    if (prodTemp >= elecoff3)
    {
        if (outputController_.outputStates[2] == true && DELAY_START3 == false)
        {

            DELAY_START3 = true;
            delayTimer3 = millis();
        }
        else if (DELAY_START3 == true && (millis() - delayTimer3 >= confingController_.maintConfig_.DELAY_TIME*1000))
        {
            outputController_.turnOff(2, 42);
            DELAY_START3 = false;
        }
    }
    else if (prodTemp < elecon3)
    {
        if (outputController_.outputStates[2] == false && DELAY_START3 == false)
        {

            DELAY_START3 = true;
            delayTimer3 = millis();
        }
        else if (DELAY_START3 == true && (millis() - delayTimer3 >= confingController_.maintConfig_.DELAY_TIME*1000))
        {
            outputController_.turnOn(2, 42);
            DELAY_START3 = false;
        }
    }
}
