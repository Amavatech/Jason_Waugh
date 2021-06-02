#include "Nextion.h"
uint32_t pageUpdateTimer_ = millis();
NexProgressBar rssi_ = NexProgressBar(1, 13, "j0");

NexPage passwordPage = NexPage(0, 0, "Password");
NexPage maintenencePage = NexPage(0, 0, "Maintenence");
NexPage programPage = NexPage(0, 0, "Program");
NexPage menuPage = NexPage(0, 0, "Menu");
NexPage statusPage = NexPage(0, 0, "Status");
NexPage loadingPage = NexPage(0, 0, "Loading");
NexPage productionPage = NexPage(0, 0, "Production");
NexPage OutputTestPage = NexPage(0, 0, "OutputTest");
NexPage BatchCompletePage = NexPage(0, 0, "CycleComplete");
NexPage StatusPage = NexPage(0, 0, "Status");
NexPage StartUpPage = NexPage(0, 0, "Setup");
NexPage ScaleCaliPage = NexPage(0, 0, "ScaleCal");
//buttons
enum Page
{
    MENU,
    STATUS,
    PRODUCTION,
    LOADING,
    MAINTENENCE,
    TESTING,
    PROGRAM,
    SCALE_CAL,
    PASSWORD
};

NexButton menuOKButton = NexButton(0, 8, "b0");
NexButton passwordOKButton = NexButton(3, 6, "b0");
NexButton passwordCancelButton = NexButton(3, 7, "b1");
NexButton zeroScaleButton = NexButton(6, 4, "b0");
NexButton cancelLoadingButton = NexButton(6, 5, "b2");
NexButton enterButtonProgramPage = NexButton(2, 4, "b0");
NexButton enterMaintenencePageButton = NexButton(4, 24, "b11");
NexButton StatusButton = NexButton(0, 7, "StatusSelect");
NexButton productionPageMenuButton = NexButton(5, 19, "b1");
NexButton productionPageStatusButton = NexButton(5, 20, "b2");
NexButton outputTestValveButton = NexButton(7, 1, "b0");
NexButton outputTestBackButton = NexButton(7, 5, "b1");
NexButton cancelMaintenenceButton = NexButton(4, 23, "b10");
NexButton cancelProgramButton = NexButton(2, 11, "b5");

NexButton ScaleCalInc10 = NexButton(10, 4, "b0");
NexButton ScaleCalDec10 = NexButton(10, 5, "b1");
NexButton ScaleCalInc100 = NexButton(10, 9, "b3");
NexButton ScaleCalDec100 = NexButton(10, 6, "b2");
NexButton ScaleCalInc1000 = NexButton(10, 10, "b4");
NexButton ScaleCalDec1000 = NexButton(10, 11, "b5");
NexButton ScaleCalDone = NexButton(10, 12, "b6");
NexButton ScaleCalCancel = NexButton(10, 13, "b7");
NexButton ScaleCali = NexButton(6, 6, "b1");

NexButton maintenanceBandGapInc_ = NexButton(4, 13, "b2");
NexButton maintenanceBandGapDec_ = NexButton(4, 12, "b1");
NexButton maintenanceWidthInc_ = NexButton(4, 15, "b3");
NexButton maintenanceWidthDec_ = NexButton(4, 14, "b0");
NexButton maintenanceOffsetInc_ = NexButton(4, 17, "b5");
NexButton maintenanceOffsetDec_ = NexButton(4, 16, "b4");
NexButton maintenanceDelayInc_ = NexButton(4, 19, "b7");
NexButton maintenanceDelayDec_ = NexButton(4, 18, "b6");
NexButton maintenanceCalibrateInc_ = NexButton(4, 21, "b9");
NexButton maintenanceCalibrateDec_ = NexButton(4, 20, "b8");

//Text Objects
NexText mode1Select = NexText(0, 1, "Mode1Select");
NexText mode2Select = NexText(0, 2, "Mode2Select");
NexText mode3Select = NexText(0, 3, "Mode3Select");
NexText mode4Select = NexText(0, 5, "Mode4Select");
NexText mode5Select = NexText(0, 4, "Mode5Select");
NexText mode1Marker = NexText(0, 9, "t0");
NexText mode2Marker = NexText(0, 10, "t1");
NexText mode3Marker = NexText(0, 11, "t2");
NexText mode5Marker = NexText(0, 12, "t3");
NexText mode4Marker = NexText(0, 13, "t4");
NexText maintenenceOutputTestButton = NexText(4, 22, "t6");
NexText CurrentModeText = NexText(0, 14, "t5");
NexText statusValveState = NexText(1, 6, "ValveState");
NexText statusPageMenuButton = NexText(1, 7, "t1");
NexText outputTestSteamValveState = NexText(7, 4, "t2");
NexText currentModeStatusPage = NexText(1, 11, "t2");

NexText TempCaliSign = NexText(4, 25, "t7");
NexText OffsetSign = NexText(4, 26, "t8");
//numbers
NexNumber passwordNum1 = NexNumber(3, 1, "n0");
NexNumber passwordNum2 = NexNumber(3, 2, "n1");
NexNumber passwordNum3 = NexNumber(3, 3, "n2");
NexNumber passwordNum4 = NexNumber(3, 4, "n3");
NexNumber TEMP_SET_number = NexNumber(2, 5, "n0");
NexNumber TIME_SET_number = NexNumber(2, 6, "n1");
NexNumber BAND_GAP_number = NexNumber(4, 7, "n0");
NexNumber WIDTH_number = NexNumber(4, 8, "n1");
NexNumber BAND_OFFSET_number = NexNumber(4, 9, "n2");
NexNumber DELAY_TIME_number = NexNumber(4, 10, "n3");
NexNumber TEMP_CALIBRATE_number = NexNumber(4, 11, "n4");
NexNumber weightLoading_number = NexNumber(6, 3, "n2");
NexNumber productionSetTemp = NexNumber(5, 7, "n0");
NexNumber productionTemp = NexNumber(5, 8, "n1");
NexNumber productionWeight = NexNumber(5, 9, "n2");
NexNumber SET_TIME_HOUR = NexNumber(5, 10, "n3");
NexNumber SET_TIME_MINUTE = NexNumber(5, 11, "n4");
NexNumber TIME_LEFT_HOUR = NexNumber(5, 12, "n5");
NexNumber TIME_LEFT_MINUTE = NexNumber(5, 13, "n6");
NexNumber statusProdWeight = NexNumber(1, 8, "n0");
NexNumber statusTempSet = NexNumber(1, 9, "n1");
NexNumber statusProdTemp = NexNumber(1, 10, "n2");
NexNumber completeWeightNumber = NexNumber(8, 3, "n0");
NexNumber scaleCaliWeight = NexNumber(10, 1, "n0");
NexNumber scaleCaliFactor = NexNumber(10, 8, "n1");



NexTouch *nex_listen_list[] = {
    &mode1Select,
    &mode2Select,
    &mode3Select,
    &mode4Select,
    &mode5Select,
    &menuOKButton,
    &passwordOKButton,
    &passwordCancelButton,
    &zeroScaleButton,
    &cancelLoadingButton,
    &enterButtonProgramPage,
    &enterMaintenencePageButton,
    &StatusButton,
    &statusPageMenuButton,
    &productionPageMenuButton,
    &maintenenceOutputTestButton,
    &outputTestValveButton,
    &outputTestBackButton,
    &cancelMaintenenceButton,
    &cancelProgramButton,
    &ScaleCalInc10,
    &ScaleCalDec10,
    &ScaleCalInc100,
    &ScaleCalDec100,
    &ScaleCalInc1000,
    &ScaleCalDec1000,
    &ScaleCalDone,
    &ScaleCalCancel,
    &ScaleCali,
    &maintenanceBandGapInc_,
    &maintenanceBandGapDec_,
    &maintenanceWidthInc_,
    &maintenanceWidthDec_,
    &maintenanceOffsetInc_,
    &maintenanceOffsetDec_,
    &maintenanceDelayInc_,
    &maintenanceDelayDec_,
    &maintenanceCalibrateInc_,
    &maintenanceCalibrateDec_,

    NULL};
class displayController
{

public:
    void displayInit_();
    void updatePasswordPage(int number_);
    void showPage(NexPage nextPage);
    void updateMenu(int currentMode_);
    void updateMaintenencePage(configController configController_);
    void updateProgramPage(configController configController_);
    void updateLoadingPage(float weight_);
    void updateProductionPage(float SET_TIME, timerController timerController_, sensorController sensorController_, int SET_TEMP, int BAND, int offset);
    void updateStatusPage(GPIOController GPIOController_, configController configController_, sensorController sensorController_, operationModes currentMode, int rssi);
    void updateTestPage(GPIOController GPIOController_);
    void updateScaleCal(float weight, int califactor);
    void updatePasswordValue(int number_, bool increase_);
    void updateProgramPage(int programPageSelect_, bool cancel_);
};
void displayController::updatePasswordValue(int number_, bool increase_)
{
    uint32_t number1 = 0;
    uint32_t number2 = 0;
    uint32_t number3 = 0;
    uint32_t number4 = 0;
    passwordNum1.getValue(&number1);
    delay(100);
    passwordNum2.getValue(&number2);
    delay(100);
    passwordNum3.getValue(&number3);
    delay(100);
    passwordNum4.getValue(&number4);

    switch (number_)
    {

    case 1:
        if (increase_)
        {
            passwordNum1.setValue(number1 + 1);
        }
        else
        {
            if (number1 == 0)
            {
            }
            else
            {
                passwordNum1.setValue(number1 - 1);
            }
        }
        break;
    case 2:
        if (increase_)
        {
            passwordNum2.setValue(number2 + 1);
        }
        else
        {
            if (number2 == 0)
            {
            }
            else
            {
                passwordNum2.setValue(number2 - 1);
            }
        }
        break;
    case 3:
        if (increase_)
        {
            passwordNum3.setValue(number3 + 1);
        }
        else
        {
            if (number3 == 0)
            {
            }
            else
            {
                passwordNum3.setValue(number3 - 1);
            }
        }
        break;
    case 4:
        if (increase_)
        {
            passwordNum4.setValue(number4 + 1);
        }
        else
        {
            if (number4 == 0)
            {
            }
            else
            {
                passwordNum4.setValue(number4 - 1);
            }
        }
        break;

    default:
        break;
    }
}
void displayController::updateTestPage(GPIOController GPIOController_)
{
    if (millis() - pageUpdateTimer_ > 1000)
    {
        if (GPIOController_.getValveState() == 1)
        {
            outputTestSteamValveState.setText("OPEN");
        }
        else
        {
            outputTestSteamValveState.setText("CLOSED");
        }
        pageUpdateTimer_ = millis();
    }
}
void displayController::updateStatusPage(GPIOController GPIOController_, configController configController_, sensorController sensorController_, operationModes currentMode, int rssi)
{
    uint32_t prodtemp = sensorController_.readTemp(sensorController_.prodTempSensor());
    uint32_t setTemp = configController_.config_.TEMP_SET;
    uint32_t band = configController_.config_.BAND_GAP;
    const int offset = configController_.config_.TEMP_CALIBRATE;
    int32_t weight = sensorController_.getWeight();
    if (millis() - pageUpdateTimer_ > 2000)
    {
      currentModeStatusPage.Set_font_color_pco(65535);
        int valvestate = GPIOController_.getValveState();
        if (valvestate == 0)
        {
            statusValveState.setText("CLOSED");
        }
        else if (valvestate == 1)
        {
            statusValveState.setText("OPEN");
        }
        if (weight >= 0)
        {
            statusProdWeight.setValue(weight);
        }
        else
        {
            statusProdWeight.setValue(0);
        }

        statusTempSet.setValue(setTemp);
        statusProdTemp.setValue(prodtemp + offset);
        if (prodtemp > (setTemp + band))
        {
            statusProdTemp.Set_background_color_bco(63488);
        }
        else if (prodtemp < (setTemp - band))
        {
            statusProdTemp.Set_background_color_bco(2047);
        }
        else
        {
            statusProdTemp.Set_background_color_bco(3744);
        }
        switch (currentMode)
        {
        case MODE_IDLE:
            currentModeStatusPage.setText("CURRENT MODE: STANDBY");
            currentModeStatusPage.Set_background_color_bco(4425);

            break;
        case MODE_LOADING:

            break;
        case MODE_MAINTENENCE:

            break;
        case MODE_PRODUCTION:
            currentModeStatusPage.setText("CURRENT MODE: PRODUCTION");
            currentModeStatusPage.Set_background_color_bco(4425);
            break;
        case MODE_TESTING:

            break;
        default:
            break;
        }
        rssi_.setValue(rssi);
        if (rssi < 75)
        {
            if (rssi < 50)
            {
                if (rssi < 25)
                {
                    rssi_.Set_font_color_pco(63488);
                }else{
                    rssi_.Set_font_color_pco(65504);
                }
                
            }
            
        }
        else
        {
            rssi_.Set_font_color_pco(1024);
        }
        pageUpdateTimer_ = millis();
    }
}
void displayController::updateProductionPage(float SET_TIME, timerController timerController_, sensorController sensorController_, int SET_TEMP, int BAND, int offset)
{
    uint32_t prodtemp = sensorController_.readTemp(sensorController_.prodTempSensor());

    if (millis() - pageUpdateTimer_ > 1500)
    {
        SET_TIME_HOUR.setValue(SET_TIME / 60);

        SET_TIME_MINUTE.setValue((int)(SET_TIME) % 60);
        TIME_LEFT_HOUR.setValue((timerController_.timeLeft) / 3600);
        TIME_LEFT_MINUTE.setValue((int)(timerController_.timeLeft / 60) % 60);
        int32_t weight = sensorController_.getWeight();
        if(weight<0){
          productionWeight.setValue(0);
        }else{
          productionWeight.setValue(weight);
        }
        
        productionTemp.setValue(sensorController_.readTemp(sensorController_.prodTempSensor()) + offset);
        productionSetTemp.setValue(SET_TEMP);
        pageUpdateTimer_ = millis();

        if (prodtemp > (SET_TEMP + BAND))
        {
            productionTemp.Set_background_color_bco(63488);
        }
        else if (prodtemp < (SET_TEMP - BAND))
        {
            productionTemp.Set_background_color_bco(2047);
        }
        else
        {
            productionTemp.Set_background_color_bco(3744);
        }
    }
}
void displayController::updateProgramPage(configController configController_)
{
    TEMP_SET_number.setValue(configController_.config_.TEMP_SET);
    TIME_SET_number.setValue(configController_.config_.TIME_SET);
}
void displayController::updateMaintenencePage(configController configController_)
{
    const int offset = configController_.config_.TEMP_CALIBRATE;
    const int bandOff = configController_.config_.BAND_OFFSET;
    BAND_GAP_number.setValue(configController_.config_.BAND_GAP);
    WIDTH_number.setValue(configController_.config_.WIDTH);
    
    DELAY_TIME_number.setValue(configController_.config_.DELAY_TIME);

    if (offset < 0)
    {
        TEMP_CALIBRATE_number.setValue((offset) * -1);
        TempCaliSign.setText("-");
    }else{
         TEMP_CALIBRATE_number.setValue(offset);
        TempCaliSign.setText(" ");
    }
    // else
    // {
    //     if (configController_.config_.TEMP_CALIBRATE >= 100 && configController_.config_.TEMP_CALIBRATE <= 1000)
    //     {
    //         TEMP_CALIBRATE_number.setValue((configController_.config_.TEMP_CALIBRATE - 256) * -1);
    //         TempCaliSign.setText("-");
    //     }
    //     else if(configController_.config_.TEMP_CALIBRATE >= 1000)
    //     {
    //         TEMP_CALIBRATE_number.setValue((configController_.config_.TEMP_CALIBRATE - 65536) * -1);
    //         TempCaliSign.setText("-");
    //     }else{
    //         TEMP_CALIBRATE_number.setValue(configController_.config_.TEMP_CALIBRATE);
    //         TempCaliSign.setText(" ");
    //     }
    // }

    if (bandOff < 0)
    {
        BAND_OFFSET_number.setValue((bandOff) * -1);
        OffsetSign.setText("-");
    }else{
        BAND_OFFSET_number.setValue(bandOff);
        OffsetSign.setText(" ");
    }
    // else
    // {
    //     if (configController_.config_.BAND_OFFSET >= 100 && configController_.config_.BAND_OFFSET <= 1000)
    //     {
    //         BAND_OFFSET_number.setValue((configController_.config_.BAND_OFFSET - 256) * -1);
    //         TempCaliSign.setText("-");
    //     }
    //     else if(configController_.config_.BAND_OFFSET >= 1000)
    //     {
    //         BAND_OFFSET_number.setValue((configController_.config_.BAND_OFFSET - 65536) * -1);
    //         TempCaliSign.setText("-");
    //     }else{
    //         BAND_OFFSET_number.setValue(configController_.config_.BAND_OFFSET);
    //         TempCaliSign.setText(" ");
    //     }
    // }


    
}
void displayController::showPage(NexPage nextPage)
{
    nextPage.show();
}
void displayController::displayInit_()
{

    nexInit();

    //
}
void displayController::updateLoadingPage(float weight_)
{
    Serial.println(weight_);
    if (millis() - pageUpdateTimer_ > 500)
    {
        if (weight_ >= 0)
        {
            weightLoading_number.setValue(weight_);
        }
        else
        {
            weightLoading_number.setValue(0);
        }

        pageUpdateTimer_ = millis();
    }
}
void displayController::updateMenu(int currentMode_)
{

    switch (currentMode_)
    {
    case 0:
        CurrentModeText.setText("STANDBY..");
        mode1Select.Set_background_color_bco(3744);
        mode2Select.Set_background_color_bco(65535);
        mode3Select.Set_background_color_bco(65535);
        mode4Select.Set_background_color_bco(65535);
        mode5Select.Set_background_color_bco(65535);
        CurrentModeText.Set_background_color_bco(4425);

        mode1Marker.Set_background_color_bco(3744);
        mode2Marker.Set_background_color_bco(65535);
        mode3Marker.Set_background_color_bco(65535);
        mode4Marker.Set_background_color_bco(65535);
        mode5Marker.Set_background_color_bco(65535);
        break;
    case 1:
        CurrentModeText.setText("MODE_PRODUCTION");
        mode1Select.Set_background_color_bco(65535);
        mode2Select.Set_background_color_bco(3744);
        mode3Select.Set_background_color_bco(65535);
        mode4Select.Set_background_color_bco(65535);
        mode5Select.Set_background_color_bco(65535);
        CurrentModeText.Set_background_color_bco(4425);

        mode1Marker.Set_background_color_bco(65535);
        mode2Marker.Set_background_color_bco(3744);
        mode3Marker.Set_background_color_bco(65535);
        mode4Marker.Set_background_color_bco(65535);
        mode5Marker.Set_background_color_bco(65535);
        break;
    case 2:
        CurrentModeText.setText("MODE_PROGRAM");
        mode1Select.Set_background_color_bco(65535);
        mode2Select.Set_background_color_bco(65535);
        mode3Select.Set_background_color_bco(3744);
        mode4Select.Set_background_color_bco(65535);
        mode5Select.Set_background_color_bco(65535);
        CurrentModeText.Set_background_color_bco(4425);

        mode1Marker.Set_background_color_bco(65535);
        mode2Marker.Set_background_color_bco(65535);
        mode3Marker.Set_background_color_bco(3744);
        mode4Marker.Set_background_color_bco(65535);
        mode5Marker.Set_background_color_bco(65535);
        break;
    case 3:
        CurrentModeText.setText("MODE_MAINTENENCE");
        mode1Select.Set_background_color_bco(65535);
        mode2Select.Set_background_color_bco(65535);
        mode3Select.Set_background_color_bco(65535);
        mode4Select.Set_background_color_bco(3744);
        mode5Select.Set_background_color_bco(65535);
        CurrentModeText.Set_background_color_bco(4425);

        mode1Marker.Set_background_color_bco(65535);
        mode2Marker.Set_background_color_bco(65535);
        mode3Marker.Set_background_color_bco(65535);
        mode4Marker.Set_background_color_bco(3744);
        mode5Marker.Set_background_color_bco(65535);
        break;
    case 4:
        CurrentModeText.setText("MODE_LOADING");
        mode1Select.Set_background_color_bco(65535);
        mode2Select.Set_background_color_bco(65535);
        mode3Select.Set_background_color_bco(65535);
        mode4Select.Set_background_color_bco(65535);
        mode5Select.Set_background_color_bco(3744);
        CurrentModeText.Set_background_color_bco(4425);

        mode1Marker.Set_background_color_bco(65535);
        mode2Marker.Set_background_color_bco(65535);
        mode3Marker.Set_background_color_bco(65535);
        mode4Marker.Set_background_color_bco(65535);
        mode5Marker.Set_background_color_bco(3744);
        break;
    }
}
void displayController::updateScaleCal(float weight, int califactor)
{
    if (millis() - pageUpdateTimer_ > 1000)
    {
        if (weight >= 0)
        {

            scaleCaliFactor.setValue(weight);
        }
        else
        {
            scaleCaliFactor.setValue(0);
        }
        scaleCaliWeight.setValue(califactor);
        pageUpdateTimer_ = millis();
    }
}
void displayController::updatePasswordPage(int number_)
{
    switch (number_)
    {
    case 1:
        passwordNum1.Set_background_color_bco(21502);
        passwordNum2.Set_background_color_bco(65535);
        passwordNum3.Set_background_color_bco(65535);
        passwordNum4.Set_background_color_bco(65535);
        break;
    case 2:
        passwordNum1.Set_background_color_bco(65535);
        passwordNum2.Set_background_color_bco(21502);
        passwordNum3.Set_background_color_bco(65535);
        passwordNum4.Set_background_color_bco(65535);

        break;
    case 3:
        passwordNum1.Set_background_color_bco(65535);
        passwordNum2.Set_background_color_bco(65535);
        passwordNum3.Set_background_color_bco(21502);
        passwordNum4.Set_background_color_bco(65535);
        break;
    case 4:
        passwordNum1.Set_background_color_bco(65535);
        passwordNum2.Set_background_color_bco(65535);
        passwordNum3.Set_background_color_bco(65535);
        passwordNum4.Set_background_color_bco(21502);
        break;

    default:
        break;
    }
}
void displayController::updateProgramPage(int programPageSelect_, bool cancel_)
{
    switch (programPageSelect_)
    {
    case 3:
        TEMP_SET_number.Set_background_color_bco(2047);
        TIME_SET_number.Set_background_color_bco(65535);
        enterButtonProgramPage.Set_background_color_bco(50744);
        cancelProgramButton.Set_background_color_bco(50744);
        break;
    case 2:
        TEMP_SET_number.Set_background_color_bco(65535);
        TIME_SET_number.Set_background_color_bco(2047);
        enterButtonProgramPage.Set_background_color_bco(50744);
        cancelProgramButton.Set_background_color_bco(50744);
        break;
    case 1:
        TEMP_SET_number.Set_background_color_bco(65535);
        TIME_SET_number.Set_background_color_bco(65535);
        if (cancel_)
        {
            enterButtonProgramPage.Set_background_color_bco(50744);
            cancelProgramButton.Set_background_color_bco(2047);
        }
        else
        {
            enterButtonProgramPage.Set_background_color_bco(2047);
            cancelProgramButton.Set_background_color_bco(50744);
        }

        break;

    default:
        break;
    }
}
