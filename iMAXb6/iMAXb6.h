#ifndef IMAXB6_H
#define IMAXB6_H

#include <stdint.h>

#ifdef IMAXB6_INNER_DLL
#define IMAXB6_FUNC __declspec(dllexport)
#else
#define IMAXB6_FUNC __declspec(dllimport)
#endif

IMAXB6_FUNC int iMAXb6Init();
IMAXB6_FUNC int iMAXb6Cleanup();
IMAXB6_FUNC int iMAXb6GetChargeInfo(struct ChargeInfo *chargeState);
IMAXB6_FUNC int iMAXb6GetDeviceInfo(struct DeviceInfo *devInfo);
IMAXB6_FUNC int iMAXb6GetChargeData(struct ChargeData *chargeData);
IMAXB6_FUNC int iMAXb6GetMaxCurrent(struct MaximumCurrent *someChargeData);
IMAXB6_FUNC int iMAXb6StartProcess(struct ProcessParams *processParams);
IMAXB6_FUNC int iMAXb6StopProcess();
IMAXB6_FUNC int iMAXb6SetCycleTime(uint8_t cycleTime);
IMAXB6_FUNC int iMAXb6SetTimelimit(uint8_t enable, uint16_t timeLimit);
IMAXB6_FUNC int iMAXb6SetCapLimit(uint8_t enable, uint16_t capLimit);
IMAXB6_FUNC int iMAXb6SetBuzz(uint8_t enableKeyBuzz, uint8_t enableSysBuzz);
IMAXB6_FUNC int iMAXb6SetTempLimit(uint8_t tempLimit);

enum BatteryType
{
    BATTERY_LIPO = 0,
    BATTERY_LIIO = 1,
    BATTERY_LIFE = 2,
    BATTERY_LIHV = 3,
    BATTERY_NIMH = 4,
    BATTERY_NICD = 5,
    BATTERY_PB = 6
};

struct ChargeInfo
{
    uint8_t workState;      // some state. 1 - charging/discharging, 2 - no process, 3 - end of process caused by discharge cutoff/by charge finish, 4 - error, check main port/balance connect
    uint16_t ChargeMah;     // current, mAh
    uint16_t ChargeTimer;   // seconds from start
    uint16_t OutVoltage;    // voltage on battery, mV
    uint16_t Current;       // current, mA
    int8_t ExtTemp;         // temperature on battery, C
    int8_t IntTemp;         // temperature on charger, C
    uint16_t Intimpedance;  // battery impedance?
    uint16_t CELL1;         // ??
    uint16_t CELL2;         // ??
    uint16_t CELL3;         // ??
    uint16_t CELL4;         // ??
    uint16_t CELL5;         // ??
    uint16_t CELL6;         // ??
    uint16_t CELL7;         // ??
    uint16_t CELL8;         // ??
};

struct DeviceInfo
{
    char core_type[23];     // ??
    uint8_t upgrade_type;   // ?? booltoader type??
    uint8_t is_encrypt;     // ?? bool
    uint16_t customer_id;   // ??
    uint8_t language_id;    // ??
    float software_version; // FW version
    float hardware_version; // HW ver?
    uint8_t reserved;       // ??
    uint8_t mctype;         // ??
    uint8_t checksum;       // ??
    char machine_id[33];    // ??
};

struct ChargeData
{
    uint8_t CycleTime;       //charge->discharge rest time, min
    uint8_t TimeLimitEnable; //bool
    uint16_t TimeLimit;      //safety timer value, min
    uint8_t CapLimitEnable;  //bool
    uint16_t CapLimit;       //capacity cut-off, mAh
    uint8_t KeyBuzz;         //bool, key beep value
    uint8_t SysBuzz;         //bool, buzzer value
    uint16_t InDClow;        //input power low value, mV
    uint8_t TempLimit;       //temp cut-off value, C
    uint16_t Voltage;        //?
    uint16_t CELL1;          //?
    uint16_t CELL2;          //?
    uint16_t CELL3;          //?
    uint16_t CELL4;          //?
    uint16_t CELL5;          //?
    uint16_t CELL6;          //?
};

struct MaximumCurrent
{
    uint8_t maxChargeCurrent;//in A
    uint8_t maxDischargeCurrent;//in A
};

struct ProcessParams
{
    uint8_t  BattType;       //hi-group of battery types?
    uint8_t  Cell;           //Cell count?
    uint8_t  LiPwmMode;      //low group of bat types?
    uint8_t  NiPwmMode;      //low group of bat types?
    uint8_t  PbPwmMode;      //low group of bat types?
    uint16_t CCurrent;       //charge current in mA?
    uint16_t DCurrent;       //discharge current in mA?
    uint16_t CellVoltage;    //min cell voltage?
    uint16_t EndVoltage;     //max cell voltage?
    uint8_t  R_PeakCount;    //?
    uint8_t  CycleType;      //?
    uint8_t  Cyc_count;      //?
    uint16_t Trickle;        //?
};



#endif
