#include "Nextion.h"

// PAGES ---------------------------------------------------------------------------
NexPage waterFillingPage = NexPage(0, 0, "WATERFILLING");
NexPage waterFillCompletePage = NexPage(0, 0, "WATERFILLDONE");
NexPage cookProdPage = NexPage(0, 0, "PRODUCTIONCOOK");
NexPage cookCompletePage = NexPage(0, 0, "BATCHCOMPLETE");
NexPage runCIPPage = NexPage(0, 0, "RUNCIP");
NexPage programPotsPage = NexPage(0, 0, "PROGRAMPOTS");
NexPage programCIPPage = NexPage(0, 0, "PROGRAMCIP");
NexPage CIPTimesPage = NexPage(0, 0, "CIPTIMES");
NexPage productionHoldPage = NexPage(0, 0, "PRODUCTIONHOLD");
NexPage maintPage = NexPage(0, 0, "MAINTENENCE");
NexPage outputPage = NexPage(0, 0, "OUTPUT");
NexPage startupPage = NexPage(0, 0, "STARTUP");
NexPage passwordPage = NexPage(0,0,"PASSWORD");

NexNumber passwordN1 = NexNumber(14,2,"n0");
NexNumber passwordN2 = NexNumber(14,3,"n1");
NexNumber passwordN3 = NexNumber(14,4,"n2");
NexNumber passwordN4 = NexNumber(14,5,"n3");

NexButton passwordEnter = NexButton(14,7,"b1");
NexButton passwordBack = NexButton(14,6,"b0");
//TESSSTTTT
//NexPage test = NexPage (0,0,"Production");

// WATERFILLING PAGE ----------------------------------------
NexNumber volumeFilled_ = NexNumber(10,4,"n0");
NexNumber volume2Fill_ = NexNumber(10,5,"n1");
NexButton waterFillingFillValve = NexButton(10, 3, "b1");
NexButton waterFillingCancel = NexButton(10, 2, "b0");

//WATER FILL DONE PAGE

NexButton fillCompleteMixerC1 = NexButton(11, 2, "b0");
NexButton fillCompleteMixerC2 = NexButton(11, 3, "b1");
NexButton fillCompleteMixerHS = NexButton(11, 4, "b2");
NexButton fillCompleteMenu = NexButton(11, 5, "b3");

//PRODUCTION COOK PAGE

NexText menuCookProdPage = NexText(2, 9, "t19");
NexText statusCookProdPage = NexText(2, 23, "t12");
NexNumber cookProdTempSet = NexNumber(2, 13, "n0");
NexNumber cookProdTemp = NexNumber(2, 14, "n1");
NexNumber cookProdTimeSetH = NexNumber(2, 15, "n2");
NexNumber cookProdTimeSetM = NexNumber(2, 17, "n4");
NexNumber cookProdTimeLeftH = NexNumber(2, 16, "n3");
NexNumber cookProdTimeLeftM = NexNumber(2, 18, "n5");
NexButton mixerC1CookProd = NexButton(2, 10, "b0");
NexButton mixerC2CookProd = NexButton(2, 11, "b1");
NexButton mixerHSCookProd = NexButton(2, 12, "b2");

NexButton startStopProdCook = NexButton(2, 24, "b3");

NexNumber secondsProdSet = NexNumber(2, 25, "n6");
NexNumber secondsProdLeft = NexNumber(2, 26, "n7");
//PRODUCTION HOLDING PAGE
NexNumber holdProdCookTemp = NexNumber(7, 9, "n2");
NexNumber holdProdTempSet1 = NexNumber(7, 12, "n3");
NexNumber holdProdTempSet2 = NexNumber(7, 6, "n0");
NexNumber holdProdHoldTemp = NexNumber(7, 7, "n1");
NexButton holdProdMixerH1 = NexButton(7, 5, "b0");
NexButton holdProdLOBE1 = NexButton(7, 14, "b1");
NexText holdEmptyMessage = NexText(7, 15, "t7");
NexText menuHoldingProduction = NexText(7, 8, "t19");
NexText statusHoldingProduction = NexText(7, 16, "t8");

NexButton startStopProdHold = NexButton(7, 17, "b2");

//PROGRAMPOTS PAGE

NexNumber progrmaPotsWaterC = NexNumber(3, 16, "n0");
NexNumber progrmaPotsWaterH = NexNumber(3, 18, "n1");
NexNumber progrmaPotsTempC = NexNumber(3, 21, "n2");
NexNumber progrmaPotsTimeSet = NexNumber(3, 24, "n3");
NexNumber progrmaPotsTempH = NexNumber(3, 27, "n4");
NexNumber progrmaPotsMixerC1 = NexNumber(3, 30, "n5");
NexNumber progrmaPotsMixerC2 = NexNumber(3, 33, "n6");
NexNumber progrmaPotsMixerHS = NexNumber(3, 36, "n7");
NexNumber progrmaPotsMixerH1 = NexNumber(3, 39, "n8");
NexNumber progrmaPotsLobe1 = NexNumber(3, 42, "n9");
NexNumber progrmaPotsLobe2 = NexNumber(3, 45, "n10");
NexText saveProgramPots = NexText(3, 47, "t13");
NexText menuProgramPots = NexText(3, 13, "t12");

//PROGRAM CIP

NexNumber programcipRinseSet = NexNumber(4, 11, "n0");
NexNumber programcipSaniSet = NexNumber(4, 13, "n1");
NexNumber programcipTempRinseSet = NexNumber(4, 16, "n2");
NexNumber programcipTempSaniSet = NexNumber(4, 19, "n3");
NexNumber programcipDVC = NexNumber(4, 22, "n4");
NexNumber programcipDVH = NexNumber(4, 25, "n5");
NexNumber programcipPumpSet = NexNumber(4, 28, "n6");
NexText programcipRinseTimePage = NexText(4, 30, "t8");
NexText programcipMenu = NexText(4, 31, "t10");
NexText programcipSave = NexText(4, 32, "t9");

//PROGRAM CIP TIMES PAGE

NexNumber cipTimeSetR1 = NexNumber(5, 9, "n0");
NexNumber cipTimeSetR2 = NexNumber(5, 11, "n1");
NexNumber cipTimeSetR3 = NexNumber(5, 14, "n2");
NexNumber cipTimeSetR4 = NexNumber(5, 17, "n3");
NexNumber cipTimeSetR5 = NexNumber(5, 20, "n4");
NexNumber cipTimeSetR6 = NexNumber(5, 23, "n11");
NexNumber cipTimeSetR7 = NexNumber(5, 56, "n12");
NexNumber cipTimeSetR8 = NexNumber(5, 64, "n14");
NexNumber cipTimeSetR9 = NexNumber(5, 72, "n16");

NexNumber cipTimeSetS1 = NexNumber(5, 33, "n5");
NexNumber cipTimeSetS2 = NexNumber(5, 35, "n6");
NexNumber cipTimeSetS3 = NexNumber(5, 38, "n7");
NexNumber cipTimeSetS4 = NexNumber(5, 41, "n8");
NexNumber cipTimeSetS5 = NexNumber(5, 44, "n9");
NexNumber cipTimeSetS6 = NexNumber(5, 47, "n10");
NexNumber cipTimeSetS7 = NexNumber(5, 60, "n13");
NexNumber cipTimeSetS8 = NexNumber(5, 68, "n15");
NexNumber cipTimeSetS9 = NexNumber(5, 76, "n17");

NexButton cipTimeSetBack = NexButton(5, 50, "b24");
NexButton cipTimeSetSave = NexButton(5, 51, "b25");

//MODE CIP PAGE
NexText pumpPrimer = NexText(6,15,"t3");
NexText cipRinse1 = NexText(6, 3, "t2");
NexText cipRinse2 = NexText(6, 4, "t4");
NexText cipRinse3 = NexText(6, 5, "t6");
NexText cipRinse4 = NexText(6, 6, "t8");
NexText cipRinse5 = NexText(6, 7, "t10");
NexText cipRinse6 = NexText(6, 8, "t12");

NexText cipRinse7 = NexText(6, 12, "t14");
NexText cipRinse8 = NexText(6, 13, "t15");
NexText cipRinse9 = NexText(6, 14, "t16");

NexButton startCIP = NexButton(6, 11, "b0");
NexButton stopCIP = NexButton(6, 10, "b25");
NexButton CIPMenu = NexButton(6, 9, "b24");

NexText rinseSelectCIP = NexText(6, 1, "t0");
NexText saniSelectCIP = NexText(6, 2, "t1");

// MAINTENANCE PAGE
NexText maintMenu = NexText(8, 12, "t12");
NexText maintOutput = NexText(8, 43, "t11");
NexNumber maintBandGap1 = NexNumber(8, 15, "n0");
NexNumber maintWidth1 = NexNumber(8, 17, "n1");
NexNumber maintOffset1 = NexNumber(8, 20, "n2");
NexNumber maintDelay1 = NexNumber(8, 23, "n3");
NexNumber maintCali1 = NexNumber(8, 26, "n4");
NexNumber maintBandGap2 = NexNumber(8, 29, "n5");
NexNumber maintWidth2 = NexNumber(8, 32, "n6");
NexNumber maintOffset2 = NexNumber(8, 35, "n7");
NexNumber maintDelay2 = NexNumber(8, 38, "n8");
NexNumber maintCali2 = NexNumber(8, 41, "n9");

//OUTPUT PAGE

NexButton outputV1 = NexButton(9, 30, "b0");
NexButton outputV2 = NexButton(9, 31, "b1");
NexButton outputV3 = NexButton(9, 32, "b2");
NexButton outputV4 = NexButton(9, 33, "b3");
NexButton outputV5 = NexButton(9, 34, "b4");
NexButton outputV6 = NexButton(9, 35, "b5");
NexButton outputV7 = NexButton(9, 36, "b6");
NexButton outputV8 = NexButton(9, 37, "b7");
NexButton outputV9 = NexButton(9, 38, "b8");
NexButton outputV10 = NexButton(9, 39, "b9");
NexButton outputV11 = NexButton(9, 40, "b10");
NexButton outputV12 = NexButton(9, 41, "b11");
NexButton outputV13 = NexButton(9, 42, "b12");
NexButton outputV14 = NexButton(9, 43, "b13");
NexButton outputV15 = NexButton(9, 44, "b14");
NexButton outputV16 = NexButton(9, 45, "b15");
NexButton outputV17 = NexButton(9, 46, "b16");
NexButton outputV18 = NexButton(9, 47, "b17");
NexButton outputV19 = NexButton(9, 48, "b18");
NexButton outputSteam1 = NexButton(9, 49, "b19");
NexButton outputSteam2 = NexButton(9, 50, "b20");

NexButton mixerC1 = NexButton(9, 51, "b21");
NexButton mixerC2 = NexButton(9, 52, "b22");
NexButton mixerH1 = NexButton(9, 53, "b23");
NexButton mixerHS = NexButton(9, 54, "b24");

NexButton pumpCIP = NexButton(9, 55, "b25");
NexButton pumpL1 = NexButton(9, 56, "b26");
NexButton pumpL2 = NexButton(9, 57, "b27");

NexButton outputBack = NexButton(9, 58, "b28");
NexButton drainCook = NexButton(9, 59, "b29");
NexButton drainHold = NexButton(9, 60, "b30");
// BATCH COMPLETE PAGE
NexText batchCompStartTransfer = NexText(12, 2, "t1");
NexButton bacthDoneMenu = NexButton(12, 3, "b0");

//Status Page Objects
NexPage statusPage = NexPage(0, 0, "STATUS");
NexText steamCookState = NexText(1, 35, "t34");
NexText steamHoldState = NexText(1, 34, "t33");
NexText mixer1State = NexText(1, 33, "t32");
NexText mixer2State = NexText(1, 32, "t31");
NexText mixerHSState = NexText(1, 31, "t30");
NexText mixerH1State = NexText(1, 30, "t29");
NexText pumpT1State = NexText(1, 29, "t28");
NexText pumpT2State = NexText(1, 28, "t27");
NexText pumpCIPState = NexText(1, 27, "t26");
NexText mixerHSSpeed = NexText(1, 36, "t35");
NexText mixerH1Speed = NexText(1, 37, "t36");
NexText mixer2Speed = NexText(1, 38, "t37");
NexText mixer1Speed = NexText(1, 39, "t38");
NexText pumpT1Speed = NexText(1, 40, "t39");
NexText pumpT2Speed = NexText(1, 41, "t40");
NexText pumpCIPSpeed = NexText(1, 42, "t41");
NexText tempSet1 = NexText(1, 44, "t43");
NexText prodTemp1 = NexText(1, 45, "t44");
NexText tempSet2 = NexText(1, 46, "t45");
NexText prodTemp2 = NexText(1, 47, "t46");
NexText cookLevelLow = NexText(1, 48, "t47");
NexText holdingLevelHigh = NexText(1, 49, "t48");
NexText holdingLevelLow = NexText(1, 50, "t49");
NexText statusCurrentMode = NexText(1, 1, "t0");
NexText menuStatusPage = NexText(1, 51, "t50");

//menu Page objects and variables
int menuModeSelected = 1; // 0 -> Top option, 8-> Bottom Option
NexPage menuPage = NexPage(0, 0, "MENU");
NexText menuCurrentMode = NexText(0, 1, "t0");

NexText menuMode1 = NexText(0, 2, "t1");
NexText menuMode2 = NexText(0, 3, "t2");
NexText menuMode3 = NexText(0, 4, "t3");
NexText menuMode4 = NexText(0, 5, "t4");
NexText menuMode5 = NexText(0, 6, "t5");
NexText menuMode6 = NexText(0, 7, "t6");
NexText menuMode7 = NexText(0, 8, "t7");
NexText menuMode8 = NexText(0, 9, "t8");

NexText menuStatusButton = NexText(0, 11, "t18");
NexText menuEnterButton = NexText(0, 12, "t19");
NexText enterStandbyMenu = NexText(0, 13, "t9");
enum Page
{
    MENU,
    STATUS,
    COOK_PROD,
    COOK_HOLD,
    DEFUALTPAGE,
    OUTPUT_PAGE,
    CIP_PAGE,
    COOK_COMPLETE,
    FILLING_PAGE
};

NexTouch *nex_listen_list[] = {
    &pumpPrimer,
    &menuMode1,
    &menuMode2,
    &menuMode3,
    &menuMode4,
    &menuMode5,
    &menuMode6,
    &menuMode7,
    &menuMode8,

    &menuStatusButton,
    &menuEnterButton,
    &enterStandbyMenu,
    &waterFillingFillValve,
    &waterFillingCancel,

    //WATER FILL DONE PAGE

    &fillCompleteMixerC1,
    &fillCompleteMixerC2,
    &fillCompleteMixerHS,
    &fillCompleteMenu,

    //PRODUCTION COOK PAGE

    &menuCookProdPage,
    &statusCookProdPage,

    &mixerC1CookProd,
    &mixerC2CookProd,
    &mixerHSCookProd,
    &startStopProdCook,

    //PRODUCTION HOLDING PAGE

    &holdProdMixerH1,
    &holdProdLOBE1,
    &startStopProdHold,

    &menuHoldingProduction,
    &statusHoldingProduction,
    //PROGRAMPOTS PAGE

    &saveProgramPots,
    &menuProgramPots,

    //PROGRAM CIP

    &programcipRinseTimePage,
    &programcipMenu,
    &programcipSave,

    //PROGRAM CIP TIMES PAGE

    &cipTimeSetBack,
    &cipTimeSetSave,

    //MODE CIP PAGE
    &cipRinse1,
    &cipRinse2,
    &cipRinse3,
    &cipRinse4,
    &cipRinse5,
    &cipRinse6,
    &startCIP,
    &stopCIP,
    &CIPMenu,
    &cipRinse7,
    &cipRinse8,
    &cipRinse9,
    &rinseSelectCIP,
    &saniSelectCIP,

    // MAINTENANCE PAGE
    &maintMenu,
    &maintOutput,

    //OUTPUT PAGE

    &outputV1,
    &outputV2,
    &outputV3,
    &outputV4,
    &outputV5,
    &outputV6,
    &outputV7,
    &outputV8,
    &outputV9,
    &outputV10,
    &outputV11,
    &outputV12,
    &outputV13,
    &outputV14,
    &outputV15,
    &outputV16,
    &outputV17,
    &outputV18,
    &outputV19,
    &outputSteam1,
    &outputSteam2,
    &drainCook,
    &drainHold,
    &mixerC1,
    &mixerC2,
    &mixerH1,
    &mixerHS,

    &pumpCIP,
    &pumpL1,
    &pumpL2,

    &outputBack,

    // BATCH COMPLETE PAGE
    &batchCompStartTransfer,
    &bacthDoneMenu,

    //Status Page Objects

    &menuStatusPage,

    NULL};
class DisplayController
{
private:
    
public:
    void initDisplay();
    void showPage(NexPage page_);

    
    void updateCIPTimes();
    
    void updateCIP();
    void updateProgramPots();
    void updateMenuPage();
    void updateMaintPage();
    void updateProgramCIPPage();
    void updateHoldingProdPage();
    void updateCookPotPage();
    void updateOutputPage();
    void updateStatusPage();

};

void DisplayController::initDisplay(){
    nexInit();
}

void DisplayController::showPage(NexPage page_){
    page_.show();
}