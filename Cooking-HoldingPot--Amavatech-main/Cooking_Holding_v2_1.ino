#include <avr/wdt.h>
#include <SPI.h>
// #include <SD.h>
//#include "RTClib.h"
//#include "SdFat.h"
#include "SensorController.h"
#include "GpioController.h"
#include "ConfigController.h"
#include <Arduino.h>
#include "OperationModeController.h"
#include "MixersController.h"
#include <ModbusMaster.h>
#include "PumpsController.h"

#include "GSMController.h"
#include "DisplayController.h"
#include "cookTimerController.h"
#include "TempController.h"
#include "ModbusController.h"
//bool drainingCook = false;
//bool drainingHold = false;
//bool waterFillBegin_ = false;
bool rstFlag = 0;
// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true; // set to false for better speed measurement
//RTC_DS1307 rtc;
//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int CIPSelect_ = 0;
//bool CIPBegin = false;
//uint32_t testFlowCali = 0;
uint16_t flowMeterPulse = 0;
double pulseRateCoefficient = 23.1;
int tempCookAveraged = 0;
int tempHoldAveraged=0;
int tempAvCounter=0;
int tempCookBuff[6];
int tempHoldBuff[6];
//#define MEGA_SOFT_SPI 0
/////////////////////////////////////////////////////////////////////////////
//double flowRateTimer = 0;
///double flowRateCoefficient = 14.77;
double waterFilled = 0;
//uint32_t waterFilledTimer = 0;

bool cipHeatComplete = false;
uint32_t cipTimer = 0;
bool cipDrainBegin = false;
uint32_t cipDrainTimer = 0;
///0.1 -- 6  -- 65hz max
uint16_t pageTimer = 0;
bool cookComplete = false;
bool cipFillComplete = false;
uint16_t prodHoldTimer=0;
int cipTypeSelect = 0; // Rinse = 0, Sani = 1
SensorController sensorController_;
GPIOController gpioController_;
DisplayController displayController_;
OperationModeController operationModeController_;
MixersController mixersController_;
PumpsController pumpsController_;
ConfigController configController_;
GSMController GSMController_;
//SDController SDController_;
TempController TempController_;
cookTimerController cookTimerController_;
ModbusController modbusController_;
bool cookDraining = false;
bool holdDraining = false;
//// Pin numbers in templates must be constants.
// uint8_t SOFT_MISO_PIN = 12;
// uint8_t SOFT_MOSI_PIN = 11;
// uint8_t SOFT_SCK_PIN = 13;
//
// Chip select may be constant or RAM variable.

//uint8_t const SOFT_SPI_CS_PIN = 10;

// SdFat software SPI template
//SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

bool cookProdRunning = false;
bool cookHoldRunning = false;
bool cipRunning = false;
bool prodCookPaused = false;
bool startTranfer = false;
Page page_ = MENU;
uint16_t gsmInitTimer = 0;
bool gsmInitFlag = false;
bool gsmstartUpSent = false;
//uint32_t cookTimeReference = 0;
bool cipPrimerActive = true;
//uint32_t primerTimer = 0;
bool primeInit = false;
valveOutputsPins valveOutputs_ = {
    40, // switched
    31,
    27, //V3
    28, //V4
    25,
    36, //V12
    44,
    30, //V6
    29, //V5
    32, //V8
    45,
    41, //V17
    34, //V10
    43, //V19
    35, //V11
    37, //V13
    33,
    26, //V2
    39, //V15
    38, //V14
    42  //V18

}; //VSteam2

generalInputs generalInputs_{
    50, // Safety Proxy Signal
    49, //Holding Low Signal
    48, // Cooking Low Signal
    47  // Holding High Signal
};

MixerAddressCache mixerAddresses{
    1, // HS
    2, // C1
    7, // C2
    6  // H1
};

PumpAddressCache pumpAddresses{
    4, //LOBE1
    5, //LOBE2
    3  //CIP
};

#define SerialAT Serial1
// cookPotConfig defualtcookPotConfig_{
//    400, 18, 1};
//holdingPotConfig defualtholdingPotConfig_{
//    600, 18};
//mixerConfig defualtmixerConfig_{
//    50, 50, 50, 30 //HZ
//};
//
//pumpConfig defualtpumpConfig_{
//    50, 50};
//
generalConfig defualtgeneralConfig_{
    4, 2, 0, 5, -1, 4, 2, 0, 5, -1};

//cipConfig defualtcipConfig_{
//    0, 100, 18, 70, 30, 30, 30, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
//
//pumpSpeeds defualtPumpSpeeds{// Hz
//                             50, 50, 50};

//mixerSpeeds defualtMixerSpeeds{
//    50, 50, 50, 50};
bool CIPCook = false;
bool CIPHold = false;
bool transferRunning = false;
int statusCount = 0;

void setup()
{
  SerialMod2.begin(19200);
  while (!SerialMod2)
  {
  }
  Serial.begin(9600);
  page_ = MENU;
  while (!Serial)
  {
  }
  //////////////////////////////////////////////////////////////////////////////
  //  Serial2.begin(9600);
  //  while(!Serial2){
  //
  //  }
  //  //Serial1.flush();
  //  Serial2.flush();
  //  delay(200);
  //  Serial2.begin(9600);
  displayController_.initDisplay();
  initDisplayTouchCallbacks();
  delay(1000);
  displayController_.showPage(NexPage(0, 0, "STARTUP"));
  modbusController_.Standby();
  //initSD();

  sensorController_.initTempSensors(sensorController_.cookingTempSensor(), sensorController_.holdingTempSensor());
  gpioController_.initValveOutputs(valveOutputs_);
  gpioController_.initGpioInputs(generalInputs_);

  operationModeController_.changeMode(MODE_STANDBY, MODE_STANDBY);
  pinMode(2, INPUT);
  pinMode(50, INPUT);
prodHoldTimer=millis();
  //displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));

  //  mixersController_.mixerSpeeds_ = defualtMixerSpeeds;
  //  pumpsController_.pumpSpeeds_ = defualtPumpSpeeds;
  //  configController_.cipConfig_ = defualtcipConfig_;
  //  configController_.cookPotConfig_ = defualtcookPotConfig_;
  //  configController_.generalConfig_ = defualtgeneralConfig_;
  //  configController_.holdingPotConfig_ = defualtholdingPotConfig_;
  //  configController_.mixerConfig_ = defualtmixerConfig_;
  //  configController_.pumpConfig_ = defualtpumpConfig_;
//     if (!rtc.begin())
//     {
//       Serial.println("Couldn't find RTC");
//       Serial.flush();
//     }

  configController_.loadConfigEEPROM();
  gpioController_.Standby(valveOutputs_);
  modbusController_.setDirection();
  modbusController_.setSpeedRefs(configController_, mixerAddresses, pumpAddresses);
  pageTimer = millis();
  gsmInitTimer = millis();
  displayController_.showPage(NexPage(0, 0, "MENU"));
  delay(200);
  //displayController_.init_Menu();
  delay(200);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
  Serial.println("Startup");
  //  configController_.loadHeatingDefualt(defualtgeneralConfig_);
  //  configController_.generalConfig_.TEMP_CALIBRATE1=-1;
  //  configController_.saveConfigEEPROM();
}

void loop()
{

  if (rstFlag == 0)
  {
    if (millis() - gsmInitTimer > 60000)
    {
      SerialAT.println("AT+RST=1");
      rstFlag = 1;
    }
  }
  if (gsmInitFlag == false)
  {
    if (millis() - gsmInitTimer > 120000)
    {
      displayController_.showPage(NexPage(0, 0, "STARTUP"));
      //NexText(13,2,"t0").setText("Please Wait, GSM Connecting");
      page_ = MENU;
      GSMController_.GSMinit_();
      gsmInitFlag = true;
      pushingBoxSend("STARTUP");
      displayController_.showPage(NexPage(0, 0, "MENU"));
      displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
    }
  }

  nexLoop(nex_listen_list);
  // Serial.println(digitalRead(50));
  int prodTemp1 = sensorController_.getCookingTemp(sensorController_.cookingTempSensor());
  int prodTemp2 = sensorController_.getHoldingTemp(sensorController_.holdingTempSensor());
  nexLoop(nex_listen_list);
  bool cookLevelLowStatus = sensorController_.getCookingLowLevel(generalInputs_.cookingLowPin);
  bool holdingLevelLowStatus = sensorController_.getHoldingLowLevel(generalInputs_.holdingLowPin);
  nexLoop(nex_listen_list);
  bool holdingLevelHighStatus = sensorController_.getHoldingHighLevel(generalInputs_.holdingHighPin);
  bool proxStatus = sensorController_.getProxiSensor(generalInputs_.safetyProxy);
  tempCookBuff[tempAvCounter]=prodTemp1;
  tempHoldBuff[tempAvCounter]=prodTemp2;
  tempAvCounter++;
  if(tempAvCounter==5){
    tempAvCounter=3;
    for(int i = 0;i < 6 ; i++){
      tempCookAveraged+=tempCookBuff[i];
      tempHoldAveraged+=tempHoldBuff[i];
    }
    tempCookAveraged=tempCookAveraged/6;
    tempHoldAveraged=tempHoldAveraged/6;
    for(int i =0;i <3;i++){
      tempCookBuff[i] = tempCookBuff[i+3];
      tempHoldBuff[i] = tempHoldBuff[i+3];
    }
  }
  
  if (millis() - pageTimer > 1500)
  {
    ////////////////////////////////////////////////////////////////

    nexLoop(nex_listen_list);
    switch (page_)
    {
    case STATUS:
      statusCount++;
      if (statusCount == 8)
      {
        displayController_.updateStatusPage(configController_, generalInputs_, pumpsController_.pumpStates_, mixersController_.mixerStates_, gpioController_.valveStates_, gpioController_, tempCookAveraged, tempHoldAveraged, operationModeController_.currentMode);
        statusCount = 0;
      }
      nexLoop(nex_listen_list);
      break;
    case COOK_PROD:
      displayController_.updateCookPotPage(configController_, mixersController_.mixerStates_, tempCookAveraged, cookTimerController_.cookTimeLeft, cookProdRunning, prodCookPaused);
      nexLoop(nex_listen_list);
      break;
    case COOK_HOLD:
      displayController_.updateHoldingProdPage(configController_, mixersController_.mixerStates_, tempCookAveraged, tempHoldAveraged, pumpsController_.pumpStates_, cookHoldRunning, holdingLevelHighStatus, holdingLevelLowStatus);
      nexLoop(nex_listen_list);
      break;
    case CIP_PAGE:
      // displayController_.updateCIP(CIPSelect_, CIPBegin, cipTypeSelect, cipPrimerActive);
      nexLoop(nex_listen_list);
      break;
    case FILLING_PAGE:
      volumeFilled_.setValue(waterFilled / 1000);

      //volume2Fill_.setValue(configController_.cookPotConfig_.WATERC_SET);
      break;
    case PASSWORD_PAGE:

      break;
    default:
      break;
    }
    nexLoop(nex_listen_list);
    pageTimer = millis();
  }
  switch (operationModeController_.currentMode)
  {
  case MODE_STANDBY:
    break;
  case MODE_WATERFILL_BAGLOADING:
    if (tallyFill(configController_.cookPotConfig_.WATERC_SET))
    {
      displayController_.showPage(NexPage(0, 0, "WATERFILLDONE"));

      fillCompleteMenu.setText("Cook Production Page");
      operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
    }
    nexLoop(nex_listen_list);
    break;
  default:
    break;
  }

  nexLoop(nex_listen_list);
  if (cookProdRunning && cookComplete == false)
  {

    TempController_.controlCookTemp(configController_, gpioController_, configController_.cookPotConfig_.TEMPC_SET, tempHoldAveraged, valveOutputs_.steamValve1);
    if (((tempHoldAveraged - configController_.generalConfig_.TEMP_CALIBRATE1) >= (configController_.cookPotConfig_.TEMPC_SET - configController_.generalConfig_.BAND_GAP1 / 2)) && cookTimerController_.cookTimerActive == false)
    {
      cookTimerController_.startCookTimer(configController_.cookPotConfig_.TIME_SET);
      nexLoop(nex_listen_list);
    }
    if (cookTimerController_.cookTimerActive)
    {
      if (cookTimerController_.updateCookTimer(configController_.cookPotConfig_.TIME_SET) && cookComplete == false)
      {
        //gpioController_.Standby(valveOutputs_);
        displayController_.showPage(NexPage(0, 0, "BATCHCOMPLETE"));
        page_ = COOK_COMPLETE;
        nexLoop(nex_listen_list);
        cookComplete = true;
        cookProdRunning = false;
      }
    }
  }
  if (startTranfer == true && cookProdRunning == false)
  {
    if (transferRunning == true)
    {
      if (cookLevelLowStatus == false && holdingLevelHighStatus == true && operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
      { // levels fine
        gpioController_.openValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
        nexLoop(nex_listen_list);
        if (pumpsController_.pumpStates_.LOBE1 == false)
        {
          modbusController_.runVSD(4);
          pumpsController_.pumpStates_.LOBE1 = true;
        }
      }
      else
      {
        nexLoop(nex_listen_list);
        if (cookLevelLowStatus == true && page_ == COOK_COMPLETE)
        {
          batchCompStartTransfer.setText("COOK POT EMPTY");
        }
        else if (holdingLevelHighStatus == false && page_ == COOK_COMPLETE)
        {
          batchCompStartTransfer.setText("HOLDING POT FULL");
        }
        transferRunning = false;
        if (cookLevelLowStatus == true)
        {
          if (pumpsController_.pumpStates_.LOBE1 == true)
          {
            modbusController_.stopVSD(4);
            pumpsController_.pumpStates_.LOBE1 = false;
          }
          startTranfer = false;
        }
        else if (holdingLevelHighStatus == false)
        {
          if (pumpsController_.pumpStates_.LOBE1 == true)
          {
            modbusController_.stopVSD(4);
            pumpsController_.pumpStates_.LOBE1 = false;
          }
        }
        nexLoop(nex_listen_list);
        //gpioController_.closeValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
      }
    }
  }
  else if (cipRunning == false)
  {
    nexLoop(nex_listen_list);
    startTranfer = false;
    transferRunning = false;
    if (pumpsController_.pumpStates_.LOBE1 == true && page_ != OUTPUT_PAGE)
    {
      modbusController_.stopVSD(4);
      pumpsController_.pumpStates_.LOBE1 = false;
    }
    nexLoop(nex_listen_list);

    // gpioController_.closeValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
  }
  if (cookHoldRunning == true)
  {
    if(millis() - prodHoldTimer > 60000){
        prodHoldTimer=millis();
        pushingBoxSend("HOLD_RUNNING");
    }
    TempController_.controlHoldTemp(configController_, gpioController_, configController_.holdingPotConfig_.TEMPH_SET, tempHoldAveraged, valveOutputs_.steamValve2);
    if (sensorController_.getHoldingLowLevel(generalInputs_.holdingLowPin) == true)
    {
      if (pumpsController_.pumpStates_.LOBE2 == true)
      {
        modbusController_.stopVSD(pumpAddresses.LOBE2);
        pumpsController_.pumpStates_.LOBE2 = false;
      }
    }
  }
  nexLoop(nex_listen_list);
  if (sensorController_.getProxiSensor(generalInputs_.safetyProxy) == true || holdingLevelLowStatus == true)
  {
    if (mixersController_.mixerStates_.H1 == true)
    {
      modbusController_.stopVSD(mixerAddresses.H1);
      mixersController_.mixerStates_.H1 = false;
    }
  }
  nexLoop(nex_listen_list);
  if (cookLevelLowStatus == true)
  {
    if (CIPCook)
    {

      if (pumpsController_.pumpStates_.CIP == true)
      {
        modbusController_.stopVSD(pumpAddresses.CIP);
        pumpsController_.pumpStates_.CIP = false;
      }
    }
    if (pumpsController_.pumpStates_.LOBE1 == true)
    {
      modbusController_.stopVSD(pumpAddresses.LOBE1);
      pumpsController_.pumpStates_.LOBE1 = false;
    }

    if (mixersController_.mixerStates_.C1 == true)
    {
      modbusController_.stopVSD(mixerAddresses.C1);
      mixersController_.mixerStates_.C1 = false;
    }

    if (mixersController_.mixerStates_.C2 == true)
    {
      modbusController_.stopVSD(mixerAddresses.C2);
      mixersController_.mixerStates_.C2 = false;
    }

    if (mixersController_.mixerStates_.HS == true)
    {
      modbusController_.stopVSD(mixerAddresses.HS);
      mixersController_.mixerStates_.HS = false;
    }
  }
  nexLoop(nex_listen_list);
  if (holdingLevelLowStatus == true)
  {
    if (CIPHold == true)
    {
      if (pumpsController_.pumpStates_.CIP == true)
      {
        modbusController_.stopVSD(pumpAddresses.CIP);
        pumpsController_.pumpStates_.CIP = false;
      }
    }
    if (pumpsController_.pumpStates_.LOBE2 == true)
    {
      modbusController_.stopVSD(pumpAddresses.LOBE2);
      pumpsController_.pumpStates_.LOBE2 = false;
    }
  }
  
  nexLoop(nex_listen_list);
}

void menuHoldingProductionCallback(void *ptr)
{
  page_ = MENU;
  displayController_.showPage(NexPage(0, 0, "MENU"));
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void statusHoldingProductionCallback(void *ptr)
{
  tempAverageCounter = 0;
  tempHoldAv = 0;
  tempCookAv = 0;
  page_ = STATUS;
  displayController_.showPage(NexPage(0, 0, "STATUS"));
  NexText(1, 12, "t11").setText("LOBE 1");
  NexText(1, 13, "t12").setText("LOBE 2");
  NexText(1, 9, "t8").setText("MIXER C2");
  NexText(1, 8, "t7").setText("MIXER C2");
  NexText(1, 14, "t13").setText("LOBE 1");
  NexText(1, 15, "t14").setText("LOBE 2");
}
void holdProdMixerH1Callback(void *ptr)
{
  if (mixersController_.mixerStates_.H1 == true)
  {
    modbusController_.stopVSD(mixerAddresses.H1);
    mixersController_.mixerStates_.H1 = false;

    holdProdMixerH1.setText("OFF");
    holdProdMixerH1.Set_background_color_bco(63488);
  }
  else
  {
    modbusController_.runVSD(mixerAddresses.H1);
    mixersController_.mixerStates_.H1 = true;

    holdProdMixerH1.setText("ON");
    holdProdMixerH1.Set_background_color_bco(2016);
  }
}
void holdProdLOBE1Callback(void *ptr)
{
  if (pumpsController_.pumpStates_.LOBE2 == true)
  {
    modbusController_.stopVSD(pumpAddresses.LOBE2);
    pumpsController_.pumpStates_.LOBE2 = false;
    holdProdLOBE1.setText("OFF");
    holdProdLOBE1.Set_background_color_bco(63488);
  }
  else if (sensorController_.getHoldingLowLevel(generalInputs_.holdingLowPin) == false)
  {
    modbusController_.runVSD(pumpAddresses.LOBE2);
    pumpsController_.pumpStates_.LOBE2 = true;
    holdProdLOBE1.setText("ON");
    holdProdLOBE1.Set_background_color_bco(2016);
  }
}
void stopCIPCallback(void *ptr)
{
  modbusController_.stopVSD(pumpAddresses.CIP);
  pumpsController_.pumpStates_.CIP = false;
  modbusController_.stopVSD(pumpAddresses.LOBE1);
  pumpsController_.pumpStates_.LOBE1 = false;
  modbusController_.stopVSD(pumpAddresses.LOBE2);
  pumpsController_.pumpStates_.LOBE2 = false;
  gpioController_.Standby(valveOutputs_);
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  startCIP.Set_background_color_bco(50712);
  operationModeController_.currentModesOperating_.CIP_ACTIVE = false;
  cipRunning = false;
  CIPHold = false;
  CIPCook = false;
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
}
void CIPMenuCallback(void *ptr)
{
  page_ = MENU;
  displayController_.showPage(NexPage(0, 0, "MENU"));
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void programcipSaveCallback(void *ptr)
{

  uint32_t WATERRINSE_SET = 0;
  uint32_t WATERSANI_SET = 0;
  uint32_t TEMPRINSE_SET = 0;
  uint32_t TEMPSANI_SET = 0;

  uint32_t DVCPOT_SET = 0;
  uint32_t DVHPOT_SET = 0;
  uint32_t CIPPUMP_SET = 0;

  programcipRinseSet.getValue(&WATERRINSE_SET);
  delay(200);
  configController_.cipConfig_.WATERRINSE_SET = WATERRINSE_SET;

  programcipSaniSet.getValue(&WATERSANI_SET);
  delay(200);
  configController_.cipConfig_.WATERSANI_SET = WATERSANI_SET;

  programcipTempRinseSet.getValue(&TEMPRINSE_SET);
  delay(200);
  configController_.cipConfig_.TEMPRINSE_SET = TEMPRINSE_SET;

  programcipTempSaniSet.getValue(&TEMPSANI_SET);
  delay(200);
  configController_.cipConfig_.TEMPSANI_SET = TEMPSANI_SET;

  programcipDVC.getValue(&DVCPOT_SET);
  delay(200);
  configController_.cipConfig_.DVCPOT_SET = DVCPOT_SET;

  programcipDVH.getValue(&DVHPOT_SET);
  delay(200);
  configController_.cipConfig_.DVHPOT_SET = DVHPOT_SET;

  programcipPumpSet.getValue(&CIPPUMP_SET);
  delay(200);
  configController_.cipConfig_.CIPPUMP_SET = CIPPUMP_SET;

  configController_.saveConfigEEPROM();

  modbusController_.setSpeedRefs(configController_, mixerAddresses, pumpAddresses);

  page_ = MENU;
  displayController_.showPage(NexPage(0, 0, "MENU"));
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void programcipMenuCallback(void *ptr)
{
  page_ = MENU;
  displayController_.showPage(NexPage(0, 0, "MENU"));
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void programcipRinseTimePageCallback(void *ptr)
{
  displayController_.showPage(NexPage(0, 0, "CIPTIMES"));
  displayController_.updateCIPTimes(configController_);
}
void cipTimeSetSaveCallback(void *ptr)
{
  cipTimeSetSave.setText("Please Wait");
  uint32_t CIPR1 = 0;
  uint32_t CIPR2 = 0;
  uint32_t CIPR3 = 0;
  uint32_t CIPR4 = 0;
  uint32_t CIPR5 = 0;
  uint32_t CIPR6 = 0;
  uint32_t CIPR7 = 0;
  uint32_t CIPR8 = 0;
  uint32_t CIPR9 = 0;

  uint32_t CIPS1 = 0;
  uint32_t CIPS2 = 0;
  uint32_t CIPS3 = 0;
  uint32_t CIPS4 = 0;
  uint32_t CIPS5 = 0;
  uint32_t CIPS6 = 0;
  uint32_t CIPS7 = 0;
  uint32_t CIPS8 = 0;
  uint32_t CIPS9 = 0;

  NexNumber(5, 9, "n0").getValue(&CIPR1);
  delay(200);
  configController_.cipConfig_.CIP1RTIME_SET = CIPR1;

  NexNumber(5, 11, "n1").getValue(&CIPR2);
  delay(200);
  configController_.cipConfig_.CIP2RTIME_SET = CIPR2;

  NexNumber(5, 14, "n2").getValue(&CIPR3);
  delay(200);
  configController_.cipConfig_.CIP3RTIME_SET = CIPR3;

  NexNumber(5, 17, "n3").getValue(&CIPR4);
  delay(200);
  configController_.cipConfig_.CIP4RTIME_SET = CIPR4;

  NexNumber(5, 20, "n4").getValue(&CIPR5);
  delay(200);
  configController_.cipConfig_.CIP5RTIME_SET = CIPR5;

  NexNumber(5, 23, "n11").getValue(&CIPR6);
  delay(200);
  configController_.cipConfig_.CIP6RTIME_SET = CIPR6;

  NexNumber(5, 56, "n12").getValue(&CIPR7);
  delay(200);
  configController_.cipConfig_.CIP7RTIME_SET = CIPR7;

  NexNumber(5, 64, "n14").getValue(&CIPR8);
  delay(200);
  configController_.cipConfig_.CIP8RTIME_SET = CIPR8;

  NexNumber(5, 72, "n16").getValue(&CIPR9);
  delay(200);
  configController_.cipConfig_.CIP9RTIME_SET = CIPR9;

  NexNumber(5, 33, "n5").getValue(&CIPS1);
  delay(200);
  configController_.cipConfig_.CIP1STIME_SET = CIPS1;

  NexNumber(5, 35, "n6").getValue(&CIPS2);
  delay(200);
  configController_.cipConfig_.CIP2STIME_SET = CIPS2;

  NexNumber(5, 38, "n7").getValue(&CIPS3);
  delay(200);
  configController_.cipConfig_.CIP3STIME_SET = CIPS3;

  NexNumber(5, 41, "n8").getValue(&CIPS4);
  delay(200);
  configController_.cipConfig_.CIP4STIME_SET = CIPS4;

  NexNumber(5, 44, "n9").getValue(&CIPS5);
  delay(200);
  configController_.cipConfig_.CIP5STIME_SET = CIPS5;

  NexNumber(5, 47, "n10").getValue(&CIPS6);
  delay(200);
  configController_.cipConfig_.CIP6STIME_SET = CIPS6;

  NexNumber(5, 60, "n13").getValue(&CIPS7);
  delay(200);
  configController_.cipConfig_.CIP7STIME_SET = CIPS7;

  NexNumber(5, 68, "n15").getValue(&CIPS8);
  delay(200);
  configController_.cipConfig_.CIP8STIME_SET = CIPS8;

  NexNumber(5, 76, "n17").getValue(&CIPS9);
  delay(200);
  configController_.cipConfig_.CIP9STIME_SET = CIPS9;

  cipTimeSetSave.setText("SAVE");
}
void cipTimeSetBackCallback(void *ptr)
{
  displayController_.showPage(NexPage(0, 0, "PROGRAMCIP"));
  displayController_.updateProgramCIPPage(configController_);
}
void outputBackCallback(void *ptr)
{
  displayController_.showPage(NexPage(0, 0, "MAINTENENCE"));
  displayController_.updateMaintPage(configController_);
}

void bacthDoneMenuCallback(void *ptr)
{
  page_ = MENU;
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.showPage(NexPage(0, 0, "MENU"));
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void saniSelectCIPCallback(void *ptr)
{
  cipTypeSelect = 1;
  if (CIPSelect_ <= 9)
  {
    CIPSelect_ += 9;
  }
  rinseSelectCIP.Set_background_color_bco(63488);
  saniSelectCIP.Set_background_color_bco(2016);
}
void rinseSelectCIPCallback(void *ptr)
{
  cipTypeSelect = 0;
  if (CIPSelect_ > 9)
  {
    CIPSelect_ -= 9;
  }
  saniSelectCIP.Set_background_color_bco(63488);
  rinseSelectCIP.Set_background_color_bco(2016);
}
void fillCompleteMenuCallback(void *ptr)
{
  operationModeController_.currentModesOperating_.WATERFILL_RUNNING = false;
  page_ = COOK_PROD;
  displayController_.showPage(NexPage(0, 0, "PRODUCTIONCOOK"));
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
}

void startStopProdCookCallback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == true)
  {
    if ((CIPSelect_ == 1 || CIPSelect_ == 4 || CIPSelect_ == 6 || CIPSelect_ == 10 || CIPSelect_ == 13 || CIPSelect_ == 15))
    {
      if (cookProdRunning == false)
      {
        cookProdRunning = true;
       
        // CIPCook = true;
        startStopProdCook.Set_background_color_bco(2016);
        startStopProdCook.setText("STOP");
        //cookTimeReference = configController_.cookPotConfig_.TIME_SET;
        // displayController_.showPage(cookProdPage);

        // page_ = COOK_PROD;
        cookTimerController_.cookTimerActive = false;
        cookComplete = false;
         pushingBoxSend("COOK PROD START");
      }
      else if (cookProdRunning == true)
      {
        cookProdRunning = false;
        
        ///prodCookPaused = true;
        //cookTimeReference = cookTimerController_.cookTimeLeft;
        gpioController_.closeValve(valveOutputs_.steamValve1, gpioController_.valveStates_.steamValve1);
        startStopProdCook.Set_background_color_bco(63488);
        startStopProdCook.setText("START");
        pushingBoxSend("COOK PROD STOP");
      }
    }
  }
  else
  {
    if (cookProdRunning == false)
    {
      cookProdRunning = true;
      // CIPCook = true;
      startStopProdCook.Set_background_color_bco(2016);
      startStopProdCook.setText("STOP");
      //cookTimeReference = configController_.cookPotConfig_.TIME_SET;
      // displayController_.showPage(cookProdPage);

      // page_ = COOK_PROD;
      cookTimerController_.cookTimerActive = false;
      cookComplete = false;
    }
    else if (cookProdRunning == true)
    {
      cookProdRunning = false;
      //      /prodCookPaused = true;
      //cookTimeReference = cookTimerController_.cookTimeLeft;
      gpioController_.closeValve(valveOutputs_.steamValve1, gpioController_.valveStates_.steamValve1);
      startStopProdCook.Set_background_color_bco(63488);
      startStopProdCook.setText("START");
    }
  }
}
void startStopProdHoldCallback(void *ptr)
{
  if (cookHoldRunning == true)
  {
    cookHoldRunning = false;
    
    modbusController_.stopVSD(pumpAddresses.LOBE2);
    pumpsController_.pumpStates_.LOBE2 = false;
    gpioController_.closeValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
    gpioController_.closeValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
    gpioController_.closeValve(valveOutputs_.steamValve2, gpioController_.valveStates_.steamValve2);
    startStopProdHold.Set_background_color_bco(63488);
    startStopProdHold.setText("START");
    pushingBoxSend("PROD HOLD STOP");
  }
  else if (cookHoldRunning == false)
  {
    if (sensorController_.getHoldingLowLevel(generalInputs_.holdingLowPin) == true)
    {
      modbusController_.stopVSD(pumpAddresses.LOBE2);
      pumpsController_.pumpStates_.LOBE2 = false;
      
      startStopProdHold.Set_background_color_bco(63488);
      startStopProdHold.setText("HOLDING EMPTY");
      pushingBoxSend("PROD HOLD STOP");
    }
    else
    {
      modbusController_.runVSD(pumpAddresses.LOBE2);
      pumpsController_.pumpStates_.LOBE2 = true;
      cookHoldRunning = true;
      
      gpioController_.openValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
      gpioController_.openValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
      startStopProdHold.Set_background_color_bco(2016);
      startStopProdHold.setText("STOP");
      pushingBoxSend("PROD HOLD START");
    }
  }
}
void pumpPrimerCallback(void *ptr)
{
  if (cipPrimerActive)
  {
    cipPrimerActive = false;
    pumpPrimer.Set_background_color_bco(63488);
  }
  else
  {
    cipPrimerActive = true;
    pumpPrimer.Set_background_color_bco(2016);
  }
}
void drainCookCallback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_DRAIN_COOK_RUNNING == true)
  {
    operationModeController_.currentModesOperating_.CIP_DRAIN_COOK_RUNNING = false;
    gpioController_.closeValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
    gpioController_.closeValve(valveOutputs_.V18, gpioController_.valveStates_.V18);
    gpioController_.closeValve(valveOutputs_.V5, gpioController_.valveStates_.V5);
    drainCook.setText("COOK DRAIN");
    drainCook.Set_background_color_bco(63488);
  }
  else
  {
    operationModeController_.currentModesOperating_.CIP_DRAIN_COOK_RUNNING = true;
    gpioController_.openValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
    gpioController_.openValve(valveOutputs_.V18, gpioController_.valveStates_.V18);
    gpioController_.openValve(valveOutputs_.V5, gpioController_.valveStates_.V5);
    drainCook.setText("COOK DRAIN");
    drainCook.Set_background_color_bco(2016);
  }
}
void drainHoldCallback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_CYCLE_HOLD_RUNNING == true)
  {
    operationModeController_.currentModesOperating_.CIP_CYCLE_HOLD_RUNNING = false;
    gpioController_.closeValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
    gpioController_.closeValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
    gpioController_.closeValve(valveOutputs_.V17, gpioController_.valveStates_.V17);
    drainHold.setText("HOLD DRAIN");
    drainHold.Set_background_color_bco(63488);
  }
  else
  {
    operationModeController_.currentModesOperating_.CIP_CYCLE_HOLD_RUNNING = true;
    gpioController_.openValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
    gpioController_.openValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
    gpioController_.openValve(valveOutputs_.V17, gpioController_.valveStates_.V17);
    drainHold.setText("HOLD DRAIN");
    drainHold.Set_background_color_bco(2016);
  }
}

void passwordEnterCallback(void *ptr)
{
  char buf[1];
  String pass1 = "";
  NexText(14, 4, "t0").getText(buf, 1);

  pass1 += buf[0];
  NexText(14, 5, "t1").getText(buf, 1);
  pass1 += buf[0];
  NexText(14, 6, "t2").getText(buf, 1);
  pass1 += buf[0];
  NexText(14, 7, "t3").getText(buf, 1);
  pass1 += buf[0];
  Serial.print(pass1);
  // uint32_t number1 = 0;
  // uint32_t number2 = 0;
  // uint32_t number3 = 0;
  // uint32_t number4 = 0;
  // NexNumber(14, 2, "n0").getValue(&number1);
  // delay(100);
  // NexNumber(14, 3, "n1").getValue(&number2);
  // delay(100);
  // NexNumber(14, 4, "n2").getValue(&number3);
  // delay(100);
  // NexNumber(14, 5, "n3").getValue(&number4);
  // String pass = String(number1);
  // String pass1 = pass + number2 + number3 + number4;

  switch (operationModeController_.currentMode)
  {
  case MODE_MAINTENANCE:
    if (pass1 == "5846")
    {
      displayController_.showPage(NexPage(0, 0, "MAINTENENCE"));
      displayController_.updateMaintPage(configController_);

      page_ = DEFUALTPAGE;
    }

    break;
  case MODE_PROGRAMPOTS:
    if (pass1 == "5437")
    {
      displayController_.showPage(NexPage(0, 0, "PROGRAMPOTS"));
      displayController_.updateProgramPots(configController_);
      page_ = DEFUALTPAGE;
    }

    break;
  case MODE_PROGRAMCIP:
    if (pass1 == "5437")
    {
      displayController_.showPage(NexPage(0, 0, "PROGRAMCIP"));
      displayController_.updateProgramCIPPage(configController_);
      page_ = DEFUALTPAGE;
    }

    break;
  default:
    break;
  }
}
void passwordBackCallback(void *ptr)
{
  displayController_.showPage(NexPage(0, 0, "MENU"));
  page_ = MENU;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void runCIPdrainCCallback(void *ptr)
{
  // gpioController_.openValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
  // gpioController_.openValve(valveOutputs_.V18, gpioController_.valveStates_.V18);
  // gpioController_.openValve(valveOutputs_.V5, gpioController_.valveStates_.V5);

  if (gpioController_.valveStates_.V3 == true)
  {
    cookDraining = false;
    runCIPdrainC.Set_background_color_bco(63488);
    gpioController_.closeValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
    gpioController_.closeValve(valveOutputs_.V18, gpioController_.valveStates_.V18);
    gpioController_.closeValve(valveOutputs_.V5, gpioController_.valveStates_.V5);
  }
  else
  {
    cookDraining = true;
    runCIPdrainC.Set_background_color_bco(2016);
    gpioController_.openValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
    gpioController_.openValve(valveOutputs_.V18, gpioController_.valveStates_.V18);
    gpioController_.openValve(valveOutputs_.V5, gpioController_.valveStates_.V5);
  }
}
void runCIPdrainHCallback(void *ptr)
{
  // gpioController_.openValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
  // gpioController_.openValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
  // gpioController_.openValve(valveOutputs_.V17, gpioController_.valveStates_.V17);

  if (gpioController_.valveStates_.V15 == true)
  {
    holdDraining = false;
    runCIPdrainH.Set_background_color_bco(63488);
    gpioController_.closeValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
    gpioController_.closeValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
    gpioController_.closeValve(valveOutputs_.V17, gpioController_.valveStates_.V17);
  }
  else
  {
    holdDraining = true;
    runCIPdrainH.Set_background_color_bco(2016);
    gpioController_.openValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
    gpioController_.openValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
    gpioController_.openValve(valveOutputs_.V17, gpioController_.valveStates_.V17);
  }
}
void initDisplayTouchCallbacks()
{
  runCIPdrainC.attachPop(runCIPdrainCCallback, &runCIPdrainC);
  runCIPdrainH.attachPop(runCIPdrainHCallback, &runCIPdrainH);
  passwordEnter.attachPop(passwordEnterCallback, &passwordEnter);
  passwordBack.attachPop(passwordBackCallback, &passwordBack);
  pumpPrimer.attachPop(pumpPrimerCallback, &pumpPrimer);
  drainCook.attachPop(drainCookCallback, &drainCook);
  drainHold.attachPop(drainHoldCallback, &drainHold);
  enterStandbyMenu.attachPop(enterStandbyMenuCallback, &enterStandbyMenu);
  menuCookProdPage.attachPop(menuCookProdPageCallback, &menuCookProdPage);
  statusCookProdPage.attachPop(statusCookProdPageCallback, &statusCookProdPage);
  startStopProdHold.attachPop(startStopProdHoldCallback, &startStopProdHold);
  startStopProdCook.attachPop(startStopProdCookCallback, &startStopProdCook);
  rinseSelectCIP.attachPop(rinseSelectCIPCallback, &rinseSelectCIP);
  saniSelectCIP.attachPop(saniSelectCIPCallback, &saniSelectCIP);
  bacthDoneMenu.attachPop(bacthDoneMenuCallback, &bacthDoneMenu);
  cipRinse7.attachPop(cipRinse7Callback, &cipRinse7);
  cipRinse8.attachPop(cipRinse8Callback, &cipRinse8);
  cipRinse9.attachPop(cipRinse9Callback, &cipRinse9);
  // fillCompleteMixerC1.attachPop(fillCompleteMixerC1Callback, &fillCompleteMixerC1);
  // fillCompleteMixerC2.attachPop(fillCompleteMixerC2Callback, &fillCompleteMixerC2);
  // fillCompleteMixerHS.attachPop(fillCompleteMixerHSCallback, &fillCompleteMixerHS);
  fillCompleteMenu.attachPop(fillCompleteMenuCallback, &fillCompleteMenu);
  outputBack.attachPop(outputBackCallback, &outputBack);
  cipTimeSetBack.attachPop(cipTimeSetBackCallback, &cipTimeSetBack);
  cipTimeSetSave.attachPop(cipTimeSetSaveCallback, &cipTimeSetSave);
  programcipRinseTimePage.attachPop(programcipRinseTimePageCallback, &programcipRinseTimePage);
  programcipMenu.attachPop(programcipMenuCallback, &programcipMenu);
  programcipSave.attachPop(programcipSaveCallback, &programcipSave);
  stopCIP.attachPop(stopCIPCallback, &stopCIP);
  CIPMenu.attachPop(CIPMenuCallback, &CIPMenu);
  holdProdMixerH1.attachPop(holdProdMixerH1Callback, &holdProdMixerH1);
  holdProdLOBE1.attachPop(holdProdLOBE1Callback, &holdProdLOBE1);
  menuHoldingProduction.attachPop(menuHoldingProductionCallback, &menuHoldingProduction);
  statusHoldingProduction.attachPop(statusHoldingProductionCallback, &statusHoldingProduction);

  startCIP.attachPop(startCIPCallback, &startCIP);

  maintOutput.attachPop(maintOutputCallback, &maintOutput);
  maintMenu.attachPop(maintMenuCallback, &maintMenu);
  menuMode1.attachPop(menuMode1Callback_, &menuMode1);
  menuMode2.attachPop(menuMode2Callback_, &menuMode2);
  menuMode3.attachPop(menuMode3Callback_, &menuMode3);
  menuMode4.attachPop(menuMode4Callback_, &menuMode4);

  menuMode5.attachPop(menuMode5Callback_, &menuMode5);
  menuMode6.attachPop(menuMode6Callback_, &menuMode6);
  menuMode7.attachPop(menuMode7Callback_, &menuMode7);

  menuMode8.attachPop(menuMode8Callback_, &menuMode8);

  menuStatusButton.attachPop(menuStatusButtonCallback_, &menuStatusButton);
  menuEnterButton.attachPop(menuEnterButtonCallback_, &menuEnterButton);

  waterFillingFillValve.attachPop(waterFillingFillValveCallback, &waterFillingFillValve);

  waterFillingCancel.attachPop(waterFillingCancelCallback, &waterFillingCancel);

  batchCompStartTransfer.attachPop(batchCompStartTransferCallback, &batchCompStartTransfer);

  //CIP
  cipRinse1.attachPop(cipRinse1Callback, &cipRinse1);
  cipRinse2.attachPop(cipRinse2Callback, &cipRinse2);
  cipRinse3.attachPop(cipRinse3Callback, &cipRinse3);
  cipRinse4.attachPop(cipRinse4Callback, &cipRinse4);
  cipRinse5.attachPop(cipRinse5Callback, &cipRinse5);
  cipRinse6.attachPop(cipRinse6Callback, &cipRinse6);

  menuStatusPage.attachPop(menuStatusPageCallback, &menuStatusPage);

  mixerC1CookProd.attachPop(mixerC1CookProdCallback, &mixerC1CookProd);
  mixerC2CookProd.attachPop(mixerC2CookProdCallback, &mixerC2CookProd);
  mixerHSCookProd.attachPop(mixerHSCookProdCallback, &mixerHSCookProd);

  saveProgramPots.attachPop(saveProgramPotsCallback, &saveProgramPots);
  menuProgramPots.attachPop(menuProgramPotsCallback, &menuProgramPots);

  outputV1.attachPop(outputV1Callback, &outputV1);
  outputV2.attachPop(outputV2Callback, &outputV2);
  outputV3.attachPop(outputV3Callback, &outputV3);
  outputV4.attachPop(outputV4Callback, &outputV4);
  outputV5.attachPop(outputV5Callback, &outputV5);
  outputV6.attachPop(outputV6Callback, &outputV6);
  outputV7.attachPop(outputV7Callback, &outputV7);
  outputV8.attachPop(outputV8Callback, &outputV8);
  outputV9.attachPop(outputV9Callback, &outputV9);
  outputV10.attachPop(outputV10Callback, &outputV10);
  outputV11.attachPop(outputV11Callback, &outputV11);
  outputV12.attachPop(outputV12Callback, &outputV12);
  outputV13.attachPop(outputV13Callback, &outputV13);
  outputV14.attachPop(outputV14Callback, &outputV14);
  outputV15.attachPop(outputV15Callback, &outputV15);
  outputV16.attachPop(outputV16Callback, &outputV16);
  outputV17.attachPop(outputV17Callback, &outputV17);
  outputV18.attachPop(outputV18Callback, &outputV18);
  outputV19.attachPop(outputV19Callback, &outputV19);
  outputSteam1.attachPop(outputSteam1Callback, &outputSteam1);
  outputSteam2.attachPop(outputSteam2Callback, &outputSteam2);

  mixerC1.attachPop(mixerC1Callback, &mixerC1);
  mixerC2.attachPop(mixerC2Callback, &mixerC2);
  mixerH1.attachPop(mixerH1Callback, &mixerH1);
  mixerHS.attachPop(mixerHSCallback, &mixerHS);

  pumpCIP.attachPop(pumpCIPCallback, &pumpCIP);
  pumpL1.attachPop(pumpL1Callback, &pumpL1);
  pumpL2.attachPop(pumpL2Callback, &pumpL2);
}
void logFlowInterruptCallback()
{
  //Serial.println(flowMeterPulse);
  flowMeterPulse++;
}

void beginFill()
{
  Serial.println("BeginFill");
  //flowRateTimer = millis();
  //waterFilledTimer = millis();
  // NexNumber(10, 5, "n1").getValue(&waterC);
  delay(200);
  //  configController_.cookPotConfig_.WATERC_SET = waterC;
  waterFilled = 0;
  //currentFlowRate = 0;
  flowMeterPulse = 0;
  gpioController_.Standby(valveOutputs_);

  switch (operationModeController_.currentMode)
  {
  case MODE_WATERFILL_BAGLOADING:

    break;
  case MODE_CIP:
    attachInterrupt(digitalPinToInterrupt(2), logFlowInterruptCallback, RISING);

    if (CIPSelect_ == 2 || CIPSelect_ == 3 || CIPSelect_ == 5 || CIPSelect_ == 8 || CIPSelect_ == 9 || CIPSelect_ == 11 && cookProdRunning == false)
    {
      CIPCook = true;
      CIPHold = false;
      gpioController_.openValve(valveOutputs_.V1, gpioController_.valveStates_.V1);
    }
    else if ((CIPSelect_ == 1 || CIPSelect_ == 2 || CIPSelect_ == 4 || CIPSelect_ == 6 || CIPSelect_ == 7 || CIPSelect_ == 8 || CIPSelect_ == 9) && cookHoldRunning == false)
    {
      CIPHold = true;
      CIPCook = false;
      gpioController_.openValve(valveOutputs_.V2, gpioController_.valveStates_.V2);
    }
    else
    {
      operationModeController_.currentModesOperating_.CIP_ACTIVE = false;
      startCIP.Set_background_color_bco(50712);
    }
    break;
  default:
    break;
  }
}

bool tallyFill(uint32_t SetPoint)
{

  waterFilled = flowMeterPulse * pulseRateCoefficient;
  if (waterFilled >= SetPoint * 1000)
  {

    gpioController_.Standby(valveOutputs_);
    detachInterrupt(digitalPinToInterrupt(2));
    return true;
  }
  return false;
}

void outputV1Callback(void *ptr)
{
  if (gpioController_.valveStates_.V1 == true)
  {
    gpioController_.closeValve(valveOutputs_.V1, gpioController_.valveStates_.V1);
    outputV1.Set_background_color_bco(63488);
    outputV1.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V1, gpioController_.valveStates_.V1);
    outputV1.Set_background_color_bco(2016);
    outputV1.setText("OPEN");
  }
}
void outputV2Callback(void *ptr)
{
  if (gpioController_.valveStates_.V2 == true)
  {
    gpioController_.closeValve(valveOutputs_.V2, gpioController_.valveStates_.V2);
    outputV2.Set_background_color_bco(63488);
    outputV2.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V2, gpioController_.valveStates_.V2);
    outputV2.Set_background_color_bco(2016);
    outputV2.setText("OPEN");
  }
}
void outputV3Callback(void *ptr)
{
  if (gpioController_.valveStates_.V3 == true)
  {
    gpioController_.closeValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
    outputV3.Set_background_color_bco(63488);
    outputV3.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
    outputV3.Set_background_color_bco(2016);
    outputV3.setText("OPEN");
  }
}
void outputV4Callback(void *ptr)
{
  if (gpioController_.valveStates_.V4 == true)
  {
    gpioController_.closeValve(valveOutputs_.V4, gpioController_.valveStates_.V4);
    outputV4.Set_background_color_bco(63488);
    outputV4.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V4, gpioController_.valveStates_.V4);
    outputV4.Set_background_color_bco(2016);
    outputV4.setText("OPEN");
  }
}
void outputV5Callback(void *ptr)
{
  if (gpioController_.valveStates_.V5 == true)
  {
    gpioController_.closeValve(valveOutputs_.V5, gpioController_.valveStates_.V5);
    outputV5.Set_background_color_bco(63488);
    outputV5.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V5, gpioController_.valveStates_.V5);
    outputV5.Set_background_color_bco(2016);
    outputV5.setText("OPEN");
  }
}
void outputV6Callback(void *ptr)
{
  if (gpioController_.valveStates_.V6 == true)
  {
    gpioController_.closeValve(valveOutputs_.V6, gpioController_.valveStates_.V6);
    outputV6.Set_background_color_bco(63488);
    outputV6.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V6, gpioController_.valveStates_.V6);
    outputV6.Set_background_color_bco(2016);
    outputV6.setText("OPEN");
  }
}
void outputV7Callback(void *ptr)
{
  if (gpioController_.valveStates_.V7 == true)
  {
    gpioController_.closeValve(valveOutputs_.V7, gpioController_.valveStates_.V7);
    outputV7.Set_background_color_bco(63488);
    outputV7.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V7, gpioController_.valveStates_.V7);
    outputV7.Set_background_color_bco(2016);
    outputV7.setText("OPEN");
  }
}
void outputV8Callback(void *ptr)
{
  if (gpioController_.valveStates_.V8 == true)
  {
    gpioController_.closeValve(valveOutputs_.V8, gpioController_.valveStates_.V8);
    outputV8.Set_background_color_bco(63488);
    outputV8.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V8, gpioController_.valveStates_.V8);
    outputV8.Set_background_color_bco(2016);
    outputV8.setText("OPEN");
  }
}
void outputV9Callback(void *ptr)
{
  if (gpioController_.valveStates_.V9 == true)
  {
    gpioController_.closeValve(valveOutputs_.V9, gpioController_.valveStates_.V9);
    outputV9.Set_background_color_bco(63488);
    outputV9.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V9, gpioController_.valveStates_.V9);
    outputV9.Set_background_color_bco(2016);
    outputV9.setText("OPEN");
  }
}
void outputV10Callback(void *ptr)
{
  if (gpioController_.valveStates_.V10 == true)
  {
    gpioController_.closeValve(valveOutputs_.V10, gpioController_.valveStates_.V10);
    outputV10.Set_background_color_bco(63488);
    outputV10.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V10, gpioController_.valveStates_.V10);
    outputV10.Set_background_color_bco(2016);
    outputV10.setText("OPEN");
  }
}

void outputV11Callback(void *ptr)
{
  if (gpioController_.valveStates_.V11 == true)
  {
    gpioController_.closeValve(valveOutputs_.V11, gpioController_.valveStates_.V11);
    outputV11.Set_background_color_bco(63488);
    outputV11.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V11, gpioController_.valveStates_.V11);
    outputV11.Set_background_color_bco(2016);
    outputV11.setText("OPEN");
  }
}
void outputV12Callback(void *ptr)
{
  if (gpioController_.valveStates_.V12 == true)
  {
    gpioController_.closeValve(valveOutputs_.V12, gpioController_.valveStates_.V12);
    outputV12.Set_background_color_bco(63488);
    outputV12.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V12, gpioController_.valveStates_.V12);
    outputV12.Set_background_color_bco(2016);
    outputV12.setText("OPEN");
  }
}
void outputV13Callback(void *ptr)
{
  if (gpioController_.valveStates_.V13 == true)
  {
    gpioController_.closeValve(valveOutputs_.V13, gpioController_.valveStates_.V13);
    outputV13.Set_background_color_bco(63488);
    outputV13.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V13, gpioController_.valveStates_.V13);
    outputV13.Set_background_color_bco(2016);
    outputV13.setText("OPEN");
  }
}
void outputV14Callback(void *ptr)
{
  if (gpioController_.valveStates_.V14 == true)
  {
    gpioController_.closeValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
    outputV14.Set_background_color_bco(63488);
    outputV14.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V14, gpioController_.valveStates_.V14);
    outputV14.Set_background_color_bco(2016);
    outputV14.setText("OPEN");
  }
}
void outputV15Callback(void *ptr)
{
  if (gpioController_.valveStates_.V15 == true)
  {
    gpioController_.closeValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
    outputV15.Set_background_color_bco(63488);
    outputV15.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V15, gpioController_.valveStates_.V15);
    outputV15.Set_background_color_bco(2016);
    outputV15.setText("OPEN");
  }
}
void outputV16Callback(void *ptr)
{
  if (gpioController_.valveStates_.V16 == true)
  {
    gpioController_.closeValve(valveOutputs_.V16, gpioController_.valveStates_.V16);
    outputV16.Set_background_color_bco(63488);
    outputV16.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V16, gpioController_.valveStates_.V16);
    outputV16.Set_background_color_bco(2016);
    outputV16.setText("OPEN");
  }
}
void outputV17Callback(void *ptr)
{
  if (gpioController_.valveStates_.V17 == true)
  {
    gpioController_.closeValve(valveOutputs_.V17, gpioController_.valveStates_.V17);
    outputV17.Set_background_color_bco(63488);
    outputV17.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V17, gpioController_.valveStates_.V17);
    outputV17.Set_background_color_bco(2016);
    outputV17.setText("OPEN");
  }
}
void outputV18Callback(void *ptr)
{
  if (gpioController_.valveStates_.V18 == true)
  {
    gpioController_.closeValve(valveOutputs_.V18, gpioController_.valveStates_.V18);
    outputV18.Set_background_color_bco(63488);
    outputV18.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V18, gpioController_.valveStates_.V18);
    outputV18.Set_background_color_bco(2016);
    outputV18.setText("OPEN");
  }
}
void outputV19Callback(void *ptr)
{
  if (gpioController_.valveStates_.V19 == true)
  {
    gpioController_.closeValve(valveOutputs_.V19, gpioController_.valveStates_.V19);
    outputV19.Set_background_color_bco(63488);
    outputV19.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.V19, gpioController_.valveStates_.V19);
    outputV19.Set_background_color_bco(2016);
    outputV19.setText("OPEN");
  }
}
void outputSteam1Callback(void *ptr)
{
  if (gpioController_.valveStates_.steamValve1 == true)
  {
    gpioController_.closeValve(valveOutputs_.steamValve1, gpioController_.valveStates_.steamValve1);
    outputSteam1.Set_background_color_bco(63488);
    outputSteam1.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.steamValve1, gpioController_.valveStates_.steamValve1);
    outputSteam1.Set_background_color_bco(2016);
    outputSteam1.setText("OPEN");
  }
}
void outputSteam2Callback(void *ptr)
{
  if (gpioController_.valveStates_.steamValve2 == true)
  {
    gpioController_.closeValve(valveOutputs_.steamValve2, gpioController_.valveStates_.steamValve2);
    outputSteam2.Set_background_color_bco(63488);
    outputSteam2.setText("CLOSED");
  }
  else
  {
    gpioController_.openValve(valveOutputs_.steamValve2, gpioController_.valveStates_.steamValve2);
    outputSteam2.Set_background_color_bco(2016);
    outputSteam2.setText("OPEN");
  }
}

void menuStatusButtonCallback_(void *ptr)
{
  page_ = STATUS;
  tempAverageCounter = 0;
  tempHoldAv = 0;
  tempCookAv = 0;
  displayController_.showPage(NexPage(0, 0, "STATUS"));

}
void menuEnterButtonCallback_(void *ptr)
{

  operationModeController_.changeMode(menuModeSelected, operationModeController_.currentMode);
  switch (operationModeController_.currentMode)
  {

  case MODE_WATERFILL_BAGLOADING:

    beginFill();
    displayController_.showPage(NexPage(0, 0, "WATERFILLING"));
    page_ = FILLING_PAGE;
    break;
  case MODE_PRODUCTION_COOKPOT:
    tempAverageCounter = 0;
    displayController_.showPage(NexPage(0, 0, "PRODUCTIONCOOK"));
    tempHoldAv = 0;
    tempCookAv = 0;
    page_ = COOK_PROD;
    if (mixersController_.mixerStates_.C1 == true)
    {
      mixerC1CookProd.Set_background_color_bco(2016);
      mixerC1CookProd.setText("ON");
    }
    else
    {
      mixerC1CookProd.Set_background_color_bco(63488);
      mixerC1CookProd.setText("OFF");
    }

    if (mixersController_.mixerStates_.C2 == true)
    {
      mixerC2CookProd.Set_background_color_bco(2016);
      mixerC2CookProd.setText("ON");
    }
    else
    {
      mixerC2CookProd.Set_background_color_bco(63488);
      mixerC2CookProd.setText("OFF");
    }

    if (mixersController_.mixerStates_.HS == true)
    {
      mixerHSCookProd.Set_background_color_bco(2016);
      mixerHSCookProd.setText("ON");
    }
    else
    {
      mixerHSCookProd.Set_background_color_bco(63488);
      mixerHSCookProd.setText("OFF");
    }

    if (cookProdRunning == true)
    {
      startStopProdCook.Set_background_color_bco(2016);

      startStopProdCook.setText("STOP");
    }
    else if (cookProdRunning == false && prodCookPaused == true)
    {
      startStopProdCook.Set_background_color_bco(63488);

      startStopProdCook.setText("CONTINUE");
    }
    else if (cookProdRunning == false && prodCookPaused == false)
    {
      startStopProdCook.Set_background_color_bco(63488);

      startStopProdCook.setText("START");
    }

    break;
  case MODE_PRODUCTION_HOLDINGPOT:
    tempAverageCounter = 0;
    tempHoldAv = 0;
    tempCookAv = 0;
    displayController_.showPage(NexPage(0, 0, "PRODUCTIONHOLD"));
    if (pumpsController_.pumpStates_.LOBE2 == true)
    {
      holdProdLOBE1.setText("ON");
      holdProdLOBE1.Set_background_color_bco(2016);
    }
    else
    {
      holdProdLOBE1.setText("OFF");
      holdProdLOBE1.Set_background_color_bco(63488);
    }
    if (mixersController_.mixerStates_.H1 == true)
    {
      holdProdMixerH1.setText("ON");
      holdProdMixerH1.Set_background_color_bco(2016);
    }
    else
    {
      holdProdMixerH1.setText("OFF");
      holdProdMixerH1.Set_background_color_bco(63488);
    }
    if (cookHoldRunning == true)
    {
      startStopProdHold.setText("STOP");
      startStopProdHold.Set_background_color_bco(2016);
    }
    else
    {
      startStopProdHold.setText("START");
      startStopProdHold.Set_background_color_bco(63488);
    }

    page_ = COOK_HOLD;
    break;
  case MODE_CIP:
    displayController_.showPage(NexPage(0, 0, "RUNCIP"));
    page_ = CIP_PAGE;
    displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
    if (cookDraining == true)
    {
      runCIPdrainC.Set_background_color_bco(2016);
    }
    else
    {
      runCIPdrainC.Set_background_color_bco(63488);
    }

    if (holdDraining == true)
    {

      runCIPdrainH.Set_background_color_bco(2016);
    }
    else
    {
      runCIPdrainH.Set_background_color_bco(63488);
    }
    break;
    /////////////////
  case MODE_PROGRAMPOTS:

    ////////////////////////////

    // displayController_.showPage(programPotsPage);
    // displayController_.updateProgramPots(configController_);
    // page_ = DEFUALTPAGE;
    displayController_.showPage(NexPage(0, 0, "PASSWORD"));

    page_ = PASSWORD_PAGE;
    break;
  case MODE_PROGRAMCIP:

    // displayController_.showPage(programCIPPage);
    // displayController_.updateProgramCIPPage(configController_);
    ////////////////////////////
    displayController_.showPage(NexPage(0, 0, "PASSWORD"));

    page_ = PASSWORD_PAGE;
    break;
  case MODE_MAINTENANCE:

    // displayController_.showPage(maintPage);
    // displayController_.updateMaintPage(configController_);
    // page_ = DEFUALTPAGE;
    ////////////////////////////

    displayController_.showPage(NexPage(0, 0, "PASSWORD"));

    page_ = PASSWORD_PAGE;
    break;
  case MODE_RESET:
    softwareReset(WDTO_60MS);
    break;
  default:
    break;
  }
}

void menuMode1Callback_(void *ptr)
{
  menuModeSelected = 1;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuMode2Callback_(void *ptr)
{
  menuModeSelected = 2;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuMode3Callback_(void *ptr)
{
  menuModeSelected = 3;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuMode4Callback_(void *ptr)
{
  menuModeSelected = 4;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuMode5Callback_(void *ptr)
{
  menuModeSelected = 5;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuMode6Callback_(void *ptr)
{
  menuModeSelected = 6;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuMode7Callback_(void *ptr)
{
  menuModeSelected = 7;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuMode8Callback_(void *ptr)
{
  menuModeSelected = 8;
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}

void waterFillingFillValveCallback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.WATERFILL_RUNNING == false)
  {
    uint32_t waterC = 0;
    operationModeController_.currentModesOperating_.WATERFILL_RUNNING = true;
    NexNumber(10, 5, "n1").getValue(&waterC);
    delay(200);
    configController_.cookPotConfig_.WATERC_SET = waterC;
    waterFilled = 0;
    //currentFlowRate = 0;
    flowMeterPulse = 0;
    gpioController_.Standby(valveOutputs_);
    attachInterrupt(digitalPinToInterrupt(2), logFlowInterruptCallback, RISING);
    gpioController_.openValve(valveOutputs_.V1, gpioController_.valveStates_.V1);
    waterFillingFillValve.setText("STOP FILL");
    waterFillingFillValve.Set_background_color_bco(2016);
  }
  else
  {
    if (gpioController_.valveStates_.V1 == true)
    {
      gpioController_.closeValve(valveOutputs_.V1, gpioController_.valveStates_.V1);
      detachInterrupt(digitalPinToInterrupt(2));
      waterFillingFillValve.setText("START FILL");
      waterFillingFillValve.Set_background_color_bco(63488);
    }
    else
    {
      attachInterrupt(digitalPinToInterrupt(2), logFlowInterruptCallback, RISING);
      gpioController_.openValve(valveOutputs_.V1, gpioController_.valveStates_.V1);
      waterFillingFillValve.setText("STOP FILL");
      waterFillingFillValve.Set_background_color_bco(2016);
    }
  }
}
void waterFillingCancelCallback(void *ptr)
{
  operationModeController_.currentModesOperating_.WATERFILL_RUNNING = false;
  gpioController_.closeValve(valveOutputs_.V1, gpioController_.valveStates_.V1);
  displayController_.showPage(NexPage(0, 0, "MENU"));
  page_ = MENU;
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void batchCompStartTransferCallback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == true)
  {

    batchCompStartTransfer.setText("CIP IN PROGRESS");
  }
  else
  {

    if (sensorController_.getCookingLowLevel(generalInputs_.cookingLowPin) != true && sensorController_.getHoldingHighLevel(generalInputs_.holdingHighPin) != false)
    {
      if (pumpsController_.pumpStates_.LOBE1 == true)
      {
        modbusController_.stopVSD(4);
        pumpsController_.pumpStates_.LOBE1 = false;
        batchCompStartTransfer.Set_background_color_bco(63488);
        startTranfer = false;
        gpioController_.closeValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
        transferRunning = false;
        batchCompStartTransfer.setText("START TRANSFER TO HOLDING POT?");
      }
      else
      {
        startTranfer = true;
        gpioController_.openValve(valveOutputs_.V3, gpioController_.valveStates_.V3);
        modbusController_.runVSD(4);
        pumpsController_.pumpStates_.LOBE1 = true;
        batchCompStartTransfer.Set_background_color_bco(2016);
        transferRunning = true;
        batchCompStartTransfer.setText("STOP TRANSFER TO HOLDING POT?");
      }
    }
    else if (sensorController_.getCookingLowLevel(generalInputs_.cookingLowPin) == true)
    {
      startTranfer = false;
      batchCompStartTransfer.Set_background_color_bco(63488);
      transferRunning = false;
      batchCompStartTransfer.setText("COOK POT EMPTY");
    }
    else if (sensorController_.getHoldingHighLevel(generalInputs_.holdingHighPin) == false)
    {
      startTranfer = false;
      batchCompStartTransfer.Set_background_color_bco(63488);
      transferRunning = false;
      batchCompStartTransfer.setText("HOLD POT FULL");
    }
  }
}

void maintOutputCallback(void *ptr)
{
  page_ = OUTPUT_PAGE;
  displayController_.showPage(NexPage(0, 0, "OUTPUT"));
  displayController_.updateOutputPage(gpioController_, mixersController_, pumpsController_, generalInputs_);
  NexText(9, 28, "t27").setText("LOBE 1");
  NexText(9, 28, "t27").setText("LOBE 2");
}

void maintMenuCallback(void *ptr)
{
  maintMenu.setText("Saving...");
  char bfr[2];
  String buff = "";
  int val = 0;

  NexText(8, 32, "t1").getText(bfr, 2);
  Serial.println(bfr);

  if (bfr[0] == '-')
  {
    buff = bfr[1];
    val = buff.toInt();
    Serial.println(val * -1);
    configController_.generalConfig_.BAND_OFFSET1 = val * -1;
  }
  else
  {
    buff = bfr[0];
    val = buff.toInt();
    Serial.println(val);
    configController_.generalConfig_.BAND_OFFSET1 = val;
  }

  NexText(8, 34, "t13").getText(bfr, 2);
  if (bfr[0] == '-')
  {
    buff = bfr[1];
    val = buff.toInt();
    Serial.println(val * -1);
    configController_.generalConfig_.BAND_OFFSET2 = val * -1;
  }
  else
  {
    buff = bfr[0];
    val = buff.toInt();
    Serial.println(val);
    configController_.generalConfig_.BAND_OFFSET2 = val;
  }

  NexText(8, 33, "t7").getText(bfr, 3);

  if (bfr[0] == '-')
  {
    buff = bfr[1];
    val = buff.toInt();
    Serial.println(val * -1);
    configController_.generalConfig_.TEMP_CALIBRATE1 = val * -1;
  }
  else
  {
    buff = bfr[0];
    val = buff.toInt();
    Serial.println(val);
    configController_.generalConfig_.TEMP_CALIBRATE1 = val;
  }
  NexText(8, 35, "t14").getText(bfr, 3);

  if (bfr[0] == '-')
  {
    buff = bfr[1];
    val = buff.toInt();
    Serial.println(val * -1);
    configController_.generalConfig_.TEMP_CALIBRATE2 = val * -1;
  }
  else
  {
    buff = bfr[0];
    val = buff.toInt();
    Serial.println(val);
    configController_.generalConfig_.TEMP_CALIBRATE2 = val;
  }

  uint32_t BAND_GAP1 = 0;
  uint32_t WIDTH1 = 0;
  uint32_t BAND_OFFSET1 = 0;
  uint32_t DELAY_TIME1 = 0;

  uint32_t TEMP_CALIBRATE1 = 0;
  uint32_t BAND_GAP2 = 0;
  uint32_t WIDTH2 = 0;

  uint32_t BAND_OFFSET2 = 0;
  uint32_t DELAY_TIME2 = 0;
  uint32_t TEMP_CALIBRATE2 = 0;

  maintBandGap1.getValue(&BAND_GAP1);
  delay(200);
  configController_.generalConfig_.BAND_GAP1 = BAND_GAP1;

  //  maintWidth1.getValue(&WIDTH1);
  delay(200);
  configController_.generalConfig_.WIDTH1 = WIDTH1;

  // maintOffset1.getValue(&BAND_OFFSET1);
  // delay(200);
  // configController_.generalConfig_.BAND_OFFSET1 = BAND_OFFSET1;

  maintDelay1.getValue(&DELAY_TIME1);
  delay(200);
  configController_.generalConfig_.DELAY_TIME1 = DELAY_TIME1;

  // maintCali1.getValue(&TEMP_CALIBRATE1);
  // delay(200);
  // configController_.generalConfig_.TEMP_CALIBRATE1 = TEMP_CALIBRATE1;

  maintBandGap2.getValue(&BAND_GAP2);
  delay(200);
  configController_.generalConfig_.BAND_GAP2 = BAND_GAP2;

  //  maintWidth2.getValue(&WIDTH2);
  delay(200);
  configController_.generalConfig_.WIDTH2 = WIDTH2;

  // maintOffset2.getValue(&BAND_OFFSET2);
  // delay(200);
  // configController_.generalConfig_.BAND_OFFSET2 = BAND_OFFSET2;

  maintDelay2.getValue(&DELAY_TIME2);
  delay(200);
  configController_.generalConfig_.DELAY_TIME2 = DELAY_TIME2;

  // maintCali2.getValue(&TEMP_CALIBRATE2);
  // delay(200);
  // configController_.generalConfig_.TEMP_CALIBRATE2 = TEMP_CALIBRATE2;

  page_ = MENU;
  maintMenu.setText("MENU");
  displayController_.showPage(NexPage(0, 0, "MENU"));
  configController_.saveConfigEEPROM();
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void saveProgramPotsCallback(void *ptr)
{
  saveProgramPots.setText("Please Wait..");
  uint32_t waterC = 0;
  uint32_t waterH = 0;
  uint32_t TempC = 0;
  uint32_t time = 0;
  uint32_t TempH = 0;
  uint32_t mixerC1 = 0;
  uint32_t MixerC2 = 0;
  uint32_t MixerHS = 0;
  uint32_t MixerH1 = 0;
  uint32_t Lobe1 = 0;
  uint32_t Lobe2 = 0;

  // progrmaPotsWaterH.getValue(&waterH);
  delay(200);
  configController_.holdingPotConfig_.WATERH_SET = waterH;
  progrmaPotsTempC.getValue(&TempC);
  delay(200);
  configController_.cookPotConfig_.TEMPC_SET = TempC;
  progrmaPotsTimeSet.getValue(&time);
  delay(200);
  configController_.cookPotConfig_.TIME_SET = time;
  progrmaPotsTempH.getValue(&TempH);
  delay(200);
  configController_.holdingPotConfig_.TEMPH_SET = TempH;
  progrmaPotsMixerC1.getValue(&mixerC1);
  delay(200);
  configController_.mixerConfig_.MIXERC1_SET = mixerC1;
  progrmaPotsMixerC2.getValue(&MixerC2);
  delay(200);
  configController_.mixerConfig_.MIXERC2_SET = MixerC2;
  progrmaPotsMixerHS.getValue(&MixerHS);
  delay(200);
  configController_.mixerConfig_.MIXERHS_SET = MixerHS;
  progrmaPotsMixerH1.getValue(&MixerH1);
  delay(200);
  configController_.mixerConfig_.MIXERH1_SET = MixerH1;
  progrmaPotsLobe1.getValue(&Lobe1);
  delay(200);
  configController_.pumpConfig_.LOBE1_SET = Lobe1;
  progrmaPotsLobe2.getValue(&Lobe2);
  delay(200);
  configController_.pumpConfig_.LOBE2_SET = Lobe2;

  configController_.saveConfigEEPROM();

  page_ = MENU;
  modbusController_.setSpeedRefs(configController_, mixerAddresses, pumpAddresses);
  saveProgramPots.setText("SAVE");
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.showPage(NexPage(0, 0, "MENU"));
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void menuProgramPotsCallback(void *ptr)
{
  page_ = MENU;
  displayController_.showPage(NexPage(0, 0, "MENU"));
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void mixerHSCookProdCallback(void *ptr)
{
  if (mixersController_.mixerStates_.HS == true)
  {
    modbusController_.stopVSD(1);
    mixersController_.mixerStates_.HS = false;

    mixerHSCookProd.setText("OFF");
    mixerHSCookProd.Set_background_color_bco(63488);
  }
  else
  {
    modbusController_.runVSD(1);
    mixersController_.mixerStates_.HS = true;
    mixerHSCookProd.setText("ON");
    mixerHSCookProd.Set_background_color_bco(2016);
  }

  //displayController_.updateCookPotPage(configController_, mixersController_.mixerStates_, sensorController_.getCookingTemp(sensorController_.cookingTempSensor()), cookTimerController_.cookTimeLeft);
}
void mixerC2CookProdCallback(void *ptr)
{
  if (mixersController_.mixerStates_.C2 == true)
  {
    modbusController_.stopVSD(7);
    mixersController_.mixerStates_.C2 = false;
    mixerC2CookProd.setText("OFF");
    mixerC2CookProd.Set_background_color_bco(63488);
  }
  else
  {
    modbusController_.runVSD(7);
    mixersController_.mixerStates_.C2 = true;
    mixerC2CookProd.setText("ON");
    mixerC2CookProd.Set_background_color_bco(2016);
  }
  //displayController_.updateCookPotPage(configController_, mixersController_.mixerStates_, sensorController_.getCookingTemp(sensorController_.cookingTempSensor()), cookTimerController_.cookTimeLeft);
}
void mixerC1CookProdCallback(void *ptr)
{
  if (mixersController_.mixerStates_.C1 == true)
  {
    modbusController_.stopVSD(2);
    mixersController_.mixerStates_.C1 = false;
    mixerC1CookProd.setText("OFF");
    mixerC1CookProd.Set_background_color_bco(63488);
  }
  else
  {
    modbusController_.runVSD(2);
    mixersController_.mixerStates_.C1 = true;
    mixerC1CookProd.setText("ON");
    mixerC1CookProd.Set_background_color_bco(2016);
  }
  // displayController_.updateCookPotPage(configController_, mixersController_.mixerStates_, sensorController_.getCookingTemp(sensorController_.cookingTempSensor()), cookTimerController_.cookTimeLeft);
}
void statusCookProdPageCallback(void *ptr)
{
  page_ = STATUS;
  tempAverageCounter = 0;
  tempHoldAv = 0;
  tempCookAv = 0;
  displayController_.showPage(NexPage(0, 0, "STATUS"));
  NexText(1, 12, "t11").setText("LOBE 1");
  NexText(1, 13, "t12").setText("LOBE 2");
  NexText(1, 9, "t8").setText("MIXER C2");
  NexText(1, 8, "t7").setText("MIXER C2");
  NexText(1, 14, "t13").setText("LOBE 1");
  NexText(1, 15, "t14").setText("LOBE 2");
}

void menuCookProdPageCallback(void *ptr)
{
  page_ = MENU;
  displayController_.showPage(NexPage(0, 0, "MENU"));
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void enterStandbyMenuCallback(void *ptr)
{
  gpioController_.Standby(valveOutputs_);
  cookHoldRunning = false;
  cookProdRunning = false;
  cookComplete = false;
  operationModeController_.changeMode(MODE_STANDBY, operationModeController_.currentMode);
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
  modbusController_.Standby();
}
void menuStatusPageCallback(void *ptr)
{
  page_ = MENU;
  displayController_.showPage(NexPage(0, 0, "MENU"));
  displayController_.updateMenuPage(operationModeController_.currentMode, (cookProdRunning || cookHoldRunning));
}
void cipRinse1Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 1;
      cipRinse1.Set_background_color_bco(2016);
      // cipRinse2.Set_background_color_bco(
    }
    else
    {
      CIPSelect_ = 10;
      cipRinse1.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
}
void cipRinse2Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 2;
      cipRinse2.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 11;
      cipRinse2.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
}
void cipRinse3Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 3;
      cipRinse3.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 12;
      cipRinse3.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
  // if (cipTypeSelect == 0)
  // {
  //   // gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 3)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse3.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 3;
  //     cipRinse3.Set_background_color_bco(2016);
  //   }
  // }
  // else if (cipTypeSelect == 1)
  // {
  //   //gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 12)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse3.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 12;
  //     cipRinse3.Set_background_color_bco(2016);
  //   }
  // }
}
void cipRinse4Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 4;
      cipRinse4.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 13;
      cipRinse4.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
  // if (cipTypeSelect == 0)
  // {
  //   //gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 4)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse4.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 4;
  //     cipRinse4.Set_background_color_bco(2016);
  //   }
  // }
  // else if (cipTypeSelect == 1)
  // {
  //   // gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 13)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse4.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 13;
  //     cipRinse4.Set_background_color_bco(2016);
  //   }
  // }
}
void cipRinse5Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 5;
      cipRinse5.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 14;
      cipRinse5.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
  // if (cipTypeSelect == 0)
  // {
  //   // gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 5)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse5.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 5;
  //     cipRinse5.Set_background_color_bco(2016);
  //   }
  // }
  // else if (cipTypeSelect == 1)
  // {
  //   // gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 14)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse5.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 14;
  //     cipRinse5.Set_background_color_bco(2016);
  //   }
  // }
}
void cipRinse6Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 6;
      cipRinse6.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 15;
      cipRinse6.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
  // if (cipTypeSelect == 0)
  // {
  //   // gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 6)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse6.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 6;
  //     cipRinse6.Set_background_color_bco(2016);
  //   }
  // }
  // else if (cipTypeSelect == 1)
  // {
  //   // gpioController_.Standby(valveOutputs_);
  //   if (CIPSelect_ == 15)
  //   {
  //     CIPSelect_ = 0;
  //     cipRinse6.Set_background_color_bco(63488);
  //   }
  //   else if (CIPSelect_ == 0)
  //   {
  //     CIPSelect_ = 15;
  //     cipRinse6.Set_background_color_bco(2016);
  //   }
  // }
}

void cipRinse9Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 9;
      cipRinse9.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 18;
      cipRinse9.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
}
void cipRinse8Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 8;
      cipRinse8.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 17;
      cipRinse8.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
}
void cipRinse7Callback(void *ptr)
{
  if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cipTypeSelect == 0)
    {

      CIPSelect_ = 7;
      cipRinse7.Set_background_color_bco(2016);
    }
    else
    {
      CIPSelect_ = 16;
      cipRinse7.Set_background_color_bco(2016);
    }
  }
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
}

void startCIPCallback(void *ptr)
{
  if (startTranfer == false && transferRunning == false && operationModeController_.currentModesOperating_.CIP_ACTIVE == false)
  {
    if (cookProdRunning == false && cookHoldRunning == false&&(CIPSelect_ == 2 || CIPSelect_ == 7 || CIPSelect_ == 8 || CIPSelect_ == 9|| CIPSelect_ == 11 || CIPSelect_ == 16 || CIPSelect_ == 17 || CIPSelect_ == 18))
    {
      if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false && cipRunning == false)
      {
        operationModeController_.currentModesOperating_.CIP_ACTIVE = true;
       
        cipFillComplete = false;
        cipHeatComplete = false;
        cipHeatComplete = false;
        cipRunning = false;
        cipTimer = 0;
        cipDrainBegin = false;
        cipDrainTimer = 0;
        operationModeController_.changeMode(MODE_CIP, operationModeController_.currentMode);
        beginFill();
        startCIP.Set_background_color_bco(2016);
        pushingBoxSend("CIP START");
      }
    }
    else if (cookProdRunning == false && transferRunning == false && startTranfer == false && (CIPSelect_ == 3 || CIPSelect_ == 5||CIPSelect_ == 12 || CIPSelect_ == 16))
    {
      if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false && cipRunning == false)
      {
        operationModeController_.currentModesOperating_.CIP_ACTIVE = true;
        cipFillComplete = false;
        cipHeatComplete = false;
        cipHeatComplete = false;
        cipRunning = false;
        cipTimer = 0;
        cipDrainBegin = false;
        cipDrainTimer = 0;
        operationModeController_.changeMode(MODE_CIP, operationModeController_.currentMode);
        beginFill();
        startCIP.Set_background_color_bco(2016);
        pushingBoxSend("CIP START");
      }
    }
    else if (cookHoldRunning == false && startTranfer == false && transferRunning == false && (CIPSelect_ == 1 || CIPSelect_ == 4 || CIPSelect_ == 6 || CIPSelect_ == 10 || CIPSelect_ == 13 || CIPSelect_ == 15))
    {
      if (operationModeController_.currentModesOperating_.CIP_ACTIVE == false && cipRunning == false)
      {
        operationModeController_.currentModesOperating_.CIP_ACTIVE = true;
        cipFillComplete = false;
        cipHeatComplete = false;
        cipHeatComplete = false;
        cipRunning = false;
        cipTimer = 0;
        cipDrainBegin = false;
        cipDrainTimer = 0;
        operationModeController_.changeMode(MODE_CIP, operationModeController_.currentMode);
        beginFill();
        startCIP.Set_background_color_bco(2016);
        pushingBoxSend("CIP START");
      }
    }
  }

  
  displayController_.updateCIP(CIPSelect_, operationModeController_.currentModesOperating_.CIP_ACTIVE, cipTypeSelect, cipPrimerActive);
}

void mixerC1Callback(void *ptr)
{
  if (mixersController_.mixerStates_.C1 == true)
  {

    modbusController_.stopVSD(2);
    mixersController_.mixerStates_.C1 = false;
    mixerC1.Set_background_color_bco(63488);
    mixerC1.setText("OFF");
  }
  else
  {

    modbusController_.runVSD(2);
    mixersController_.mixerStates_.C1 = true;
    mixerC1.Set_background_color_bco(2016);
    mixerC1.setText("ON");
  }
}
void mixerC2Callback(void *ptr)
{
  if (mixersController_.mixerStates_.C2 == true)
  {

    modbusController_.stopVSD(7);
    mixersController_.mixerStates_.C2 = false;
    mixerC2.Set_background_color_bco(63488);
    mixerC2.setText("OFF");
  }
  else
  {

    modbusController_.runVSD(7);
    mixersController_.mixerStates_.C2 = true;

    mixerC2.Set_background_color_bco(2016);
    mixerC2.setText("ON");
  }
}
void mixerH1Callback(void *ptr)
{
  if (mixersController_.mixerStates_.H1 == true)
  {

    //mixersController_.turnOffMixer(3, node);
    modbusController_.stopVSD(6);
    mixersController_.mixerStates_.H1 = false;

    mixerH1.Set_background_color_bco(63488);
    mixerH1.setText("OFF");
  }
  else
  {
    modbusController_.runVSD(6);
    mixersController_.mixerStates_.H1 = true;
    //mixersController_.turnOnMixer(3, node);

    mixerH1.Set_background_color_bco(2016);
    mixerH1.setText("ON");
  }
}
void mixerHSCallback(void *ptr)
{
  if (mixersController_.mixerStates_.HS == true)
  {
    modbusController_.stopVSD(1);
    mixersController_.mixerStates_.HS = false;
    //mixersController_.turnOffMixer(4, node);

    mixerHS.Set_background_color_bco(63488);
    mixerHS.setText("OFF");
  }
  else
  {
    modbusController_.runVSD(1);
    mixersController_.mixerStates_.HS = true;
    //mixersController_.turnOnMixer(4, node);

    mixerHS.Set_background_color_bco(2016);
    mixerHS.setText("ON");
  }
}
void pumpCIPCallback(void *ptr)
{
  if (pumpsController_.pumpStates_.CIP == true)
  {
    modbusController_.stopVSD(3);
    pumpsController_.pumpStates_.CIP = false;
    //pumpsController_.turnOffPump(5, node);

    pumpCIP.Set_background_color_bco(63488);
    pumpCIP.setText("OFF");
  }
  else
  {
    modbusController_.runVSD(3);
    pumpsController_.pumpStates_.CIP = true;
    //pumpsController_.turnOnPump(5, node); /* code */

    pumpCIP.Set_background_color_bco(2016);
    pumpCIP.setText("ON");
  }
}
void pumpL1Callback(void *ptr)
{
  if (pumpsController_.pumpStates_.LOBE1 == true)
  {
    modbusController_.stopVSD(4);
    pumpsController_.pumpStates_.LOBE1 = false;
    //pumpsController_.turnOffPump(6, node);

    pumpL1.Set_background_color_bco(63488);
    pumpL1.setText("OFF");
  }
  else
  {
    modbusController_.runVSD(4);
    pumpsController_.pumpStates_.LOBE1 = true;
    //pumpsController_.turnOnPump(6, node); /* code */

    pumpL1.Set_background_color_bco(2016);
    pumpL1.setText("ON");
  }
  //displayController_.updateOutputPage(gpioController_, mixersController_, pumpsController_, generalInputs_);
}
void pumpL2Callback(void *ptr)
{
  if (pumpsController_.pumpStates_.LOBE2 == true)
  {
    modbusController_.stopVSD(5);
    pumpsController_.pumpStates_.LOBE2 = false;
    //pumpsController_.turnOffPump(7, node);

    pumpL2.Set_background_color_bco(63488);
    pumpL2.setText("OFF");
  }
  else
  {
    modbusController_.runVSD(5);
    pumpsController_.pumpStates_.LOBE2 = true;
    //pumpsController_.turnOnPump(7, node); /* code */

    pumpL2.Set_background_color_bco(2016);
    pumpL2.setText("ON");
  }
}
void initSD()
{
  //  bool success = false;
  //  for (size_t i = 0; (i < 2) && !success; ++i)
  //  {
  //    success = SD.begin(4);
  //    if (!success)
  //    {
  //      Serial.println("Could not initialize SD. retrying...");
  //    }
  //    delay(1000);
  //  }
  //  if (success)
  //  {
  //    Serial.println("SD Init successful");
  //  }
  //  delay(1000);
}

void logDataSD(String logEvent_)
{

  //  File dataFile = SD.open("DataLog.txt", FILE_WRITE);
  //  if (dataFile)
  //  {
  //    String Time = "RTC not active";
  //
  //    String date = "RTC not active";
  //    if (rtc.isrunning())
  //    {
  //      DateTime now = rtc.now();
  //
  //      String Time = String(now.hour());
  //      Time += ":" + String(now.minute()) + ":" + String(now.second());
  //      String date = String(now.day());
  //      date += "/" + String(now.month()) + "/" + String(now.year());
  //      Serial.println(date);
  //      Serial.println(Time);
  //    }
  //
  //    if (logEvent_ == "PROD")
  //    {
  //      dataFile.print("PRODUCTION START");
  //      dataFile.print(",");
  //      dataFile.print(date);
  //      dataFile.print(",");
  //      dataFile.print(Time);
  //      dataFile.print(",");
  //      dataFile.print(configController_.cookPotConfig_.TEMPC_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.cookPotConfig_.TIME_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.cookPotConfig_.WATERC_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.mixerConfig_.MIXERC1_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.mixerConfig_.MIXERC2_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.mixerConfig_.MIXERH1_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.mixerConfig_.MIXERHS_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.pumpConfig_.LOBE1_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.pumpConfig_.LOBE2_SET);
  //      dataFile.print(",");
  //      dataFile.println(configController_.cipConfig_.CIPPUMP_SET);
  //    }
  //    else if (logEvent_ == "CIP")
  //    {
  //      dataFile.print("CIP START");
  //      dataFile.print(",");
  //      dataFile.print(date);
  //      dataFile.print(",");
  //      dataFile.print(Time);
  //      dataFile.print(",");
  //      dataFile.print(configController_.cipConfig_.TEMPRINSE_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.cipConfig_.TEMPSANI_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.cipConfig_.WATERRINSE_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.cipConfig_.WATERSANI_SET);
  //      dataFile.print(",");
  //
  //      dataFile.print(configController_.pumpConfig_.LOBE1_SET);
  //      dataFile.print(",");
  //      dataFile.print(configController_.pumpConfig_.LOBE2_SET);
  //      dataFile.print(",");
  //      dataFile.println(configController_.cipConfig_.CIPPUMP_SET);
  //    }
  //
  //    dataFile.close();
  //  }
  //  else
  //  {
  //    Serial.println("Unable to open Datalogg file");
  //  }
}

void pushingBoxSend(String event)
{
  while (SerialAT.available() != 0)
  {
    Serial.write(SerialAT.read());
  }

  SerialAT.println("AT+CIPSHUT");

  String query = "GET /CookHold.php?";

  query += "EVENT=" + event;
  //query += "&TEMP_SET=123";
  query += "&TIME_SET=" + String(configController_.cookPotConfig_.TIME_SET);
  query += "&PROD_TEMP=" + String(sensorController_.getCookingTemp(sensorController_.cookingTempSensor()));
  if (event = "HOLD_RUNNING")
  {
    query += "&HOLD_RUNNING_LOG=1";
  }
  else
  {
    query += "&HOLD_RUNNING_LOG=0";
  }

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

void softwareReset(uint8_t prescaller)
{
  // start watchdog with the provided prescaller
  wdt_enable(prescaller);
  // wait for the prescaller time to expire
  // without sending the reset signal by using
  // the wdt_reset() method
  while (1)
  {
  }
}
