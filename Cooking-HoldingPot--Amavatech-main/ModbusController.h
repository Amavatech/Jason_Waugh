#include <ModbusMaster.h>
#include <SoftwareSerial.h>
SoftwareSerial SerialMod2(A11, A10);
ModbusMaster node;

class ModbusController
{
private:
    /* data */
public:
    ModbusMaster node;
    bool VSD_States[7];
    void stopVSD(int address);
    void runVSD(int address);
    void setDirection();
    void setSpeedRefs(ConfigController configController_);
    void Standby();
};
void ModbusController::runVSD(int address)
{

    SerialMod2.begin(19200);
    while (!SerialMod2)
    {
    }
    node.begin(address, SerialMod2);
    this->VSD_States[address] = true;
    node.writeSingleRegister(633, 0x1);
}

void ModbusController::stopVSD(int address)
{

    SerialMod2.begin(19200);
    while (!SerialMod2)
    {
    }
    node.begin(address, SerialMod2);
    this->VSD_States[address] = false;
    node.writeSingleRegister(633, 0x0);
}

void ModbusController::Standby()
{
    SerialMod2.begin(19200);

    for (int i = 0; i < 7; i++)
    {
        node.begin(i, SerialMod2);
        this->VSD_States[i] = false;
        node.writeSingleRegister(633, 0x0);
    }

    
}

void ModbusController::setDirection(){

  SerialMod2.begin(19200);
  while (!SerialMod2)
  {
  }
  node.begin(2, SerialMod2);
  node.writeSingleRegister(632, 0x1);

  node.begin(6, SerialMod2);
  node.writeSingleRegister(632, 0x1);

  node.begin(5, SerialMod2);
  node.writeSingleRegister(632, 0x1);
}

void ModbusController::setSpeedRefs(ConfigController configController_)
{
  SerialMod2.begin(19200);
  while (!SerialMod2)
  {
  }
//  node.begin(COOK_1_MIXER_ADDRESS, SerialMod2);
//  node.writeSingleRegister(17, configController_.mixerConfig_.MIXERC1_SET*100);
//
//  node.begin(COOK_2_MIXER_ADDRESS, SerialMod2);
//  node.writeSingleRegister(17, configController_.mixerConfig_.MIXERC2_SET*100);
//
//  node.begin(HOLD_MIXER_ADDRESS, SerialMod2);
//  node.writeSingleRegister(17, configController_.mixerConfig_.MIXERH1_SET*100);
//
//  node.begin(HIGH_SHEER_MIXER_ADDRESS, SerialMod2);
//  node.writeSingleRegister(17, configController_.mixerConfig_.MIXERHS_SET*100);
//
//  node.begin(CIP_PUMP_ADDRESS, SerialMod2);
//  node.writeSingleRegister(17, configController_.cipConfig_.CIPPUMP_SET*100);
//
//  node.begin(LOBE1_PUMP_ADDRESS, SerialMod2);
//  node.writeSingleRegister(17, configController_.pumpConfig_.LOBE1_SET*100);
//
//  node.begin(LOBE2_PUMP_ADDRESS, SerialMod2);
//  node.writeSingleRegister(17, configController_.pumpConfig_.LOBE2_SET*100);
}
