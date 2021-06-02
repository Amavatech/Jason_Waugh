
#include "GPIOController.h"
#include "operationModeController.h"
#include "sensorController.h"
#include "timerController.h"
#include "configController.h"
#include "displayController.h"
#include "temperatureController.h"
#include "productionController.h"
#include <SD.h>
#include "buttonController.h"
#include "RTClib.h"
#include <Ethernet.h>
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.amavatech.com"; // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
int rstFlag =0;
// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true; // set to false for better speed measurement

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//defualt scale factor--------------------
#define scaleCalibrationFactor_ 665 //-675 -- real load cells -- // -25500 test load cells
#define DOUT 6
#define CLK 7
bool cookTimerStartFlag_ = false;
enum logType
{
    RESET,
    ZERO,
    PRODUCTION_START,
    PRODUCTION_RUNNING,
    PRODUCTION_STOP
};
Config defualtConfig_ = {
    92, 60, 4, 0, 0, 4, -1};

int buttonPinout_[] = {
    29, 30, 31, 32, 33};
buttonController buttonController_;
//defualt password sets-------------------

#define programPassword "1579"
#define maintenencePassword "2503"

//defualt GPIO definitions----------------

#define valveOutput 35
#define gpio_input 36
const int chipSelectSD = 4;

//Class instances-------------------------
productionController productionController_;
operationModeController operationModeController_;
sensorController sensorController_;
timerController timerController_;
configController configController_;
displayController displayController_;
GPIOController GPIOController_(gpio_input, valveOutput);
temperatureController temperatureController_;

//Global variables-------------------------
operationModes selectedMode;
Page page_ = MENU;
uint32_t buttonTimer = 0;
uint32_t productionLogTimer = 0;
int passwordInputSelected = 1;
int programPageSelect = 3;
bool programPageCancel_ = true;
//#define SerialAT Serial1

#include <SoftwareSerial.h>
SoftwareSerial SerialAT(50, 49); // RX, TX
uint32_t gsmInitTimer = 0;
bool gsmInitFlag = false;
uint32_t rate = 0;
int signalValue = 0;
void setup()
{
    // SD Card library needs this to function accordingly

    pinMode(53, OUTPUT);
    digitalWrite(53, HIGH);

    // Soft Restart of GSM Module for Netowkr registration process
    SerialAT.begin(115200);
    while(!SerialAT){
      
    }
    
    gsmInitTimer = millis();


    Serial.begin(9600);
    while (!Serial)
    {
    }

    buttonController_.initButtons(buttonPinout_);
    displayController_.displayInit_();
    displayController_.showPage(StartUpPage);

    initSD();

    sensorController_.initThermo(sensorController_.prodTempSensor());
    sensorController_.initScale(DOUT, CLK);
    sensorController_.setScaleCalibrate_(scaleCalibrationFactor_);

    sensorController_.calibrateScale();

    GPIOController_.setPinModes(gpio_input, valveOutput);
    GPIOController_.setValveState(0);
    GPIOController_.state_ = false;

    if (!configController_.loadConfigEEPROM())
    {
        configController_.config_ = defualtConfig_;
        Serial.println("Defualt config Loaded");
    }

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
    }

    // rtc.adjust(DateTime(2020, 10, 21, 10, 45, 0));

    // initialize all Nextion object callback functions--------------
    initDisplayCallbacks();

    displayController_.showPage(menuPage);
    displayController_.updateMenu(operationModeController_.currentMode_);
    buttonTimer = millis();
}
void manageButtons()
{
    int buttonPushed = buttonController_.checkInput();
    // Serial.println(buttonPushed);
    switch (page_)
    {
    case MENU:
        switch (selectedMode)
        {
        case MODE_IDLE:
            switch (buttonPushed)
            {
            case UP:
                mode5SelectPopCallback(&mode5Select);
                break;
            case DOWN:
                mode2SelectPopCallback(&mode2Select);
                break;
            case LEFT:
                StatusButtonCallback(&StatusButton);
                break;
            case RIGHT:
                StatusButtonCallback(&StatusButton);
                break;
            case ENTER:
                menuOKCallback(&menuOKButton);
                break;

            default:
                break;
            }
            break;
        case MODE_PRODUCTION:
            switch (buttonPushed)
            {
            case UP:
                mode1SelectPopCallback(&mode1Select);
                break;
            case DOWN:
                mode3SelectPopCallback(&mode3Select);
                break;
            case LEFT:
                StatusButtonCallback(&StatusButton);
                break;
            case RIGHT:
                StatusButtonCallback(&StatusButton);
                break;
            case ENTER:
                menuOKCallback(&menuOKButton);
                break;

            default:
                break;
            }
            break;
        case MODE_PROGRAM:
            switch (buttonPushed)
            {
            case UP:
                mode2SelectPopCallback(&mode2Select);
                break;
            case DOWN:
                mode4SelectPopCallback(&mode4Select);
                break;
            case LEFT:
                StatusButtonCallback(&StatusButton);
                break;
            case RIGHT:
                StatusButtonCallback(&StatusButton);
                break;
            case ENTER:
                menuOKCallback(&menuOKButton);
                break;

            default:
                break;
            }
            break;
        case MODE_MAINTENENCE:
            switch (buttonPushed)
            {
            case UP:
                mode3SelectPopCallback(&mode3Select);
                break;
            case DOWN:
                mode5SelectPopCallback(&mode5Select);
                break;
            case LEFT:
                StatusButtonCallback(&StatusButton);
                break;
            case RIGHT:
                StatusButtonCallback(&StatusButton);
                break;
            case ENTER:
                menuOKCallback(&menuOKButton);
                break;

            default:
                break;
            }
            break;
        case MODE_LOADING:
            switch (buttonPushed)
            {
            case UP:
                mode4SelectPopCallback(&mode4Select);
                break;
            case DOWN:
                mode1SelectPopCallback(&mode1Select);
                break;
            case LEFT:
                StatusButtonCallback(&StatusButton);
                break;
            case RIGHT:
                StatusButtonCallback(&StatusButton);
                break;
            case ENTER:
                menuOKCallback(&menuOKButton);
                break;

            default:
                break;
            }
            break;
        case MODE_TESTING:
            switch (buttonPushed)
            {
            case UP:
                mode5SelectPopCallback(&mode5Select);
                break;
            case DOWN:
                mode5SelectPopCallback(&mode5Select);
                break;
            case LEFT:
                StatusButtonCallback(&StatusButton);
                break;
            case RIGHT:
                StatusButtonCallback(&StatusButton);
                break;
            case ENTER:
                menuOKCallback(&menuOKButton);
                break;

            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    case PRODUCTION:
        switch (buttonPushed)
        {
        case UP:

            break;
        case DOWN:

            break;
        case LEFT:
            StatusButtonCallback(&StatusButton);
            break;
        case RIGHT:
            StatusButtonCallback(&StatusButton);
            break;
        case ENTER:
            statusPageMenuButtonCallback(&statusPageMenuButton);
            break;

        default:
            break;
        }
        break;
    case STATUS:
        switch (buttonPushed)
        {
        case UP:

            break;
        case DOWN:

            break;
        case LEFT:
            if (operationModeController_.currentMode_ == PRODUCTION)
            {
                page_ = PRODUCTION;
                displayController_.showPage(productionPage);
            }

            break;
        case RIGHT:
            if (operationModeController_.currentMode_ == PRODUCTION)
            {
                page_ = PRODUCTION;
                displayController_.showPage(productionPage);
            }
            break;
        case ENTER:
            statusPageMenuButtonCallback(&statusPageMenuButton);
            break;

        default:
            break;
        }
        break;
    case PASSWORD:
        switch (buttonPushed)
        {
        case UP:
            displayController_.updatePasswordValue(passwordInputSelected, true);
            break;
        case DOWN:
            displayController_.updatePasswordValue(passwordInputSelected, false);
            break;
        case LEFT:
            switch (passwordInputSelected)
            {
            case 1:
                passwordInputSelected = 4;
                displayController_.updatePasswordPage(passwordInputSelected);

                break;
            case 2:
                passwordInputSelected = 1;
                displayController_.updatePasswordPage(passwordInputSelected);
                break;
            case 3:
                passwordInputSelected = 2;
                displayController_.updatePasswordPage(passwordInputSelected);
                break;
            case 4:
                passwordInputSelected = 3;
                displayController_.updatePasswordPage(passwordInputSelected);
                break;

            default:
                break;
            }
            break;
        case RIGHT:
            switch (passwordInputSelected)
            {
            case 1:
                passwordInputSelected = 2;
                displayController_.updatePasswordPage(passwordInputSelected);
                break;
            case 2:
                passwordInputSelected = 3;
                displayController_.updatePasswordPage(passwordInputSelected);
                break;
            case 3:
                passwordInputSelected = 4;
                displayController_.updatePasswordPage(passwordInputSelected);
                break;
            case 4:
                passwordInputSelected = 1;
                displayController_.updatePasswordPage(passwordInputSelected);
                break;

            default:
                break;
            }
            break;
        case ENTER:
            passwordOKCheck(&passwordOKButton);
            break;

        default:
            break;
        }
        break;
    case PROGRAM:
        uint32_t number1 = 0;
        uint32_t number2 = 0;
        if (buttonPushed != -1)
        {
            //Serial.println(programPageSelect);

            TEMP_SET_number.getValue(&number1);
            delay(200);

            TIME_SET_number.getValue(&number2);
        }
        switch (buttonPushed)
        {
        case UP:
            if (programPageSelect == 3)
            {
            }
            else
            {
                programPageSelect++;
            }
            displayController_.updateProgramPage(programPageSelect, programPageCancel_);
            break;
        case DOWN:
            if (programPageSelect == 1)
            {
            }
            else
            {
                programPageSelect--;
            }
            displayController_.updateProgramPage(programPageSelect, programPageCancel_);
            break;
        case LEFT:

            switch (programPageSelect)
            {
            case 1:
                if (programPageCancel_)
                {
                    programPageCancel_ = false;
                }
                else
                {
                    programPageCancel_ = true;
                }
                displayController_.updateProgramPage(programPageSelect, programPageCancel_);
                break;
            case 2:

                Serial.println(number2);
                delay(200);
                if (number2 != 0)
                {
                    TIME_SET_number.setValue(number2 - 1);
                }

                break;
            case 3:

                delay(200);
                Serial.println(number1);

                if (number1 != 0)
                {

                    TEMP_SET_number.setValue(number1 - 1);
                }
                break;

            default:
                break;
            }
            break;
        case RIGHT:

            switch (programPageSelect)
            {
            case 1:
                if (programPageCancel_)
                {
                    programPageCancel_ = false;
                }
                else
                {
                    programPageCancel_ = true;
                }
                displayController_.updateProgramPage(programPageSelect, programPageCancel_);
                break;
            case 2:

                TIME_SET_number.setValue(number2 + 1);
                break;
            case 3:

                TEMP_SET_number.setValue(number1 + 1);

                break;

            default:
                break;
            }
            break;
        case ENTER:
            if (programPageCancel_ == false)
            {
                enterButtonProgramPageCallback(&enterButtonProgramPage);
            }
            else
            {
                cancelProgramButtonCallback(&cancelProgramButton);
            }
            break;

        default:
            break;
        }
        break;
    default:
        break;
    }
}
void loop()
{

    nexLoop(nex_listen_list);
    if(millis() - gsmInitTimer > 25000 && rstFlag ==0){
      SerialAT.println("AT+RST=1");
      rstFlag=1;
    }
    if (millis() - buttonTimer > 150)
    {
        manageButtons();
        buttonTimer = millis();
    }
    if (millis() - gsmInitTimer > 60000 && gsmInitFlag == false)
    {
        setupGSM();
        gsmInitFlag = true;
        pushingboxSend("STARTUP");
        
    }
    if (millis() - gsmInitTimer > 65000)
    {
        checkSignal();
        gsmInitTimer = millis();
    }
    switch (operationModeController_.currentMode_)
    {
    case MODE_PRODUCTION:
        const float BAND_GAP = configController_.config_.BAND_GAP;
        const float SET_TEMP = configController_.config_.TEMP_SET;
        const float BAND_OFFSET = configController_.config_.BAND_OFFSET;

        const float BAND_LOWER = SET_TEMP + BAND_OFFSET - (BAND_GAP / 2);
        const float BAND_UPPER = SET_TEMP + BAND_OFFSET + (BAND_GAP / 2);

        const int offset = configController_.config_.TEMP_CALIBRATE;
        const float PROD_TEMP = sensorController_.readTemp(sensorController_.prodTempSensor());

        temperatureController_.doControl(configController_, sensorController_, GPIOController_);

        if (millis() - productionLogTimer > 60000)
        {
            logDataSD(PRODUCTION_RUNNING);

            pushingboxSend("PRODUCTION_UPDATE");
            productionLogTimer = millis();
        }
        if ((PROD_TEMP + offset) >= BAND_LOWER || cookTimerStartFlag_ == true)
        {
            cookTimerStartFlag_ = true;

            if (timerController_.timerUpdate())
            {

                timerController_.timerStop();
                productionController_.stopProduction(timerController_, GPIOController_);
                operationModeController_.changeMode(MODE_IDLE);
                displayController_.showPage(BatchCompletePage);
                completeWeightNumber.setValue(sensorController_.getWeight());
                cookTimerStartFlag_ = false;
                pushingboxSend("PRODUCTION_COMPLETE");
                delay(10000);
                displayController_.showPage(menuPage);
                displayController_.updateMenu(operationModeController_.currentMode_);
            }
        }
    }
    //page details update processing -----------------------------------------------------
    switch (page_)
    {
    case STATUS:
        displayController_.updateStatusPage(GPIOController_, configController_, sensorController_, operationModeController_.currentMode_, signalValue);
        break;
    case PRODUCTION:

        displayController_.updateProductionPage(configController_.config_.TIME_SET, timerController_, sensorController_, configController_.config_.TEMP_SET, configController_.config_.BAND_GAP, configController_.config_.TEMP_CALIBRATE);
        break;
    case LOADING:
        displayController_.updateLoadingPage(sensorController_.getWeight());
        break;
    case TESTING:
        displayController_.updateTestPage(GPIOController_);
        break;
    case SCALE_CAL:
        displayController_.updateScaleCal(sensorController_.getWeight(), sensorController_.scaleCalibrationfactor);
    default:

        break;
    }
}

// Nextion Display interface functions -----------------------------------------------------

void initDisplayCallbacks()
{
    //menu screen object callbacks ------------------------------------------------------
    menuOKButton.attachPop(menuOKCallback, &menuOKButton);
    mode1Select.attachPop(mode1SelectPopCallback, &mode1Select);
    mode2Select.attachPop(mode2SelectPopCallback, &mode2Select);
    mode3Select.attachPop(mode3SelectPopCallback, &mode3Select);
    mode4Select.attachPop(mode4SelectPopCallback, &mode4Select);
    mode5Select.attachPop(mode5SelectPopCallback, &mode5Select);
    StatusButton.attachPop(StatusButtonCallback, &StatusButton);
    statusPageMenuButton.attachPop(statusPageMenuButtonCallback, &statusPageMenuButton);
    productionPageMenuButton.attachPop(productionPageMenuButtonCallback, &productionPageMenuButton);
    outputTestValveButton.attachPop(outputTestValveButtonCallback, &outputTestValveButton);
    maintenenceOutputTestButton.attachPop(maintenenceOutputTestButtonCallback, &maintenenceOutputTestButton);
    outputTestBackButton.attachPop(outputTestBackButtonCallback, &outputTestBackButton);
    cancelMaintenenceButton.attachPop(cancelMaintenenceButtonCallback, &cancelMaintenenceButton);
    cancelProgramButton.attachPop(cancelProgramButtonCallback, &cancelProgramButton);
    //password screen callbacks
    passwordOKButton.attachPop(passwordOKCheck, &passwordOKButton);
    passwordCancelButton.attachPop(passwordCancelCallback, &passwordCancelButton);
    //loading screen callbacks
    zeroScaleButton.attachPop(zeroScalecallback, &zeroScaleButton);
    cancelLoadingButton.attachPop(cancelLoadingButtoncallback, &cancelLoadingButton);
    enterButtonProgramPage.attachPop(enterButtonProgramPageCallback, &enterButtonProgramPage);
    enterMaintenencePageButton.attachPop(enterMaintenencePageButtoncallback, &enterMaintenencePageButton);

    ScaleCalInc10.attachPop(ScaleCalInc10CB, &ScaleCalInc10);
    ScaleCalDec10.attachPop(ScaleCalDec10CB, &ScaleCalDec10);
    ScaleCalInc100.attachPop(ScaleCalInc100CB, &ScaleCalInc100);
    ScaleCalDec100.attachPop(ScaleCalDec100CB, &ScaleCalDec100);
    ScaleCalInc1000.attachPop(ScaleCalInc1000CB, &ScaleCalInc1000);
    ScaleCalDec1000.attachPop(ScaleCalDec1000CB, &ScaleCalDec1000);
    ScaleCalDone.attachPop(ScaleCalDoneCB, &ScaleCalDone);
    ScaleCalCancel.attachPop(ScaleCalCancelCB, &ScaleCalCancel);
    ScaleCali.attachPop(ScaleCaliCB, &ScaleCali);

    maintenanceBandGapInc_.attachPop(maintenanceBandGapInc_Callback, &maintenanceBandGapInc_);
    maintenanceBandGapDec_.attachPop(maintenanceBandGapDec_Callback, &maintenanceBandGapDec_);

    maintenanceWidthInc_.attachPop(maintenanceWidthInc_Callback, &maintenanceWidthInc_);
    maintenanceWidthDec_.attachPop(maintenanceWidthDec_Callback, &maintenanceWidthDec_);

    maintenanceOffsetInc_.attachPop(maintenanceOffsetInc_Callback, &maintenanceOffsetInc_);
    maintenanceOffsetDec_.attachPop(maintenanceOffsetDec_Callback, &maintenanceOffsetDec_);

    maintenanceDelayInc_.attachPop(maintenanceDelayInc_Callback, &maintenanceDelayInc_);
    maintenanceDelayDec_.attachPop(maintenanceDelayDec_Callback, &maintenanceDelayDec_);

    maintenanceCalibrateInc_.attachPop(maintenanceCalibrateInc_Callback, &maintenanceCalibrateInc_);
    maintenanceCalibrateDec_.attachPop(maintenanceCalibrateDec_Callback, &maintenanceCalibrateDec_);
}
void ScaleCalDoneCB(void *ptr)
{
    Serial.println("Scale new cali");
    Serial.println(sensorController_.scaleCalibrationfactor);
    displayController_.showPage(loadingPage);
    page_ = LOADING;
    pushingboxSend("SCALE_CALIBRATED");
}
void ScaleCalCancelCB(void *ptr)
{
    displayController_.showPage(loadingPage);
    page_ = LOADING;
}

void ScaleCaliCB(void *ptr)
{
    page_ = SCALE_CAL;
    displayController_.showPage(ScaleCaliPage);
}
void ScaleCalInc10CB(void *ptr)
{
    sensorController_.setScaleCalibrate_(sensorController_.scaleCalibrationfactor + 10);
}
void ScaleCalDec10CB(void *ptr)
{
    sensorController_.setScaleCalibrate_(sensorController_.scaleCalibrationfactor - 10);
}
void ScaleCalInc100CB(void *ptr)
{
    sensorController_.setScaleCalibrate_(sensorController_.scaleCalibrationfactor + 100);
}
void ScaleCalDec100CB(void *ptr)
{
    sensorController_.setScaleCalibrate_(sensorController_.scaleCalibrationfactor - 100);
}
void ScaleCalInc1000CB(void *ptr)
{
    sensorController_.setScaleCalibrate_(sensorController_.scaleCalibrationfactor + 1000);
}
void ScaleCalDec1000CB(void *ptr)
{
    sensorController_.setScaleCalibrate_(sensorController_.scaleCalibrationfactor - 1000);
}
void cancelMaintenenceButtonCallback(void *ptr)
{
    displayController_.showPage(menuPage);
    operationModeController_.changeMode(MODE_IDLE);
    displayController_.updateMenu(operationModeController_.currentMode_);
    selectedMode = operationModeController_.currentMode_;
}
void cancelProgramButtonCallback(void *ptr)
{
    displayController_.showPage(menuPage);
    operationModeController_.changeMode(MODE_IDLE);
    displayController_.updateMenu(operationModeController_.currentMode_);
    selectedMode = operationModeController_.currentMode_;
    page_ = MENU;
}
void outputTestBackButtonCallback(void *ptr)
{
    operationModeController_.changeMode(MODE_MAINTENENCE);
    displayController_.showPage(maintenencePage);
    page_ = MAINTENENCE;
    displayController_.updateMaintenencePage(configController_);
}
void outputTestValveButtonCallback(void *ptr)
{
    if (GPIOController_.getValveState() == 0)
    {
        GPIOController_.state_ = false;
        digitalWrite(35, HIGH);
    }
    else
    {
        GPIOController_.state_ = true;
        digitalWrite(35, LOW);
    }
}
void maintenenceOutputTestButtonCallback(void *ptr)
{
    page_ = TESTING;
    operationModeController_.changeMode(MODE_TESTING);
    displayController_.showPage(OutputTestPage);
}
void productionPageMenuButtonCallback(void *ptr)
{
    displayController_.showPage(menuPage);
    page_ = MENU;
    displayController_.updateMenu(operationModeController_.currentMode_);
}
void statusPageMenuButtonCallback(void *ptr)
{
    displayController_.showPage(menuPage);
    page_ = MENU;
    displayController_.updateMenu(operationModeController_.currentMode_);
}
void StatusButtonCallback(void *ptr)
{
    page_ = STATUS;
    displayController_.showPage(StatusPage);
    displayController_.updateStatusPage(GPIOController_, configController_, sensorController_, operationModeController_.currentMode_, signalValue);
}
void zeroScalecallback(void *ptr)
{
    sensorController_.calibrateScale();
    logDataSD(ZERO);
    pushingboxSend("SCALE_ZERO");
}
void passwordCancelCallback(void *ptr)
{
    operationModeController_.changeMode(MODE_IDLE);
    displayController_.showPage(menuPage);
    page_ = MENU;
    displayController_.updateMenu(operationModeController_.currentMode_);
    selectedMode = operationModeController_.currentMode_;
}
void passwordOKCheck(void *ptr)
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
    String pass = String(number1);
    String pass1 = pass + number2 + number3 + number4;

    switch (operationModeController_.currentMode_)
    {
    case MODE_PROGRAM:
        if (pass1 == programPassword)
        {
            page_ = PROGRAM;
            displayController_.showPage(programPage);
            displayController_.updateProgramPage(configController_);
            displayController_.updateProgramPage(programPageSelect, programPageCancel_);
        }
        else
        {
            passwordCancelCallback(&passwordCancelButton);
        }
        break;
    case MODE_MAINTENENCE:
        if (pass1 == maintenencePassword)
        {
            page_ = MAINTENENCE;
            displayController_.showPage(maintenencePage);
            displayController_.updateMaintenencePage(configController_);
        }
        else
        {
            passwordCancelCallback(&passwordCancelButton);
        }
        break;
    default:
        break;
    }
}
void enterMaintenencePageButtoncallback(void *ptr)
{

    configController_.saveConfigEEPROM(configController_.config_);
    operationModeController_.changeMode(MODE_IDLE);
    displayController_.showPage(menuPage);
    page_ = MENU;
    displayController_.updateMenu(operationModeController_.currentMode_);
}

void cancelLoadingButtoncallback(void *ptr)
{
    operationModeController_.changeMode(MODE_IDLE);
    displayController_.showPage(menuPage);
    page_ = MENU;
    displayController_.updateMenu(operationModeController_.currentMode_);
    selectedMode = operationModeController_.currentMode_;
}
void enterButtonProgramPageCallback(void *ptr)
{
    TEMP_SET_number.getValue(&configController_.config_.TEMP_SET);
    TIME_SET_number.getValue(&configController_.config_.TIME_SET);
    configController_.saveConfigEEPROM(configController_.config_);
    operationModeController_.changeMode(MODE_IDLE);
    displayController_.showPage(menuPage);
    page_ = MENU;
    displayController_.updateMenu(operationModeController_.currentMode_);
}
void menuOKCallback(void *ptr)
{
    switch (selectedMode)
    {
    case MODE_IDLE:
        displayController_.showPage(menuPage);
        operationModeController_.changeMode(MODE_IDLE);
        displayController_.updateMenu(operationModeController_.currentMode_);
        productionController_.stopProduction(timerController_, GPIOController_);
        GPIOController_.setValveState(0);
        GPIOController_.state_ = false;

        break;
    case MODE_PRODUCTION:
        if (operationModeController_.currentMode_ == MODE_PRODUCTION)
        {
            page_ = PRODUCTION;
            displayController_.showPage(productionPage);
        }
        else
        {

            GPIOController_.setValveState(1);
            GPIOController_.state_ = true;
            cookTimerStartFlag_ = false;
            operationModeController_.changeMode(MODE_PRODUCTION);
            timerController_.timerConfigure_(configController_.config_.TIME_SET);
            displayController_.showPage(productionPage);
            timerController_.timeLeft=configController_.config_.TIME_SET *60;
            displayController_.updateProductionPage(configController_.config_.TIME_SET, timerController_, sensorController_, configController_.config_.TEMP_SET, configController_.config_.BAND_GAP, configController_.config_.TEMP_CALIBRATE);
            logDataSD(PRODUCTION_START);
            productionLogTimer = millis();
            ;
            pushingboxSend("PRODUCTION_START");
        }
        page_ = PRODUCTION;

        break;
    case MODE_PROGRAM:
        if (operationModeController_.currentMode_ == MODE_PRODUCTION)
        {
        }
        else
        {
            displayController_.showPage(passwordPage);
            page_ = PASSWORD;
            operationModeController_.changeMode(MODE_PROGRAM);
            displayController_.updatePasswordPage(passwordInputSelected);
        }

        break;
    case MODE_MAINTENENCE:
        if (operationModeController_.currentMode_ == MODE_PRODUCTION)
        {
        }
        else
        {
            page_ = PASSWORD;
            displayController_.showPage(passwordPage);
            operationModeController_.changeMode(MODE_MAINTENENCE);
            displayController_.updatePasswordPage(passwordInputSelected);
        }

        break;
    case MODE_LOADING:

        if (operationModeController_.currentMode_ == MODE_PRODUCTION)
        {
        }
        else
        {
            page_ = LOADING;
            operationModeController_.changeMode(MODE_LOADING);
            displayController_.showPage(loadingPage);
        }

        break;

    default:
        break;
    }
}

void mode1SelectPopCallback(void *ptr)
{
    selectedMode = MODE_IDLE;
    mode1Marker.Set_background_color_bco(3744);
    mode2Marker.Set_background_color_bco(65535);
    mode3Marker.Set_background_color_bco(65535);
    mode4Marker.Set_background_color_bco(65535);
    mode5Marker.Set_background_color_bco(65535);
}
void mode2SelectPopCallback(void *ptr)
{
    selectedMode = MODE_PRODUCTION;
    mode1Marker.Set_background_color_bco(65535);
    mode2Marker.Set_background_color_bco(3744);
    mode3Marker.Set_background_color_bco(65535);
    mode4Marker.Set_background_color_bco(65535);
    mode5Marker.Set_background_color_bco(65535);
}
void mode3SelectPopCallback(void *ptr)
{
    selectedMode = MODE_PROGRAM;
    mode1Marker.Set_background_color_bco(65535);
    mode2Marker.Set_background_color_bco(65535);
    mode3Marker.Set_background_color_bco(3744);
    mode4Marker.Set_background_color_bco(65535);
    mode5Marker.Set_background_color_bco(65535);
}
void mode4SelectPopCallback(void *ptr)
{
    selectedMode = MODE_MAINTENENCE;
    mode1Marker.Set_background_color_bco(65535);
    mode2Marker.Set_background_color_bco(65535);
    mode3Marker.Set_background_color_bco(65535);
    mode4Marker.Set_background_color_bco(65535);
    mode5Marker.Set_background_color_bco(3744);
}
void mode5SelectPopCallback(void *ptr)
{
    selectedMode = MODE_LOADING;
    mode1Marker.Set_background_color_bco(65535);
    mode2Marker.Set_background_color_bco(65535);
    mode3Marker.Set_background_color_bco(65535);
    mode4Marker.Set_background_color_bco(3744);
    mode5Marker.Set_background_color_bco(65535);
}

void maintenanceBandGapInc_Callback(void *ptr)
{
    configController_.config_.BAND_GAP++;
    displayController_.updateMaintenencePage(configController_);
}
void maintenanceBandGapDec_Callback(void *ptr)
{
    if (configController_.config_.BAND_GAP == 0)
    {
    }
    else
    {
        configController_.config_.BAND_GAP--;
        displayController_.updateMaintenencePage(configController_);
    }
}
void maintenanceWidthInc_Callback(void *ptr)
{

    configController_.config_.WIDTH++;
    displayController_.updateMaintenencePage(configController_);
}
void maintenanceWidthDec_Callback(void *ptr)
{
    if (configController_.config_.WIDTH == 0)
    {
    }
    else
    {
        configController_.config_.WIDTH--;
        displayController_.updateMaintenencePage(configController_);
    }
}
void maintenanceOffsetInc_Callback(void *ptr)
{
    configController_.config_.BAND_OFFSET++;
    displayController_.updateMaintenencePage(configController_);
}
void maintenanceOffsetDec_Callback(void *ptr)
{

    configController_.config_.BAND_OFFSET--;
    displayController_.updateMaintenencePage(configController_);
}
void maintenanceDelayInc_Callback(void *ptr)
{
    configController_.config_.DELAY_TIME++;
    displayController_.updateMaintenencePage(configController_);
}
void maintenanceDelayDec_Callback(void *ptr)
{
    if (configController_.config_.DELAY_TIME == 0)
    {
    }
    else
    {
        configController_.config_.DELAY_TIME--;
        displayController_.updateMaintenencePage(configController_);
    }
}
void maintenanceCalibrateInc_Callback(void *ptr)
{
    configController_.config_.TEMP_CALIBRATE++;
    displayController_.updateMaintenencePage(configController_);
}
void maintenanceCalibrateDec_Callback(void *ptr)
{
    configController_.config_.TEMP_CALIBRATE--;
    displayController_.updateMaintenencePage(configController_);
}

// Data logging functions
void initSD()
{
    bool success = false;
    for (size_t i = 0; (i < 5) && !success; ++i)
    {
        success = SD.begin(chipSelectSD);
        if (!success)
        {
            Serial.println("Could not initialize SD. retrying...");
        }
        delay(1000);
    }
    if (success)
    {
        Serial.println("SD Init successful");
    }
    delay(1000);
}

void logDataSD(logType logType_)
{

    File dataFile = SD.open("DataLog.txt", FILE_WRITE);
    if (dataFile)
    {
        String Time = "RTC not active";

        String date = "RTC not active";
        if (rtc.isrunning())
        {
            DateTime now = rtc.now();

            String Time = String(now.hour());
            Time += ":" + String(now.minute()) + ":" + String(now.second());
            String date = String(now.day());
            date += "/" + String(now.month()) + "/" + String(now.year());
            Serial.println(date);
            Serial.println(Time);
        }

        switch (logType_)
        {
        case RESET:

            dataFile.print(date);
            dataFile.print(",");
            dataFile.print(Time);
            dataFile.print(",");
            dataFile.print("SYSTEM RESET");
            dataFile.print(",");
            dataFile.print(configController_.config_.TEMP_SET);
            dataFile.print(",");
            dataFile.print(configController_.config_.TIME_SET);
            dataFile.print(",");
            dataFile.print(sensorController_.readTemp(sensorController_.prodTempSensor()));
            dataFile.print(",");
            if (GPIOController_.getValveState() == 1)
            {
                dataFile.print("VALVE_OPEN");
            }
            else
            {
                dataFile.print("VALVE_CLOSED");
            }
            dataFile.println();
            break;
        case ZERO:

            dataFile.print(date);
            dataFile.print(",");
            dataFile.print(Time);
            dataFile.print(",");
            dataFile.print("SCALE ZERO:  ");
            dataFile.print(",");
            dataFile.print(configController_.config_.TEMP_SET);
            dataFile.print(",");
            dataFile.print(configController_.config_.TIME_SET);
            dataFile.print(",");
            dataFile.print(sensorController_.readTemp(sensorController_.prodTempSensor()));
            dataFile.print(",");
            if (GPIOController_.getValveState() == 1)
            {
                dataFile.print("VALVE_OPEN");
            }
            else
            {
                dataFile.print("VALVE_CLOSED");
            }
            dataFile.println();
            break;
        case PRODUCTION_START:

            dataFile.print(date);
            dataFile.print(",");
            dataFile.print(Time);
            dataFile.print(",");
            dataFile.print("PRODUCTION START");
            dataFile.print(",");
            dataFile.print(configController_.config_.TEMP_SET);
            dataFile.print(",");
            dataFile.print(configController_.config_.TIME_SET);
            dataFile.print(",");
            dataFile.print(sensorController_.readTemp(sensorController_.prodTempSensor()));
            dataFile.print(",");
            if (GPIOController_.getValveState() == 1)
            {
                dataFile.print("VALVE_OPEN");
            }
            else
            {
                dataFile.print("VALVE_CLOSED");
            }
            dataFile.println();

            break;
        case PRODUCTION_RUNNING:

            dataFile.print(date);
            dataFile.print(",");
            dataFile.print(Time);
            dataFile.print(",");
            dataFile.print("PRODUCTION UPDATE");
            dataFile.print(",");
            dataFile.print(configController_.config_.TEMP_SET);
            dataFile.print(",");
            dataFile.print(configController_.config_.TIME_SET);
            dataFile.print(",");
            dataFile.print(sensorController_.readTemp(sensorController_.prodTempSensor()));
            dataFile.print(",");
            if (GPIOController_.getValveState() == 1)
            {
                dataFile.print("VALVE_OPEN");
            }
            else
            {
                dataFile.print("VALVE_CLOSED");
            }
            dataFile.println();
            break;
        case PRODUCTION_STOP:
            dataFile.print("Date:  ");
            dataFile.print(date);
            dataFile.print(",");
            dataFile.print(Time);
            dataFile.print(",");
            dataFile.print("PRODUCTION STOP:  ");
            dataFile.print(",");
            dataFile.print(configController_.config_.TEMP_SET);
            dataFile.print(",");
            dataFile.print(configController_.config_.TIME_SET);
            dataFile.print(",");
            dataFile.print(sensorController_.readTemp(sensorController_.prodTempSensor()));
            dataFile.print(",");
            if (GPIOController_.getValveState() == 1)
            {
                dataFile.print("VALVE_OPEN");
            }
            else
            {
                dataFile.print("VALVE_CLOSED");
            }

            dataFile.println();
            break;
        default:
            break;
        }

        dataFile.close();
    }
    else
    {
        Serial.println("Unable to open Datalogg file");
    }
}

void setupGSM()
{
  while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }
    uint32_t tim = millis();
    Serial.println("Connecting GSM");

    SerialAT.println("AT");
    delay(200);

    SerialAT.println("AT+CPIN?");
    delay(200);
    while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }

    SerialAT.println("AT+CREG?");
    delay(1000);
    while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }

    SerialAT.println("AT+CGATT?");
    delay(200);
    while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }

    SerialAT.println("AT+CIPSHUT");
    delay(200);
    while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }

    SerialAT.println("AT+CIPSTATUS");
    delay(200);
    while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }
}
void checkSignal()
{
    int rxByte;

    int count = 0;
    bool a = true;
    while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }

    SerialAT.println("AT+CSQ");
    delay(200);
    while (SerialAT.available() != 0)
    {
        rxByte = SerialAT.read();
        //Serial.println(rxByte);
        if (rxByte == 58)
        {
            rxByte = SerialAT.read();
            if (rxByte == 32)
            {
                rxByte = SerialAT.read();
                signalValue = (int)char(rxByte) - 48;

                rxByte = SerialAT.read();
                if (rxByte == 44)
                {
                }
                else
                {

                    signalValue = signalValue * 10;
                    signalValue += (int)char(rxByte) - 48;
                }
            }
        }
    }

    signalValue = signalValue * 100;
    signalValue = signalValue / 30;
}
void pushingboxSend(String Event)
{
  while (SerialAT.available() != 0)
    {
        Serial.write(SerialAT.read());
    }
    String steamValveStatus;
    SerialAT.println("AT+CIPSHUT");
    delay(100);

    if (digitalRead(valveOutput) == 1)
    {
        steamValveStatus = "OPEN";
    }
    else if (digitalRead(valveOutput) == 0)
    {
        steamValveStatus = "CLOSED";
    }

    String query = "GET /CookingPotLog.php?";

    query += "EVENT=" + Event;
    query += "&TEMP_SET=" + String(configController_.config_.TEMP_SET);
    query += "&TIME_SET=" + String(configController_.config_.TIME_SET);
    query += "&PROD_TEMP=" + String(sensorController_.readTemp(sensorController_.prodTempSensor()));
    query += "&STEAM_VALVE_STATUS=" + steamValveStatus;
    query += " HTTP/1.1\r\n";
    
    // Ethernet option
    Ethernet.begin(mac, ip);
    client.connect(server, 80);
    if (client.connected())
    {
        client.println(query);
    }
    else
    {
        Serial.println("No Ethernet Connection");
        SerialAT.println("AT+CIPMUX=0");
        delay(100);
        while (SerialAT.available() != 0)
        {
            Serial.write(SerialAT.read());
        }

        SerialAT.println("AT+CSTT=\"justworx\""); //start task and setting the APN, mms.mtn.co.za/mms/wapenc   mms1.live.vodafone.in/mms/   http://mmsc.vodacom4me.co.za   AT+CSTT=mms.mtn.co.za/mms/wapenc
        delay(100);
        while (SerialAT.available() != 0)
        {
            Serial.write(SerialAT.read());
        }
        SerialAT.println("AT+CIICR"); //bring up wireless connection
        delay(1500);
        while (SerialAT.available() != 0)
        {
            Serial.write(SerialAT.read());
        }

        SerialAT.println("AT+CIFSR"); //get local IP adress
        delay(100);
        while (SerialAT.available() != 0)
        {
            Serial.write(SerialAT.read());
        }

        SerialAT.println("AT+CIPSPRT=0");
        delay(100);
        //http://www.amavatech.com/CookingPotLog.php?TEMP_SET=25&TIME_SET=65&PROD_TEMP=100&STEAM_VALVE_STATUS=OPEN&ID=1
        //SerialAT.println("AT+CIPSTART=\"TCP\",\"api.pushingbox.com\",80"); //start up the connection
        //
        SerialAT.println("AT+CIPSTART=\"TCP\",\"www.amavatech.com\",80");
        delay(3000);
        while (SerialAT.available() != 0)
        {
            Serial.write(SerialAT.read());
        }

        SerialAT.println("AT+CIPSEND\r\n"); //begin send data to remote server

        SerialAT.print(query);

        //    SerialAT.print("Host: api.pushingbox.com\r\n");
        SerialAT.print("Host: www.amavatech.com\r\n");
        SerialAT.print("\r\n");   // <- just end of line
        SerialAT.print((char)26); // <- execute HTTP request

        uint32_t tim = millis();

        delay(500); //waitting for reply, important! the time is base on the condition of internet
        while (Serial1.available() != 0)
        {
            Serial.write(Serial1.read());
        }

        Serial1.println("AT+CIPSHUT"); //close the connection
        delay(100);
        while (Serial1.available() != 0)
        {
            Serial.write(Serial1.read());
        }
    }
}
