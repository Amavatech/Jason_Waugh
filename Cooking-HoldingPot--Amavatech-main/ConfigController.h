#include "Arduino.h"
#include "EEPROM.h"
struct cookPotConfig
{
    uint32_t WATERC_SET; // Litres - increments of 5 - 10 - 1000 -- fine
    uint32_t TEMPC_SET;  // 10 - 93 --> 1 degree --- this is fine too
    uint32_t TIME_SET;   // 1 - 180 mins, 1 minute steps -- this is fine
};
struct holdingPotConfig
{
    uint32_t WATERH_SET;
    uint32_t TEMPH_SET;
};
struct mixerConfig
{
    uint32_t MIXERC1_SET; // Hertz - 10 - 50 --- fine
    uint32_t MIXERC2_SET;
    uint32_t MIXERHS_SET;
    uint32_t MIXERH1_SET;
};
struct pumpConfig
{
    uint32_t LOBE1_SET;
    uint32_t LOBE2_SET;
};
struct generalConfig
{
    uint32_t BAND_GAP1;       // auto geerated ,,, shouldnt be allowed to toggle
    uint32_t WIDTH1;          // 1 - 12, 0.1 increments degrees
    uint32_t BAND_OFFSET1;    //  -2 to 2 in 1 increm -- needs to be handled !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    uint32_t DELAY_TIME1;     // 1 - 60, 1 sec
    uint32_t TEMP_CALIBRATE1; // -10 to 10 in 0.1 increm  -- needs to be handled !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    uint32_t BAND_GAP2;
    uint32_t WIDTH2;
    uint32_t BAND_OFFSET2;
    uint32_t DELAY_TIME2;
    uint32_t TEMP_CALIBRATE2;
};
struct cipConfig
{
    uint32_t WATERRINSE_SET;
    uint32_t WATERSANI_SET;
    uint32_t TEMPRINSE_SET;
    uint32_t TEMPSANI_SET;
    uint32_t DVCPOT_SET;
    uint32_t DVHPOT_SET;
    uint32_t CIPPUMP_SET;
    uint32_t CIP1RTIME_SET;
    uint32_t CIP2RTIME_SET;
    uint32_t CIP3RTIME_SET;
    uint32_t CIP4RTIME_SET;
    uint32_t CIP5RTIME_SET;
    uint32_t CIP6RTIME_SET;

    uint32_t CIP7RTIME_SET;
    uint32_t CIP8RTIME_SET;
    uint32_t CIP9RTIME_SET;

    uint32_t CIP1STIME_SET;
    uint32_t CIP2STIME_SET;
    uint32_t CIP3STIME_SET;
    uint32_t CIP4STIME_SET;
    uint32_t CIP5STIME_SET;
    uint32_t CIP6STIME_SET;

    uint32_t CIP7STIME_SET;
    uint32_t CIP8STIME_SET;
    uint32_t CIP9STIME_SET;
};

class ConfigController
{

public:
    cipConfig cipConfig_;
    generalConfig generalConfig_;
    pumpConfig pumpConfig_;
    mixerConfig mixerConfig_;
    holdingPotConfig holdingPotConfig_;
    cookPotConfig cookPotConfig_;
    void saveConfigEEPROM();
    bool loadConfigEEPROM();
    void saveDefaultEEPROM();
};

bool ConfigController::loadConfigEEPROM()
{
    // cook pot config
    this->cookPotConfig_.TEMPC_SET = EEPROM.read(0);
    this->cookPotConfig_.TIME_SET = EEPROM.read(1);
    this->cookPotConfig_.WATERC_SET = EEPROM.read(2);

    // holdingPotConfig
    this->holdingPotConfig_.TEMPH_SET = EEPROM.read(3);
    this->holdingPotConfig_.WATERH_SET = EEPROM.read(4);

    this->mixerConfig_.MIXERC1_SET = EEPROM.read(5);
    this->mixerConfig_.MIXERC2_SET = EEPROM.read(6);
    this->mixerConfig_.MIXERH1_SET = EEPROM.read(7);
    this->mixerConfig_.MIXERHS_SET = EEPROM.read(8);

    this->pumpConfig_.LOBE1_SET = EEPROM.read(9);
    this->pumpConfig_.LOBE2_SET = EEPROM.read(10);
    // if(this->config_.TEMP_CALIBRATE >= 100){
    //   this->config_.TEMP_CALIBRATE=this->config_.TEMP_CALIBRATE-256;
    // }
    // if(this->config_.BAND_OFFSET>=100){
    //   this->config_.BAND_OFFSET=this->config_.BAND_OFFSET-256;
    // }
    this->generalConfig_.BAND_GAP1 = EEPROM.read(11);
    this->generalConfig_.BAND_GAP2 = EEPROM.read(12);
    this->generalConfig_.BAND_OFFSET1 = EEPROM.read(13);
    this->generalConfig_.BAND_OFFSET2 = EEPROM.read(14);

    if (this->generalConfig_.BAND_OFFSET1 >= 100)
    {
        this->generalConfig_.BAND_OFFSET1 = this->generalConfig_.BAND_OFFSET1 - 256; // EEPROM - 8 bit uint32_t - config ranges below 0
    }
    if (this->generalConfig_.BAND_OFFSET2 >= 100)
    {
        this->generalConfig_.BAND_OFFSET2 = this->generalConfig_.BAND_OFFSET2 - 256;
    }

    this->generalConfig_.DELAY_TIME1 = EEPROM.read(15);
    this->generalConfig_.DELAY_TIME2 = EEPROM.read(16);

    this->generalConfig_.TEMP_CALIBRATE1 = EEPROM.read(17);
    this->generalConfig_.TEMP_CALIBRATE2 = EEPROM.read(18);

    if (this->generalConfig_.TEMP_CALIBRATE1 >= 100)
    {
        this->generalConfig_.TEMP_CALIBRATE1 = this->generalConfig_.TEMP_CALIBRATE1 - 256;
    }
    if (this->generalConfig_.TEMP_CALIBRATE2 >= 100)
    {
        this->generalConfig_.TEMP_CALIBRATE2 = this->generalConfig_.TEMP_CALIBRATE2 - 256;
    }

    this->generalConfig_.WIDTH1 = EEPROM.read(19);
    this->generalConfig_.WIDTH2 = EEPROM.read(20);

    this->cipConfig_.CIP1RTIME_SET = EEPROM.read(21);
    this->cipConfig_.CIP1STIME_SET = EEPROM.read(22);
    this->cipConfig_.CIP2RTIME_SET = EEPROM.read(23);
    this->cipConfig_.CIP2STIME_SET = EEPROM.read(24);
    this->cipConfig_.CIP3RTIME_SET = EEPROM.read(25);
    this->cipConfig_.CIP3STIME_SET = EEPROM.read(26);
    this->cipConfig_.CIP4RTIME_SET = EEPROM.read(27);
    this->cipConfig_.CIP4STIME_SET = EEPROM.read(28);
    this->cipConfig_.CIP5RTIME_SET = EEPROM.read(29);
    this->cipConfig_.CIP5STIME_SET = EEPROM.read(30);
    this->cipConfig_.CIP6RTIME_SET = EEPROM.read(31);
    this->cipConfig_.CIP6STIME_SET = EEPROM.read(32);
    this->cipConfig_.CIP7RTIME_SET = EEPROM.read(33);
    this->cipConfig_.CIP7STIME_SET = EEPROM.read(34);
    this->cipConfig_.CIP8RTIME_SET = EEPROM.read(35);
    this->cipConfig_.CIP8STIME_SET = EEPROM.read(36);
    this->cipConfig_.CIP9RTIME_SET = EEPROM.read(37);
    this->cipConfig_.CIP9STIME_SET = EEPROM.read(38);
    this->cipConfig_.CIPPUMP_SET = EEPROM.read(39);
    this->cipConfig_.DVCPOT_SET=EEPROM.read(40);
    this->cipConfig_.DVHPOT_SET = EEPROM.read(41);
    this->cipConfig_.TEMPRINSE_SET = EEPROM.read(42);
    this->cipConfig_.TEMPSANI_SET = EEPROM.read(43);
    this->cipConfig_.WATERRINSE_SET = EEPROM.read(44);
    this->cipConfig_.WATERSANI_SET = EEPROM.read(45);
}
void ConfigController::saveConfigEEPROM()
{
    // cook pot config
    EEPROM.write(0, this->cookPotConfig_.TEMPC_SET);
    EEPROM.write(1, this->cookPotConfig_.TIME_SET);
    EEPROM.write(2, this->cookPotConfig_.WATERC_SET);

    // holdingPotConfig
    EEPROM.write(3, this->holdingPotConfig_.TEMPH_SET);
    EEPROM.write(4, this->holdingPotConfig_.WATERH_SET);

    EEPROM.write(5, this->mixerConfig_.MIXERC1_SET);
    EEPROM.write(6, this->mixerConfig_.MIXERC2_SET);
    EEPROM.write(7, this->mixerConfig_.MIXERH1_SET);
    EEPROM.write(8, this->mixerConfig_.MIXERHS_SET);

    EEPROM.write(9, this->pumpConfig_.LOBE1_SET);
    EEPROM.write(10, this->pumpConfig_.LOBE2_SET);

    EEPROM.write(11, this->generalConfig_.BAND_GAP1);
    EEPROM.write(12, this->generalConfig_.BAND_GAP2);
    EEPROM.write(13, this->generalConfig_.BAND_OFFSET1);
    EEPROM.write(14, this->generalConfig_.BAND_OFFSET2);
    EEPROM.write(15, this->generalConfig_.DELAY_TIME1);
    EEPROM.write(16, this->generalConfig_.DELAY_TIME2);
    EEPROM.write(17, this->generalConfig_.TEMP_CALIBRATE1);
    EEPROM.write(18, this->generalConfig_.TEMP_CALIBRATE2);
    EEPROM.write(19, this->generalConfig_.WIDTH1);
    EEPROM.write(20, this->generalConfig_.WIDTH2);

    EEPROM.write(21, this->cipConfig_.CIP1RTIME_SET);
    EEPROM.write(22, this->cipConfig_.CIP1STIME_SET);
    EEPROM.write(23, this->cipConfig_.CIP2RTIME_SET);
    EEPROM.write(24, this->cipConfig_.CIP2STIME_SET);
    EEPROM.write(25, this->cipConfig_.CIP3RTIME_SET);
    EEPROM.write(26, this->cipConfig_.CIP3STIME_SET);
    EEPROM.write(27, this->cipConfig_.CIP4RTIME_SET);
    EEPROM.write(28, this->cipConfig_.CIP4STIME_SET);
    EEPROM.write(29, this->cipConfig_.CIP5RTIME_SET);
    EEPROM.write(30, this->cipConfig_.CIP5STIME_SET);
    EEPROM.write(31, this->cipConfig_.CIP6RTIME_SET);
    EEPROM.write(32, this->cipConfig_.CIP6STIME_SET);
    EEPROM.write(33, this->cipConfig_.CIP7RTIME_SET);
    EEPROM.write(34, this->cipConfig_.CIP7STIME_SET);
    EEPROM.write(35, this->cipConfig_.CIP8RTIME_SET);
    EEPROM.write(36, this->cipConfig_.CIP8STIME_SET);
    EEPROM.write(37, this->cipConfig_.CIP9RTIME_SET);
    EEPROM.write(38, this->cipConfig_.CIP9STIME_SET);
    EEPROM.write(39, this->cipConfig_.CIPPUMP_SET);
    EEPROM.write(40, this->cipConfig_.DVCPOT_SET);
    EEPROM.write(41, this->cipConfig_.DVHPOT_SET);
    EEPROM.write(42, this->cipConfig_.TEMPRINSE_SET);
    EEPROM.write(43, this->cipConfig_.TEMPSANI_SET);
    EEPROM.write(44, this->cipConfig_.WATERRINSE_SET);
    EEPROM.write(45, this->cipConfig_.WATERSANI_SET);

    return true;
}
