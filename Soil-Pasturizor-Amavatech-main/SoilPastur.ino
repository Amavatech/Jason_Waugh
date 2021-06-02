#include "DisplayController.h"
#include <avr/wdt.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "SensorController.h"
#include "OutputController.h"
#include "OperationModeController.h"
#include "TimerController.h"
#include "ConfigController.h"
#include "TempController.h"

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

uint32_t tempSoilAveraged = 0;
uint32_t tempBoxAveraged = 0;
uint32_t tempSoilBuff[6];
uint32_t tempBoxBuff[6];
uint32_t cookUpdateTimer = 0;
const int chipSelect = 53;

TempController tempController_;
TimerController timerController_;
ConfigController confingController_;
OperationModeController operationModeController_;
SensorController sensorController_;
OutputController outputController_;
DisplayController displayController_;

Page currPage_ = MENU_PAGE;

uint32_t pageUpdateTimer = 0;
uint32_t timerPauseReference_ = 0;

int levelInputs_[] = {32, 33};
int outputPins_[] = {40, 41, 42};
int tempAvCounter = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
  }
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
  }

  displayController_.menuModeSelect = 1;
  // put your setup code here, to run once:

  displayController_.init_();
  displayController_.showPage(startupPageNextion);
  initTouchCallbacks();
  operationModeController_.init();
  sensorController_.initTempSensors(sensorController_.soilTempSensor(), sensorController_.steamTempSensor());
  sensorController_.initLevelSensors(levelInputs_);

  outputController_.initOutputs(outputPins_);
  confingController_.loadConfigEEPROM();

  Serial.println();
  initSD();
  delay(5000);
  displayController_.showPage(menuPageNextion);
  displayController_.updatePage(MENU_PAGE_SELECT);

  pageUpdateTimer = millis();
  //timerPauseReference_ = confingController_.pastConfig_.TIME_SET;
  logEventSD("STARTUP");
}
void loop()
{
  nexLoop(nex_listen_list);
  bool levelA = sensorController_.getLevelA();
  bool levelB = sensorController_.getLevelB();
  ///////////////////////////////////////////////////////////
  int prodtemp = sensorController_.getSoilTemp(sensorController_.soilTempSensor()) + confingController_.maintConfig_.TEMP_CALIBRATE;
  int boxtemp = sensorController_.getSteamTemp(sensorController_.steamTempSensor()) + confingController_.maintConfig_.CALIBRATE2;

  bool elec1 = outputController_.outputStates[0];
  bool elec2 = outputController_.outputStates[1];
  bool elec3 = outputController_.outputStates[2];

  displayController_.box_temp = boxtemp;   //tempBoxAveraged;
  displayController_.prod_temp = prodtemp; //tempSoilAveraged;

  displayController_.elec1 = elec1;
  displayController_.elec2 = elec2;
  displayController_.elec3 = elec3;

  displayController_.temp_set = confingController_.pastConfig_.TEMP_SET;
  displayController_.wl1 = levelA;
  displayController_.wl2 = levelB;
  displayController_.timeleft = timerController_.cookTimeLeft;
  displayController_.timeset = confingController_.pastConfig_.TIME_SET;

  ///////////////////////////////////////////////////////////
  //   displayController_.display_OperationsController_.currentModesOperating_.PRODUCTION_PAUSED  = operationModeController_.currentModesOperating_.PRODUCTION_PAUSED;
  // displayController_.display_OperationsController_.currentModesOperating_.PRODUCTION_RUNNING = operationModeController_.currentModesOperating_.PRODUCTION_RUNNING;
  ///////////////////////////////////////////////////////////

  displayController_.prodRunning = operationModeController_.currentModesOperating_.PRODUCTION_RUNNING;

  displayController_.bandgap = confingController_.maintConfig_.BAND_GAP;
  displayController_.cali = confingController_.maintConfig_.TEMP_CALIBRATE;
  displayController_.delay = confingController_.maintConfig_.DELAY_TIME;
  displayController_.h1 = confingController_.maintConfig_.STOP_2;
  displayController_.h2 = confingController_.maintConfig_.STOP_4;
  displayController_.h3 = confingController_.maintConfig_.STOP_6;
  displayController_.offset = confingController_.maintConfig_.BAND_OFFSET;
  displayController_.cali2 = confingController_.maintConfig_.CALIBRATE2;
  displayController_.prodPaused = operationModeController_.currentModesOperating_.PRODUCTION_PAUSED;

  //    tempSoilBuff[tempAvCounter] = prodtemp;
  //  tempBoxBuff[tempAvCounter] = boxtemp;

  // tempAvCounter++;

  //  if (tempAvCounter == 6)
  //  {
  //    tempAvCounter = 3;
  //    for (int i = 0; i < 6; i++)
  //    {
  //      tempSoilAveraged += tempSoilBuff[i];
  //      tempBoxAveraged += tempBoxBuff[i];
  //    }
  //    tempSoilAveraged = tempSoilAveraged / 6;
  //    tempBoxAveraged = tempBoxAveraged / 6;
  //    for (int i = 0; i < 3; i++)
  //    {
  //      tempSoilBuff[i] = tempSoilBuff[i + 3];
  //      tempBoxBuff[i] = tempBoxBuff[i + 3];
  //    }
  //  }
  if (levelA == false || levelB == false)
  {
    displayController_.showPage(NexPage(0, 0, "WATERLOW"));
    currPage_ = WATERLOW;
    outputController_.standby();
    operationModeController_.init();
  }

  //Production
  if (operationModeController_.currentModesOperating_.PRODUCTION_RUNNING == true)
  {
    //    if (levelA == true && levelB == true)
    // needs to be true -- TESTING
    if (levelA == true && levelB == true)
    {
      tempController_.controlTemp(confingController_, outputController_, prodtemp);
      if ((prodtemp >= confingController_.pastConfig_.TEMP_SET) && operationModeController_.currentModesOperating_.PRODUCTION_HEATING == true)
      {
        operationModeController_.currentModesOperating_.PRODUCTION_HEATING = false;
        logEventSD("PRODUCTION_HEAT_FINISH");
        //operationModeController_.currentModesOperating_.PRODUCTION_COOKING=true;
      }
    }
    else
    {
      displayController_.showPage(NexPage(0, 0, "WATERLOW"));
      currPage_ = WATERLOW;
      outputController_.standby();
      operationModeController_.init();
    }

    if (operationModeController_.currentModesOperating_.PRODUCTION_HEATING == false && operationModeController_.currentModesOperating_.PRODUCTION_COOKING == false)
    {
      
      timerController_.startTimer(confingController_.pastConfig_.TIME_SET);
      operationModeController_.currentModesOperating_.PRODUCTION_COOKING = true;
      logEventSD("PRODUCTION_COOK_START");
      cookUpdateTimer = millis();
    }
    else if (operationModeController_.currentModesOperating_.PRODUCTION_COOKING == true)
    {
      if (millis() - cookUpdateTimer >= 60000)
      {
        logEventSD("PRODUCTION_UPDATE");
        cookUpdateTimer = millis();
      }
      if (timerController_.timerUpdate(confingController_.pastConfig_.TIME_SET))
      {
        outputController_.standby();
        operationModeController_.init();
        displayController_.showPage(completecyclePageNextion);
        logEventSD("PRODUCTION_CYCLE_FINISH");
      }
    }
  }

  if (millis() - pageUpdateTimer > 1500)
  {
    if (currPage_ != OUTPUT_PAGE && currPage_ != PROGRAM_PAGE && currPage_ != MAINT_PAGE)
    {
      displayController_.updatePage(currPage_);
    }

    pageUpdateTimer = millis();
    if (currPage_ == PASSWORD_PAGE)
    {
      checkPassword();
    }
  }
}

//STATUS

void menuStatusPageCallback(void *ptr)
{
  displayController_.showPage(menuPageNextion);
  displayController_.updatePage(MENU_PAGE_SELECT);

  currPage_ = MENU_PAGE;
}

//MENU

void menuCompletePageCallback(void *ptr)
{
  displayController_.showPage(menuPageNextion);
  displayController_.updatePage(MENU_PAGE_SELECT);

  currPage_ = MENU_PAGE;
}
void standbyModeMenuCallback(void *ptr)
{
  displayController_.menuModeSelect = 1;
  displayController_.updatePage(MENU_PAGE_SELECT);
  operationModeController_.init();
  outputController_.standby();
}
void heatingModeMenuCallback(void *ptr)
{
  displayController_.menuModeSelect = 2;
  displayController_.updatePage(MENU_PAGE_SELECT);

  displayController_.showPage(productionPageNextion);
  displayController_.updatePage(PRODUCTION_PAGE);

  if (operationModeController_.currentModesOperating_.PRODUCTION_RUNNING)
  {
    startStopProductionPage.setText("PAUSE");
    startStopProductionPage.Set_background_color_bco(2016);
  }
  else if (operationModeController_.currentModesOperating_.PRODUCTION_PAUSED)
  {
    startStopProductionPage.setText("RESUME");
    startStopProductionPage.Set_background_color_bco(63488);
  }
  else
  {
    startStopProductionPage.setText("START");
    startStopProductionPage.Set_background_color_bco(63488);
  }

  currPage_ = PRODUCTION_PAGE;
}
void programModeMenuCallback(void *ptr)
{
  displayController_.menuModeSelect = 3;
  displayController_.updatePage(MENU_PAGE_SELECT);

  displayController_.showPage(passwordPageNextion);

  currPage_ = PASSWORD_PAGE;
}
void maintenanceModeMenuCallback(void *ptr)
{
  displayController_.menuModeSelect = 4;
  displayController_.updatePage(MENU_PAGE_SELECT);

  displayController_.showPage(passwordPageNextion);

  currPage_ = PASSWORD_PAGE;
}
void resetMenuCallback(void *ptr)
{
  displayController_.menuModeSelect = 5;
  displayController_.updatePage(MENU_PAGE_SELECT);
  logEventSD("RESET");
  softwareReset(WDTO_60MS);
}
void enterMenuPageCallback(void *ptr)
{
  switch (displayController_.menuModeSelect)
  {
  case 1:
    // operationModeController_.init();
    // outputController_.standby();
    break;
  case 2:
    // displayController_.showPage(productionPageNextion);
    // displayController_.updatePage(PRODUCTION_PAGE);

    // if (operationModeController_.currentModesOperating_.PRODUCTION_RUNNING)
    // {
    //   startStopProductionPage.setText("PAUSE");
    //   startStopProductionPage.Set_background_color_bco(2016);
    // }
    // else if (operationModeController_.currentModesOperating_.PRODUCTION_PAUSED)
    // {
    //   startStopProductionPage.setText("RESUME");
    //   startStopProductionPage.Set_background_color_bco(63488);
    // }
    // else
    // {
    //   startStopProductionPage.setText("START");
    //   startStopProductionPage.Set_background_color_bco(63488);
    // }

    // currPage_ = PRODUCTION_PAGE;
    break;
  case 3:
    // displayController_.showPage(passwordPageNextion);

    // currPage_ = PASSWORD_PAGE;
    break;
  case 4:
    // displayController_.showPage(passwordPageNextion);

    // currPage_ = PASSWORD_PAGE;
    break;
  case 5:
    // logEventSD("RESET");
    // softwareReset(WDTO_60MS);
    break;

  default:
    break;
  }
}
void statusMenuPageCallback(void *ptr)
{
  displayController_.showPage(statusPageNextion);
  displayController_.updatePage(STATUS_PAGE);

  currPage_ = STATUS_PAGE;
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
void initTouchCallbacks()
{
  resetWaterlowPage.attachPop(resetWaterlowPageCallback, &resetWaterlowPage);
  menuCompletePage.attachPop(menuCompletePageCallback, &menuCompletePage);
  //STATUS
  menuStatusPage.attachPop(menuStatusPageCallback, &menuStatusPage);
  //MENU
  standbyModeMenu.attachPop(standbyModeMenuCallback, &standbyModeMenu);
  heatingModeMenu.attachPop(heatingModeMenuCallback, &heatingModeMenu);
  programModeMenu.attachPop(programModeMenuCallback, &programModeMenu);
  maintenanceModeMenu.attachPop(maintenanceModeMenuCallback, &maintenanceModeMenu);
  resetMenu.attachPop(resetMenuCallback, &resetMenu);
 // enterMenuPage.attachPop(enterMenuPageCallback, &enterMenuPage);
  statusMenuPage.attachPop(statusMenuPageCallback, &statusMenuPage);

  //PASSWORD
  enterPasswordPage.attachPop(enterPasswordPageCallback, &enterPasswordPage);
  menuPasswordPage.attachPop(menuPasswordPageCallback, &menuPasswordPage);

  //PRODUCTION
  menuProductionPage.attachPop(menuProductionPageCallback, &menuProductionPage);
  statusProductionPage.attachPop(statusProductionPageCallback, &statusProductionPage);
  startStopProductionPage.attachPop(startStopProductionPageCallback, &startStopProductionPage);
  resetProductionPage.attachPop(resetProductionPageCallback, &resetProductionPage);

  //PROGRAM
  menuProgramPage.attachPop(menuProgramPageCallback, &menuProgramPage);
  saveProgramPage.attachPop(saveProgramPageCallback, &saveProgramPage);
  //MAINT
  menuMaintenancePage.attachPop(menuMaintenancePageCallback, &menuMaintenancePage);
  saveMaintenancePage.attachPop(saveMaintenancePageCallback, &saveMaintenancePage);
  outputMaintenancePage.attachPop(outputMaintenancePageCallback, &outputMaintenancePage);
  //OUTPUT
  elec1OutputPage.attachPop(elec1OutputPageCallback, &elec1OutputPage);
  elec2OutputPage.attachPop(elec2OutputPageCallback, &elec2OutputPage);
  elec3OutputPage.attachPop(elec3OutputPageCallback, &elec3OutputPage);
  backOutputPage.attachPop(backOutputPageCallback, &backOutputPage);
}

//OUTPUT

void elec1OutputPageCallback(void *ptr)
{
  if (outputController_.outputStates[0] == true)
  {

    outputController_.turnOff(0, outputPins_[0]);
    //    elec1OutputPage.Set_background_color_bco(63488); //red
    //    elec1OutputPage.setText("H1 OFF");
  }
  else
  {
    outputController_.turnOn(0, outputPins_[0]);
    //    elec1OutputPage.Set_background_color_bco(2016); //green
    //    elec1OutputPage.setText("H1 ON");
  }
  displayController_.elec1 = outputController_.outputStates[0];
  displayController_.updatePage(OUTPUT_PAGE);
}
void elec2OutputPageCallback(void *ptr)
{
  if (outputController_.outputStates[1] == true)
  {

    outputController_.turnOff(1, outputPins_[1]);
    //    elec2OutputPage.Set_background_color_bco(63488); //red
    //    elec2OutputPage.setText("H2 OFF");
  }
  else
  {
    outputController_.turnOn(1, outputPins_[1]);
    //    elec2OutputPage.Set_background_color_bco(2016); //green
    //    elec2OutputPage.setText("H2 ON");
  }

  displayController_.elec2 = outputController_.outputStates[1];
  displayController_.updatePage(OUTPUT_PAGE);
}
void elec3OutputPageCallback(void *ptr)
{
  if (outputController_.outputStates[2] == true)
  {

    outputController_.turnOff(2, outputPins_[2]);
    //   elec3OutputPage.Set_background_color_bco(63488); //red
    //    elec3OutputPage.setText("H3 OFF");
  }
  else
  {
    outputController_.turnOn(2, outputPins_[2]);
    //    elec3OutputPage.Set_background_color_bco(2016); //green
    //    elec3OutputPage.setText("H3 ON");
  }

  displayController_.elec3 = outputController_.outputStates[2];
  displayController_.updatePage(OUTPUT_PAGE);
}
void backOutputPageCallback(void *ptr)
{
  displayController_.showPage(maintenancePageNextion);
  displayController_.updatePage(MAINT_PAGE);

  currPage_ = MAINT_PAGE;
}

//MAINT

void menuMaintenancePageCallback(void *ptr)
{
  displayController_.showPage(menuPageNextion);
  displayController_.updatePage(MENU_PAGE_SELECT);

  currPage_ = MENU_PAGE;
}
void saveMaintenancePageCallback(void *ptr)
{
  //SAVE CONFIG LOGIC
  saveMaintenancePage.setText("SAVING...");
  //CHAR BUFFER FOR INT STRING

  char bfr[2];
  String buff = "";
  int val = 0;

  //CALI FACTOR
  NexText(2, 33, "t2").getText(bfr, 2);
  if (bfr[0] == '-')
  {
    buff = bfr[1];
    val = buff.toInt();
    Serial.println(val * -1);
    confingController_.maintConfig_.TEMP_CALIBRATE = val * -1;
  }
  else
  {
    buff = bfr[0];
    val = buff.toInt();
    Serial.println(val);
    confingController_.maintConfig_.TEMP_CALIBRATE = val;
  }

  //OFFSET
  NexText(2, 34, "t9").getText(bfr, 2);
  if (bfr[0] == '-')
  {
    buff = bfr[1];
    val = buff.toInt();
    Serial.println(val * -1);
    confingController_.maintConfig_.BAND_OFFSET = val * -1;
  }
  else
  {
    buff = bfr[0];
    val = buff.toInt();
    Serial.println(val);
    confingController_.maintConfig_.BAND_OFFSET = val;
  }

  //calie factor 2
  NexText(2, 39, "t11").getText(bfr, 2);
  if (bfr[0] == '-')
  {
    buff = bfr[1];
    val = buff.toInt();
    Serial.println(val * -1);
    confingController_.maintConfig_.CALIBRATE2 = val * -1;
  }
  else
  {
    buff = bfr[0];
    val = buff.toInt();
    Serial.println(val);
    confingController_.maintConfig_.CALIBRATE2 = val;
  }

  uint32_t DELAY = 0;
  uint32_t BAND = 0;
  uint32_t H1STOP = 0;
  uint32_t H2STOP = 0;
  uint32_t H3STOP = 0;

  NexNumber(2, 2, "n0").getValue(&BAND);
  delay(200);
  confingController_.maintConfig_.BAND_GAP = BAND;

  NexNumber(2, 4, "n3").getValue(&DELAY);
  delay(200);
  confingController_.maintConfig_.DELAY_TIME = DELAY;

  NexNumber(2, 5, "n4").getValue(&H1STOP);
  delay(200);
  confingController_.maintConfig_.STOP_2 = H1STOP;

  NexNumber(2, 6, "n5").getValue(&H2STOP);
  delay(200);
  confingController_.maintConfig_.STOP_4 = H2STOP;

  NexNumber(2, 7, "n6").getValue(&H3STOP);
  delay(200);
  confingController_.maintConfig_.STOP_6 = H3STOP;

  confingController_.saveConfigEEPROM();
  confingController_.loadConfigEEPROM();

  saveMaintenancePage.setText("SAVE");
}
void outputMaintenancePageCallback(void *ptr)
{
  displayController_.showPage(outputPageNextion);
  displayController_.updatePage(OUTPUT_PAGE);

  currPage_ = OUTPUT_PAGE;
}

//PASSWORD

void checkPassword()
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

  switch (displayController_.menuModeSelect)
  {

  case 3:
    if (pass1 == "5437")
    {
      displayController_.showPage(programPageNextion);
      displayController_.updatePage(PROGRAM_PAGE);

      currPage_ = PROGRAM_PAGE;
    }
    break;
  case 4:
    if (pass1 == "6492")
    {
      displayController_.showPage(maintenancePageNextion);
      displayController_.updatePage(MAINT_PAGE);
      currPage_ = MAINT_PAGE;
    }
    break;

  default:
    break;
  }
}
void enterPasswordPageCallback(void *ptr)
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

  switch (displayController_.menuModeSelect)
  {

  case 3:
    if (pass1 == "5437")
    {
      displayController_.showPage(programPageNextion);
      displayController_.updatePage(PROGRAM_PAGE);

      //currPage_ = PROGRAM_PAGE;
    }
    break;
  case 4:
    if (pass1 == "6492")
    {
      displayController_.showPage(maintenancePageNextion);
      displayController_.updatePage(MAINT_PAGE);
      // currPage_ = MAINT_PAGE;
    }
    break;

  default:
    break;
  }
}
void menuPasswordPageCallback(void *ptr)
{
  displayController_.showPage(menuPageNextion);
  displayController_.updatePage(MENU_PAGE_SELECT);

  currPage_ = MENU_PAGE;
}

//PROGRAM

void menuProgramPageCallback(void *ptr)
{
  displayController_.showPage(menuPageNextion);
  displayController_.updatePage(MENU_PAGE_SELECT);

  currPage_ = MENU_PAGE;
}
void saveProgramPageCallback(void *ptr)
{
  //SAVE CONFIG LOGIC
  saveProgramPage.setText("SAVING...");

  uint32_t TEMP = 0;
  uint32_t TIME = 0;

  NexNumber(1, 4, "n55").getValue(&TEMP);
  delay(200);
  confingController_.pastConfig_.TEMP_SET = TEMP;
  NexNumber(1, 5, "n56").getValue(&TIME);
  delay(200);
  confingController_.pastConfig_.TIME_SET = TIME;

  confingController_.saveConfigEEPROM();
  confingController_.loadConfigEEPROM();
  saveProgramPage.setText("SAVE");
}

//PRODUCTION

void menuProductionPageCallback(void *ptr)
{
  displayController_.showPage(menuPageNextion);
  displayController_.updatePage(MENU_PAGE_SELECT);

  currPage_ = MENU_PAGE;
}
void statusProductionPageCallback(void *ptr)
{
  displayController_.showPage(statusPageNextion);
  displayController_.updatePage(STATUS_PAGE);

  currPage_ = STATUS_PAGE;
}
void startStopProductionPageCallback(void *ptr)
{
  //start stop production LOGIC
  // if (operationModeController_.currentModesOperating_.PRODUCTION_RUNNING == true)
  // {

  //   operationModeController_.currentModesOperating_.PRODUCTION_RUNNING = false;
  //   operationModeController_.currentModesOperating_.PRODUCTION_HEATING = false;
  //   operationModeController_.init();
  //   outputController_.standby();
  //   startStopProductionPage.setText("START");
  //   startStopProductionPage.Set_background_color_bco(63488);
  // }
  // else
  // {
  //   operationModeController_.currentModesOperating_.PRODUCTION_RUNNING = true;
  //   operationModeController_.currentModesOperating_.PRODUCTION_HEATING = true;
  //   timerController_.cookTimerActive = false;
  //   startStopProductionPage.setText("STOP");
  //   startStopProductionPage.Set_background_color_bco(2016);
  // }

  if (operationModeController_.currentModesOperating_.PRODUCTION_RUNNING == false)
  {
    startStopProductionPage.setText("STOP");
    startStopProductionPage.Set_background_color_bco(2016);
    operationModeController_.currentModesOperating_.PRODUCTION_RUNNING = true;
    operationModeController_.currentModesOperating_.PRODUCTION_PAUSED = false;
    operationModeController_.currentModesOperating_.PRODUCTION_HEATING = true;
    operationModeController_.currentModesOperating_.PRODUCTION_COOKING = false;
    timerPauseReference_ = confingController_.pastConfig_.TIME_SET;
    Serial.println("PROD TIME SET");
    Serial.println(timerPauseReference_);
    logEventSD("PRODUCTION_HEAT_START");
  }
  else if (operationModeController_.currentModesOperating_.PRODUCTION_RUNNING == true )
  {
    startStopProductionPage.setText("START");
    outputController_.standby();
    startStopProductionPage.Set_background_color_bco(63488);
    operationModeController_.init();
    timerPauseReference_ = confingController_.pastConfig_.TIME_SET;
  }
  // else if (operationModeController_.currentModesOperating_.PRODUCTION_RUNNING == false && operationModeController_.currentModesOperating_.PRODUCTION_PAUSED == true)
  // {
  //   startStopProductionPage.setText("PAUSE");
  //   startStopProductionPage.Set_background_color_bco(63488);
  //   operationModeController_.currentModesOperating_.PRODUCTION_RUNNING = true;
  //   operationModeController_.currentModesOperating_.PRODUCTION_PAUSED = false;
  //   operationModeController_.currentModesOperating_.PRODUCTION_HEATING = true;
  //   operationModeController_.currentModesOperating_.PRODUCTION_COOKING = false;
  //   logEventSD("PRODUCTION_HEAT_START");
  //   timerPauseReference_ = timerController_.cookTimeLeft;
  // }
}
void resetProductionPageCallback(void *ptr)
{
  operationModeController_.init();
  outputController_.standby();
  logEventSD("PRODUCTION_RESET");
}

//SD LOG

void logEventSD(String EVENT)
{
  File root;
  root = SD.open("/");

  printDirectory(root, 0);
  File dataFile = SD.open("DATALOG.txt", FILE_WRITE);
  dataFile.close();
  if (SD.exists("DATALOG.txt"))
  {
    Serial.println("File Exists");
  }
  String Time = "RTC not active";
  String date = "RTC not active";
  if (rtc.isrunning())
  {
    DateTime now = rtc.now();

    Time = String(now.hour());
    Time += ":" + String(now.minute()) + ":" + String(now.second());
    date = String(now.day());
    date += "/" + String(now.month()) + "/" + String(now.year());
    Serial.println(date);
    Serial.println(Time);
  }
  if (SD.exists("DATALOG.txt"))
  {
    Serial.println("File Exists");
  }
  
   dataFile = SD.open("DATALOG.txt", FILE_WRITE);
  
  if (dataFile)
  {
    if (EVENT == "RESET")
    {
      Serial.println("RESET LOGGED");
      dataFile.print("RESET");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.println(date);
    }
    else if (EVENT == "STARTUP")
    {
      Serial.println("STARTUP LOGGED");
      dataFile.print("STARTUP");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.println(date);
    }
    else if (EVENT == "PRODUCTION_HEAT_START")
    {
      Serial.println("PRODUCTION_HEAT_START LOGGED");
      dataFile.print("PRODUCTION_START");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.print(date);
      dataFile.print(",");
      dataFile.print(confingController_.maintConfig_.BAND_GAP);
      dataFile.print(",");

      dataFile.print(confingController_.maintConfig_.TEMP_CALIBRATE);
      dataFile.print(",");

      dataFile.print(confingController_.maintConfig_.BAND_OFFSET);
      dataFile.print(",");

      dataFile.print(confingController_.maintConfig_.DELAY_TIME);
      dataFile.print(",");

      dataFile.print(confingController_.maintConfig_.STOP_2);
      dataFile.print(",");

      dataFile.print(confingController_.maintConfig_.STOP_4);
      dataFile.print(",");

      dataFile.print(confingController_.maintConfig_.STOP_6);
      dataFile.print(",");

      dataFile.println(confingController_.maintConfig_.CALIBRATE2);
      
    }
    else if (EVENT == "PRODUCTION_HEAT_FINISH")
    {
      Serial.println("PRODUCTION_HEAT_FINISH LOGGED");
      dataFile.print("PRODUCTION_FINISH");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.println(date);
    }
    else if (EVENT == "PRODUCTION_RESET")
    {
      Serial.println("PRODUCTION_RESET LOGGED");
      dataFile.print("PRODUCTION_RESET");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.println(date);
    }
    else if (EVENT == "PRODUCTION_COOK_START")
    {
      Serial.println("PRODUCTION_COOK_START LOGGED");
      dataFile.print("PRODUCTION_COOK_START");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.println(date);
    }
    else if (EVENT == "PRODUCTION_CYCLE_FINISH")
    {
      Serial.println("PRODUCTION_CYCLE_FINISH LOGGED");
      dataFile.print("PRODUCTION_CYCLE_FINISH");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.println(date);
    }
    else if (EVENT == "PRODUCTION_UPDATE")
    {
      Serial.println("PRODUCTION_UPDATE LOGGED");
      dataFile.print("PRODUCTION_UPDATE");
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TEMP_SET);
      dataFile.print(",");
      dataFile.print(confingController_.pastConfig_.TIME_SET);
      dataFile.print(",");
      String temp = "PROD_TEMP: ";
      temp+=displayController_.prod_temp;
      dataFile.print(temp);
      dataFile.print(",");
      String temp2 = "BOX_TEMP: ";
      temp2+=displayController_.box_temp;
      dataFile.print( temp2);
      dataFile.print(",");
      dataFile.print(Time);
      dataFile.print(",");
      dataFile.println(date);
    }
    dataFile.close();
  }
}
void initSD()
{
  
  bool success = false;
  for (size_t i = 0; (i < 2) && !success; ++i)
  {
    success = SD.begin(53);
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
  File root;
  root = SD.open("/");

  printDirectory(root, 0);
  File dataFile = SD.open("DATALOG.txt", FILE_WRITE);
  dataFile.close();
  if (SD.exists("DATALOG.txt"))
  {
    Serial.println("File Exists");
  }
}

//WATERLOW
void resetWaterlowPageCallback(void *ptr)
{
  logEventSD("RESET");
  softwareReset(WDTO_60MS);
}
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
void extractDatalog(){
  File myFile = SD.open("DATALOG.txt");
   if (myFile)
   {
      // read from the file until there's nothing else in it:
      while (myFile.available())
      {
         Serial.write((char)myFile.read());
      }                               
      // close the file:             
      myFile.close();
   }
}
