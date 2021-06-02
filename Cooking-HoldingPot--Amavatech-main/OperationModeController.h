enum operationModes
{
    MODE_STANDBY,
    MODE_WATERFILL_BAGLOADING,
    MODE_PRODUCTION_COOKPOT,
    MODE_PRODUCTION_HOLDINGPOT,
    MODE_CIP,
    MODE_PROGRAMPOTS,
    MODE_PROGRAMCIP,
    MODE_MAINTENANCE,
    MODE_RESET
};

struct currentModesOperating
{
    bool STANDBY_RUNNING;
    bool WATERFILL_RUNNING; // start fill init


    bool PRODUCTION_COOK_RUNNING; //
    bool PRODUCTION_COOK_HEAT_RUNNING;
    bool PRODUCTION_COOK_CYCLE_RUNNING
    bool PRODUCTION_HOLD_RUNNING;
    bool PRODUCTION_HOLD_HEAT_RUNNING;
    bool PRODUCTION_HOLD_PUMP_RUNNING;

    bool CIP_ACTIVE;
    bool CIP_COOK_RUNNING;
    bool CIP_HOLD_RUNNING;
    bool TRANSFER_RUNNING;


    bool CIP_FILL_COOK_RUNNING;
    bool CIP_HEAT_COOK_RUNNING;
    bool CIP_CYCLE_COOK_RUNNING;
    bool CIP_DRAIN_COOK_RUNNING;

    bool CIP_FILL_HOLD_RUNNING;
    bool CIP_HEAT_HOLD_RUNNING;
    bool CIP_CYCLE_HOLD_RUNNING;
    bool CIP_DRAIN_HOLD_RUNNING;

    bool COOK_POT_DRAINING;
    bool HOLD_POT_DRAINING;
};


class OperationModeController
{

public:
  currentModesOperating currentModesOperating_;

  void init();
};

void OperationModeController::init(){
    this->currentModesOperating_.STANDBY_RUNNING=true;
    this->currentModesOperating_.WATERFILL_RUNNING=false;
    this->currentModesOperating_.PRODUCTION_COOK_RUNNING=false;
    this->currentModesOperating_.PRODUCTION_HOLD_RUNNING=false;
    this->currentModesOperating_.CIP_COOK_RUNNING=false;
    this->currentModesOperating_.CIP_HOLD_RUNNING=false;
}
