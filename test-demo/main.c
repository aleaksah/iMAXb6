#include "test-demo.h"


void onexitfn(void)
{

}

int main(int argc, char **argv)
{
    struct ChargeInfo state;
    struct ChargeData cdata;
    struct DeviceInfo dinfo;
    struct MaximumCurrent scinfo;
    struct ProcessParams pparams;

    atexit(onexitfn);

    if (iMAXb6Init() == -1)
    {
#ifdef _DEBUG
        printf("iMAXb6Init error\n");
#endif
        return -1;
    }

    if (iMAXb6GetChargeInfo(&state) == -1)
    {
#ifdef _DEBUG
        printf("iMAXb6GetState error\n");
#endif
        return -1;
    }

    printf(
        "workState    " "%hhu" " ?"    "\n"
        "ChargeMah    " "%hu"  " mAh"  "\n"
        "ChargeTimer  " "%hu"  " s"    "\n"
        "OutVoltage   " "%hu"  " mV"   "\n"
        "Current      " "%hd"  " mA"   "\n"
        "ExtTemp      " "%hhd" " C"    "\n"
        "IntTemp      " "%hhd" " C"    "\n"
        "Intimpedance " "%hu"  " ?"    "\n"
        "CELL1        " "%hu"  " ?"    "\n"
        "CELL2        " "%hu"  " ?"    "\n"
        "CELL3        " "%hu"  " ?"    "\n"
        "CELL4        " "%hu"  " ?"    "\n"
        "CELL5        " "%hu"  " ?"    "\n"
        "CELL6        " "%hu"  " ?"    "\n"
        "CELL7        " "%hu"  " ?"    "\n"
        "CELL8        " "%hu"  " ?"    "\n"
        ,
        state.workState,
        state.ChargeMah,
        state.ChargeTimer,
        state.OutVoltage,
        state.Current,
        state.ExtTemp,
        state.IntTemp,
        state.Intimpedance,
        state.CELL1,
        state.CELL2,
        state.CELL3,
        state.CELL4,
        state.CELL5,
        state.CELL6,
        state.CELL7,
        state.CELL8
    );

    printf("\n\n");

    if (iMAXb6GetChargeData(&cdata) == -1)
    {
#ifdef _DEBUG
        printf("iMAXb6GetState error\n");
#endif
        return -1;
    }

    printf(
        "CycleTime        " "%hhu" " ?"   "\n"
        "TimeLimitEnable  " "%hhu" " b"   "\n"
        "TimeLimit        " "%hu"  " ?"   "\n"
        "CapLimitEnable   " "%hhu" " b"   "\n"
        "CapLimit         " "%hd"  " ?"   "\n"
        "KeyBuzz          " "%hhd" " b"   "\n"
        "SysBuzz          " "%hhd" " b"   "\n"
        "InDClow          " "%hu"  " ?"   "\n"
        "TempLimit        " "%hhu" " ?"   "\n"
        "Voltage          " "%hu"  " ?"   "\n"
        "CELL1            " "%hu"  " ?"   "\n"
        "CELL2            " "%hu"  " ?"   "\n"
        "CELL3            " "%hu"  " ?"   "\n"
        "CELL4            " "%hu"  " ?"   "\n"
        "CELL5            " "%hu"  " ?"   "\n"
        "CELL6            " "%hu"  " ?"   "\n"
        ,
        cdata.CycleTime,
        cdata.TimeLimitEnable,
        cdata.TimeLimit,
        cdata.CapLimitEnable,
        cdata.CapLimit,
        cdata.KeyBuzz,
        cdata.SysBuzz,
        cdata.InDClow,
        cdata.TempLimit,
        cdata.Voltage,
        cdata.CELL1,
        cdata.CELL2,
        cdata.CELL3,
        cdata.CELL4,
        cdata.CELL5,
        cdata.CELL6
    );

    printf("\n\n");

    if (iMAXb6GetDeviceInfo(&dinfo) == -1)
    {
#ifdef _DEBUG
        printf("iMAXb6GetState error\n");
#endif
        return -1;
    }

    printf(
        "core_type         " "%s"   " ?"   "\n"
        "upgrade_type      " "%hhu" " ?"   "\n"
        "is_encrypt        " "%hhu" " b"   "\n"
        "customer_id       " "%hu"  " ?"   "\n"
        "language_id       " "%hhu" " ?"   "\n"
        "software_version  " "%g"   " v"   "\n"
        "hardware_version  " "%g"   " v"   "\n"
        "reserved          " "%hhu" " ?"   "\n"
        "mctype            " "%hhu" " ?"   "\n"
        "checksum          " "%hhu" " ?"   "\n"
        "machine_id        " "%s"   " ?"   "\n"
        ,
        dinfo.core_type,
        dinfo.upgrade_type,
        dinfo.is_encrypt,
        dinfo.customer_id,
        dinfo.language_id,
        dinfo.software_version,
        dinfo.hardware_version,
        dinfo.reserved,
        dinfo.mctype,
        dinfo.checksum,
        dinfo.machine_id
    );

    printf("\n\n");

    if (iMAXb6GetMaxCurrent(&scinfo) == -1)
    {
#ifdef _DEBUG
        printf("iMAXb6GetChargeData error\n");
#endif
        return -1;
    }
        
    printf(
        "a        " "%hhu"   " ?"   "\n"
        "b        " "%hhu"   " ?"   "\n"
        ,
        scinfo.maxChargeCurrent,
        scinfo.maxDischargeCurrent
    );


    printf("\n\n");

    pparams.BattType = 1;
    pparams.Cell = 1;
    pparams.LiPwmMode = 0;
    pparams.NiPwmMode = 0;
    pparams.PbPwmMode = 0;
    pparams.CCurrent = 100;
    pparams.DCurrent = 100;
    pparams.CellVoltage = 3100;
    pparams.EndVoltage = 4100;
    pparams.R_PeakCount = 1;
    pparams.CycleType = 1;
    pparams.Cyc_count = 1;
    pparams.Trickle = 0;

    if (iMAXb6StartProcess(&pparams) == -1)
    {
#ifdef _DEBUG
        printf("iMAXbStartProcess error\n");
#endif
        return -1;
    }

    Sleep(2000);

    if (iMAXb6StopProcess() == -1)
    {
#ifdef _DEBUG
        printf("iMAXbStopProcess error\n");
#endif
        return -1;
    }
    
    if (iMAXb6Cleanup() == -1)
    {
#ifdef _DEBUG
        printf("iMAXb6Cleanup error\n");
#endif
        return -1;
    }

    return 0;
}