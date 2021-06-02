#include "Nextion.h"
#include <Arduino.h>
//#include "OperationModeController.h"
//////////////////////////////////////////////////////////////////////////////////
//Page Objects
//STARTUP
//COMPLETE
NexPage completecyclePageNextion = NexPage(0, 0, "COMPLETECYCLE");
NexButton menuCompletePage = NexButton(8, 2, "b0");
// Menu Page
NexPage menuPageNextion = NexPage(0, 0, "MENU");
NexText standbyModeMenu = NexText(0, 1, "t0");
NexText heatingModeMenu = NexText(0, 2, "t1");
NexText programModeMenu = NexText(0, 3, "t2");
NexText maintenanceModeMenu = NexText(0, 4, "t3");
NexText resetMenu = NexText(0, 5, "t4");
//NexButton enterMenuPage = NexButton(0, 7, "b0");
NexButton statusMenuPage = NexButton(0, 7, "b1");
NexText currentModeMenuPage = NexText(0,8,"t6");
// PROD
NexPage productionPageNextion = NexPage(0, 0, "PRODUCTION");
NexButton menuProductionPage = NexButton(4, 11, "b0");
NexButton statusProductionPage = NexButton(4, 12, "b1");
NexButton startStopProductionPage = NexButton(4, 13, "b2");
NexButton resetProductionPage = NexButton(4,24,"b3");
//MAINT
NexPage maintenancePageNextion = NexPage(0, 0, "MAINTENANCE");
NexButton menuMaintenancePage = NexButton(2, 23, "b16");
NexButton saveMaintenancePage = NexButton(2, 24, "b17");
NexButton outputMaintenancePage = NexButton(2, 25, "b18");
//OUTPUT
NexPage outputPageNextion = NexPage(0, 0, "OUTPUT");
NexButton elec1OutputPage = NexButton(3, 2, "b0");
NexButton elec2OutputPage = NexButton(3, 3, "b1");
NexButton elec3OutputPage = NexButton(3, 4, "b2");
NexButton backOutputPage = NexButton(3, 5, "b3");
//STARTUP
NexPage startupPageNextion = NexPage(0, 0, "STARTUP");
//PROGRAM
NexPage programPageNextion = NexPage(0, 0, "PROGRAM");
NexButton menuProgramPage = NexButton(1, 10, "b4");

NexButton saveProgramPage = NexButton(1, 11, "b5");
//PASSWORD
NexPage passwordPageNextion = NexPage(0, 0, "PASSWORD");
NexButton enterPasswordPage = NexButton(6, 15, "b10");
NexButton menuPasswordPage = NexButton(6, 16, "b11");

//STATUS
NexPage statusPageNextion = NexPage(0, 0, "STATUS");
NexNumber tempSetStatusPage = NexNumber(7, 10, "n0");
NexNumber prodTempStatusPage = NexNumber(7, 11, "n1");
NexNumber boxTempStatusPage = NexNumber(7, 12, "n2");

NexButton menuStatusPage = NexButton(7, 13, "b0");

//waterlow
NexButton  resetWaterlowPage = NexButton(9,3,"b0");
enum Page
{
    MENU_PAGE,
    PRODUCTION_PAGE,
    OUTPUT_PAGE,
    STATUS_PAGE,
    PROGRAM_PAGE,
    PASSWORD_PAGE,
    MAINT_PAGE,
    MENU_PAGE_SELECT,
    WATERLOW
};

NexTouch *nex_listen_list[] = {
    &menuCompletePage,
    //STATUS
    &menuStatusPage,
    //OUTPUT
    &elec1OutputPage,
    &elec2OutputPage,
    &elec3OutputPage,
    &backOutputPage,
    //MAINT
    &menuMaintenancePage,
    &saveMaintenancePage,
    &outputMaintenancePage,
    //PROGRAM
    &menuProgramPage,
    &saveProgramPage,
    //PRODUCTION
    &menuProductionPage,
    &statusProductionPage,
    &startStopProductionPage,
    &resetProductionPage,
    //MENU
    &standbyModeMenu,
    &heatingModeMenu,
    &programModeMenu,
    &maintenanceModeMenu,
    &resetMenu,
    //&enterMenuPage,
    &statusMenuPage,
    //PASSWROD
    &enterPasswordPage,
    &menuPasswordPage,
    //waterlow
    &resetWaterlowPage

};

class DisplayController
{
public:
    int menuModeSelect = 1;
    int temp_set;
    int prod_temp;
    int box_temp;
    bool elec1;
    bool elec2;
    bool elec3;
    bool wl1;
    bool wl2;
    uint32_t timeset;
    bool prodRunning = false;
    uint32_t timeleft;
    int bandgap;
    int cali;
    bool prodPaused = false;
    int offset;
    int delay;
    int h1;
    int h2;
    int cali2;
    //OperationModeController display_OperationsController_;

    int h3;
    void init_();
    void showPage(NexPage nextPage_);
    void updatePage(Page currentPage_);
};

void DisplayController::init_()
{
    nexInit();
}

void DisplayController::showPage(NexPage nextPage_)
{
    nextPage_.show();
}

void DisplayController::updatePage(Page currentPage_)
{
    
    switch (currentPage_)
    {
    case MENU_PAGE_SELECT:
    if(this->prodRunning == true){
        currentModeMenuPage.setText("PRODUCTION RUNNING");
    }else{
        currentModeMenuPage.setText("STANDBY");
    }
        switch (this->menuModeSelect)
        {
        case 1:
            standbyModeMenu.Set_background_color_bco(2047);
            heatingModeMenu.Set_background_color_bco(65535);
            programModeMenu.Set_background_color_bco(65535);
            maintenanceModeMenu.Set_background_color_bco(65535);
            resetMenu.Set_background_color_bco(65535);
            break;
        case 2:
            standbyModeMenu.Set_background_color_bco(65535);
            heatingModeMenu.Set_background_color_bco(2047);
            programModeMenu.Set_background_color_bco(65535);
            maintenanceModeMenu.Set_background_color_bco(65535);
            resetMenu.Set_background_color_bco(65535);
            break;
        case 3:
            standbyModeMenu.Set_background_color_bco(65535);
            heatingModeMenu.Set_background_color_bco(65535);
            programModeMenu.Set_background_color_bco(2047);
            maintenanceModeMenu.Set_background_color_bco(65535);
            resetMenu.Set_background_color_bco(65535);
            break;
        case 4:
            standbyModeMenu.Set_background_color_bco(65535);
            heatingModeMenu.Set_background_color_bco(65535);
            programModeMenu.Set_background_color_bco(65535);
            maintenanceModeMenu.Set_background_color_bco(2047);
            resetMenu.Set_background_color_bco(65535);
            break;
        case 5:
            standbyModeMenu.Set_background_color_bco(65535);
            heatingModeMenu.Set_background_color_bco(65535);
            programModeMenu.Set_background_color_bco(65535);
            maintenanceModeMenu.Set_background_color_bco(65535);
            resetMenu.Set_background_color_bco(2047);
            break;

        default:
            break;
        }

        break;

    case OUTPUT_PAGE:
        if (this->elec1 == true)
        {
            NexButton(3, 2, "b0").Set_background_color_bco(2016); //green
            NexButton(3, 2, "b0").setText("H1 ON");
        }
        else
        {
            NexButton(3, 2, "b0").Set_background_color_bco(63488); //red
            NexButton(3, 2, "b0").setText("H1 OFF");
        }

        if (this->elec2 == true)
        {
            NexButton(3, 3, "b1").Set_background_color_bco(2016); //green
            NexButton(3, 3, "b1").setText("H2 ON");
        }
        else
        {
            NexButton(3, 3, "b1").Set_background_color_bco(63488); //red
            NexButton(3, 3, "b1").setText("H2 OFF");
        }

        if (this->elec3 == true)
        {
            NexButton(3, 4, "b2").Set_background_color_bco(2016); //green
            NexButton(3, 4, "b2").setText("H3 ON");
        }
        else
        {
            NexButton(3, 4, "b2").Set_background_color_bco(63488); //red
            NexButton(3, 4, "b2").setText("H3 OFF");
        }
        break;
    case STATUS_PAGE:
        if (this->elec1 == true)
        {
            NexText(7, 2, "t1").Set_background_color_bco(2016); //green
            NexText(7, 2, "t1").setText("H1 ON");
        }
        else
        {
            NexText(7, 2, "t1").Set_background_color_bco(63488); //red
            NexText(7, 2, "t1").setText("H1 OFF");
        }

        if (this->elec2 == true)
        {
            NexText(7, 3, "t2").Set_background_color_bco(2016); //green
            NexText(7, 3, "t2").setText("H2 ON");
        }
        else
        {
            NexText(7, 3, "t2").Set_background_color_bco(63488); //red
            NexText(7, 3, "t2").setText("H2 OFF");
        }

        if (this->elec3 == true)
        {
            NexText(7, 4, "t3").Set_background_color_bco(2016); //green
            NexText(7, 4, "t3").setText("H3 ON");
        }
        else
        {
            NexText(7, 4, "t3").Set_background_color_bco(63488); //red
            NexText(7, 4, "t3").setText("H3 OFF");
        }

        if (this->wl1 == true)
        {
            NexText(7, 14, "t9").setText("OK");
            NexText(7, 14, "t9").Set_background_color_bco(2016); //green
        }
        else
        {
            NexText(7, 14, "t9").setText("LOW");
            NexText(7, 14, "t9").Set_background_color_bco(63488); //green
        }

        if (this->wl2 == true)
        {
            NexText(7, 15, "t10").setText("OK");
            NexText(7, 15, "t10").Set_background_color_bco(2016); //green
        }
        else
        {
            NexText(7, 15, "t10").setText("LOW");
            NexText(7, 15, "t10").Set_background_color_bco(63488); //green
        }

        NexNumber(7, 10, "n0").setValue(this->temp_set);

        if (this->box_temp < 0)
        {
            NexNumber(7, 12, "n2").setValue(0);
        }
        else
        {
            NexNumber(7, 12, "n2").setValue(this->box_temp);
        }

        if (this->prod_temp < 0)
        {
            NexNumber(7, 11, "n1").setValue(0);
        }
        else
        {
            NexNumber(7, 11, "n1").setValue(this->prod_temp);
        }
        break;

    case PRODUCTION_PAGE:
        if (this->prod_temp < 0)
        {
            NexNumber(4, 5, "n1").setValue(0);
        }
        else
        {
            NexNumber(4, 5, "n1").setValue(this->prod_temp);
        }

        if (this->prodRunning == false && this->prodPaused == false)
        {
            NexNumber(4, 8, "n3").setValue(0);
            NexNumber(4, 15, "n4").setValue(0);
            NexNumber(4, 23, "n6").setValue(0);
        }
        else
        {
            NexNumber(4, 8, "n3").setValue((int)this->timeleft / 3600);
            NexNumber(4, 15, "n4").setValue((int)(this->timeleft / 60) % 60);
            NexNumber(4, 23, "n6").setValue((int)this->timeleft % 60);
        }
        Serial.println("PROD TIME SET _ DISPLAY UPDATE");
        Serial.println(this->timeset / 60);
        NexNumber(4, 2, "n0").setValue(this->temp_set);

        //NexNumber(4, 7, "n2").setValue(this->timeset);
        NexNumber(4, 19, "n2").setValue(this->timeset / 3600);
        NexNumber(4, 21, "n5").setValue((this->timeset / 60) % 60);

        // NexNumber(4, 8, "n3").Set_background_color_bco(65535);
        // NexNumber(4, 15, "n4").Set_background_color_bco(65535);

        // if (this->prodRunning)
        // {
        //     startStopProductionPage.setText("STOP");
        //     startStopProductionPage.Set_background_color_bco(2016);
        // }
        // else
        // {
        //     startStopProductionPage.setText("START");
        //     startStopProductionPage.Set_background_color_bco(63488);
        // }

        if (this->wl1 == true)
        {
            NexText(7, 18, "t9").setText("OK");
            NexText(7, 18, "t9").Set_background_color_bco(2016); //green
        }
        else
        {
            NexText(7, 18, "t9").setText("LOW");
            NexText(7, 18, "t9").Set_background_color_bco(63488); //green
        }

        if (this->wl2 == true)
        {
            NexText(7, 19, "t10").setText("OK");
            NexText(7, 19, "t10").Set_background_color_bco(2016); //green
        }
        else
        {
            NexText(7, 19, "t10").setText("LOW");
            NexText(7, 19, "t10").Set_background_color_bco(63488); //green
        }

        break;

    case PROGRAM_PAGE:
        char buf[3];
        NexNumber(1, 4, "n55").setValue(this->temp_set);
        NexNumber(1, 5, "n56").setValue((int)this->timeset / 60);
        Serial.println("PROG TIME SET");
        Serial.println(this->timeset / 60);
        // Serial.println(this->TEMP_SET);
        break;
    case MAINT_PAGE:
        NexNumber(2, 2, "n0").setValue(this->bandgap);
        if (this->cali < 0)
        {
            String(this->cali).toCharArray(buf, 3);
            NexText(2, 33, "t2").setText(buf);
        }
        else
        {
            String(this->cali).toCharArray(buf, 3);
            //NexNumber(2, 8, "n7").setValue(this->cali); //cali
            NexText(2, 33, "t2").setText(buf);
        }
        if (this->offset < 0)
        {
            String(this->offset).toCharArray(buf, 3);
            NexText(2, 34, "t9").setText(buf);
        }
        else
        {
            String(this->offset).toCharArray(buf, 3);
            //NexNumber(2, 3, "n2").setValue(this->offset); //offset
            NexText(2, 34, "t9").setText(buf);
        }

        if (this->cali2 < 0)
        {
            String(this->cali2).toCharArray(buf, 3);
            NexText(2, 39, "t11").setText(buf);
        }
        else
        {
            String(this->cali2).toCharArray(buf, 3);
            //NexNumber(2, 37, "n1").setValue(this->cali2); //offset
            NexText(2, 39, "t11").setText(buf);
        }

        NexNumber(2, 4, "n3").setValue(this->delay);

        NexNumber(2, 5, "n4").setValue(this->h1);
        NexNumber(2, 6, "n5").setValue(this->h2);
        NexNumber(2, 7, "n6").setValue(this->h3);
        break;
    default:
        break;
    }
}
