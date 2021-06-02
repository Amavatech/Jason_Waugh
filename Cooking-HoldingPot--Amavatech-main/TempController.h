

class TempController
{

public:
    void controlCookTemp();
    void CipHeat(OperationModeController OperationModeController_, ConfigController ConfigController_, GpioController GpioController_, int cipType, float tempCook); // at temp --> this must change operation mode to cip cycle running
};

void TempController::controlCookTemp()
{
}

void TempController::CipHeat(OperationModeController OperationModeController_, ConfigController ConfigController_, GpioController GpioController_, int cipType, float tempCook)
{
    float setTemp;
    if (cipType == 1)
    {
        setTemp = ConfigController_.cipConfig_.TEMPRINSE_SET;
    }
    else
    {
        setTemp = ConfigController_.cipConfig_.TEMPSANI_SET;
    }

    if (tempCook >= setTemp)
    {
        if (OperationModeController_.currentModesOperating_.CIP_COOK_RUNNING)
        {
            OperationModeController_.currentModesOperating_.CIP_HEAT_COOK_RUNNING = false;
            OperationModeController_.currentModesOperating_.CIP_CYCLE_COOK_RUNNING = true;
            GpioController_.closeValve(STEAM_VALVE_OUTPUT_1, GpioController_.valveStates_.steamValve1);
        }
        else if (OperationModeController_.currentModesOperating_.CIP_HOLD_RUNNING)
        {
            OperationModeController_.currentModesOperating_.CIP_HEAT_HOLD_RUNNING = false;
            OperationModeController_.currentModesOperating_.CIP_CYCLE_HOLD_RUNNING = true;
            GpioController_.closeValve(STEAM_VALVE_OUTPUT_2, GpioController_.valveStates_.steamValve2);
        }
    }
    else
    {
        if (OperationModeController_.currentModesOperating_.CIP_COOK_RUNNING)
        {
            if (GpioController_.valveStates_.steamValve1 == false)
            {
                GpioController_.openValve(STEAM_VALVE_OUTPUT_1, GpioController_.valveStates_.steamValve1);
            }
        }
        else if (OperationModeController_.currentModesOperating_.CIP_HOLD_RUNNING)
        {
            if (GpioController_.valveStates_.steamValve1 == false)
            {
                GpioController_.openValve(STEAM_VALVE_OUTPUT_2, GpioController_.valveStates_.steamValve2);
            }
        }
    }
}
