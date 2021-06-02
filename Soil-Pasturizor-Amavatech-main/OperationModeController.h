enum operationModes
{
    MODE_STANDBY,
    MODE_PRODUCTION,
    MODE_TESTING
};

struct currentModesOperating
{
    bool STANDBY_RUNNING;
    bool PRODUCTION_RUNNING;
    bool OUTPUT_TESTING;
    bool PRODUCTION_HEATING;
    bool PRODUCTION_COOKING;
    bool PRODUCTION_COOK_COMPLETED;
    bool PRODUCTION_PAUSED;
};

class OperationModeController
{
private:
  
public:
   currentModesOperating currentModesOperating_;

  void init();
};

void OperationModeController::init(){
    this->currentModesOperating_.STANDBY_RUNNING=true;
    this->currentModesOperating_.PRODUCTION_RUNNING=false;
    this->currentModesOperating_.OUTPUT_TESTING=false;
    this->currentModesOperating_.PRODUCTION_HEATING=false;
    this->currentModesOperating_.PRODUCTION_COOKING=false;
    this->currentModesOperating_.PRODUCTION_COOK_COMPLETED=false;
    this->currentModesOperating_.PRODUCTION_PAUSED=false;
    
    

}