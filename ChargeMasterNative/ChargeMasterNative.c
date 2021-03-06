#include "ChargeMasterNative.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg = { 0 };
    RECT rect = { 0, 0, 300, 300 };
    uint8_t i;
    wchar_t *batteryTypeDropBoxStrings[] = { L"LiPo", L"LiIo", L"LiFe", L"LiHv", L"NiMh", L"NiCd", L"Pb", NULL };


    if (!(Dialog = CreateDialogParam(NULL, MAKEINTRESOURCE(MAINWINDOW), NULL, WindowProc, 0)))
    {
        PrintError();
        exit(-1);
    }

    for (i = 0; batteryTypeDropBoxStrings[i]; ++i)
    {
        SendMessage(GetDlgItem(Dialog, IDC_COMBO_BATTERYTYPE), CB_ADDSTRING, 0, (LPARAM)batteryTypeDropBoxStrings[i]);
    }

    SendMessage(GetDlgItem(Dialog, IDC_COMBO_BATTERYTYPE), CB_SETCURSEL, 0, 0);
    SetBatterySettings(0);

    SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMIN, 1, 1);
    SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMAX, 1, 6);

    SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CHARGECURRENT_SLIDER), TBM_SETRANGEMIN, 1, 1);
    SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CHARGECURRENT_SLIDER), TBM_SETRANGEMAX, 1, 60);

    SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_DISCHARGECURRENT_SLIDER), TBM_SETRANGEMIN, 1, 1);
    SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_DISCHARGECURRENT_SLIDER), TBM_SETRANGEMAX, 1, 20);

    if (iMAXb6Init() == -1)
    {
        PrintError();
        exit(-1);
    }

    if (_beginthread(WorkerThread, 0, NULL) == -1)
    {
        PrintError();
        exit(-1);
    }

    ShowWindow(Dialog, SW_SHOW);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!IsDialogMessage(Dialog, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG result = 1;
    wchar_t buffer[16];

    switch (uMsg)
    {
        case WM_CLOSE:
        {
            iMAXb6Cleanup();
            PostQuitMessage(0);
        }
        break;

        case WM_NOTIFY:
        switch (wParam)
        {            
            case IDC_SETTINGS_CELLCOUNT_SLIDER:
            swprintf(buffer, sizeof(buffer)/sizeof(*buffer), L"%u", SendMessage(((NMHDR *)lParam)->hwndFrom, TBM_GETPOS, 0, 0));
            SetDlgItemText(Dialog, IDC_SETTINGS_CELLCOUNT_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_PP_CELL_EDIT, buffer);
            break;

            case IDC_SETTINGS_CHARGECURRENT_SLIDER:
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%.1fA", (double)SendMessage(((NMHDR *)lParam)->hwndFrom, TBM_GETPOS, 0, 0) / 10.0f);
            SetDlgItemText(Dialog, IDC_SETTINGS_CHARGECURRENT_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%u", SendMessage(((NMHDR *)lParam)->hwndFrom, TBM_GETPOS, 0, 0) *100);
            SetDlgItemText(Dialog, IDC_PP_CCCURRENT_EDIT, buffer);
            break;

            case IDC_SETTINGS_DISCHARGECURRENT_SLIDER:
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%.1fA", (double)SendMessage(((NMHDR *)lParam)->hwndFrom, TBM_GETPOS, 0, 0) / 10.0f);
            SetDlgItemText(Dialog, IDC_SETTINGS_DISCHARGECURRENT_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%u", SendMessage(((NMHDR *)lParam)->hwndFrom, TBM_GETPOS, 0, 0) * 100);
            SetDlgItemText(Dialog, IDC_PP_DCCURRENT_EDIT, buffer);
            break;

            default:

            break;
        }
        break;

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDC_BUTTON_READ_DEVICEINFO:
                QuenuedActions |= READ_DEVICEINFO;
                break;

                case IDC_BUTTON_READ_MAXIMUMCURRENT:
                QuenuedActions |= READ_SOMECHARGEDATA;
                break;

                case IDC_BUTTON_READ_CHARGEDATA:
                QuenuedActions |= READ_CHARGEDATA;
                break;

                case IDC_BUTTON_STOP_PROCESS:
                QuenuedActions |= STOP_PROCESS;
                break;

                case IDC_BUTTON_START_PROCESS:
                QuenuedActions |= START_PROCESS;
                break;

                case IDC_BUTTON_SET_TEMPLIMIT:
                QuenuedActions |= SET_TEMPLIMIT;
                break;

                case IDC_BUTTON_SET_BUZZ:
                QuenuedActions |= SET_BUZZ;
                break;

                case IDC_BUTTON_SET_CAPLIMIT:
                QuenuedActions |= SET_CAPLIMIT;
                break;

                case IDC_BUTTON_SET_TIMELIMIT:
                QuenuedActions |= SET_TIMELIMIT;
                break;

                case IDC_BUTTON_SET_CYCLETIME:
                QuenuedActions |= SET_CYCLETIME;
                break;

                case IDC_COMBO_BATTERYTYPE:
                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    SetBatterySettings((uint8_t)SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0));
                }
                break;

                default:

                break;
            }
        }
        break;

        default:
        {
            result = 0;
        }
        break;
    }


    return result;
}

void SetBatterySettings(uint8_t batteryType)
{
    wchar_t type[8];
    swprintf(type, sizeof(type) / sizeof(*type), L"%u", batteryType);
    SetDlgItemText(Dialog, IDC_PP_BATTTYPE_EDIT, type);

    SetDlgItemText(Dialog, IDC_PP_LIPWMMODE_EDIT, L"0");
    SetDlgItemText(Dialog, IDC_PP_NIPWMMODE_EDIT, L"0");
    SetDlgItemText(Dialog, IDC_PP_PBPWMMODE_EDIT, L"0");
    SetDlgItemText(Dialog, IDC_PP_R_PEAKCOUNT_EDIT, L"1");
    SetDlgItemText(Dialog, IDC_PP_CYCLETYPE_EDIT, L"1");
    SetDlgItemText(Dialog, IDC_PP_CYC_COUNT_EDIT, L"1");
    SetDlgItemText(Dialog, IDC_PP_TRICKLE_EDIT, L"0");

    switch (batteryType)
    {
        case BATTERY_LIPO:
        SetDlgItemText(Dialog, IDC_PP_CELLVOLTAGE_EDIT, L"3200");
        SetDlgItemText(Dialog, IDC_PP_ENDVOLTAGE_EDIT, L"4200");
        SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMAX, 1, 6);
        break;

        case BATTERY_LIIO:
        SetDlgItemText(Dialog, IDC_PP_CELLVOLTAGE_EDIT, L"3100");
        SetDlgItemText(Dialog, IDC_PP_ENDVOLTAGE_EDIT, L"4100");
        SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMAX, 1, 6);
        break;

        case BATTERY_LIFE:
        SetDlgItemText(Dialog, IDC_PP_CELLVOLTAGE_EDIT, L"2800");
        SetDlgItemText(Dialog, IDC_PP_ENDVOLTAGE_EDIT, L"3600");
        SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMAX, 1, 6);
        break;

        case BATTERY_LIHV:
        SetDlgItemText(Dialog, IDC_PP_CELLVOLTAGE_EDIT, L"3000");
        SetDlgItemText(Dialog, IDC_PP_ENDVOLTAGE_EDIT, L"4350");
        SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMAX, 1, 6);
        break;

        case BATTERY_NIMH:
        case BATTERY_NICD: //yes, same category
        SetDlgItemText(Dialog, IDC_PP_CELLVOLTAGE_EDIT, L"900");
        SetDlgItemText(Dialog, IDC_PP_ENDVOLTAGE_EDIT, L"4");
        SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMAX, 1, 6);
        break;

        case BATTERY_PB:
        SetDlgItemText(Dialog, IDC_PP_CELLVOLTAGE_EDIT, L"1800");
        SetDlgItemText(Dialog, IDC_PP_ENDVOLTAGE_EDIT, L"0");
        SendMessage(GetDlgItem(Dialog, IDC_SETTINGS_CELLCOUNT_SLIDER), TBM_SETRANGEMAX, 1, 6);
        break;

        default:

        break;
    }

}

void WorkerThread(void *lpThreadParameter)
{
    struct ChargeInfo ChargeInfo;
    struct DeviceInfo DeviceInfo;
    struct ChargeData ChargeData;
    struct MaximumCurrent SomeChargeData;
    struct ProcessParams processParams;
    wchar_t buffer[1024];

    while (1)
    {
        if (QuenuedActions & READ_DEVICEINFO)
        {
            QuenuedActions &= ~READ_DEVICEINFO;
            if (iMAXb6GetDeviceInfo(&DeviceInfo) != -1)
            {
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hs", DeviceInfo.core_type);
                SetDlgItemText(Dialog, IDC_DI_CORE_TYPE_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", DeviceInfo.upgrade_type);
                SetDlgItemText(Dialog, IDC_DI_UPDATE_TYPE_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", DeviceInfo.is_encrypt);
                SetDlgItemText(Dialog, IDC_DI_IS_ENCRYPT_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", DeviceInfo.customer_id);
                SetDlgItemText(Dialog, IDC_DI_CUSTOMER_ID_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", DeviceInfo.language_id);
                SetDlgItemText(Dialog, IDC_DI_LANGUAGE_ID_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%g", DeviceInfo.software_version);
                SetDlgItemText(Dialog, IDC_DI_SOFTWARE_VERSION_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%g", DeviceInfo.hardware_version);
                SetDlgItemText(Dialog, IDC_DI_HARDWARE_VERSION_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", DeviceInfo.reserved);
                SetDlgItemText(Dialog, IDC_DI_RESERVED_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", DeviceInfo.mctype);
                SetDlgItemText(Dialog, IDC_DI_MCTYPE_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", DeviceInfo.checksum);
                SetDlgItemText(Dialog, IDC_DI_CHECKSUM_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hs", DeviceInfo.machine_id);
                SetDlgItemText(Dialog, IDC_DI_MACHINE_ID_MONITOR, buffer);

            }
            else
            {
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"-");
                SetDlgItemText(Dialog, IDC_DI_CORE_TYPE_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_UPDATE_TYPE_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_IS_ENCRYPT_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_CUSTOMER_ID_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_LANGUAGE_ID_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_SOFTWARE_VERSION_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_HARDWARE_VERSION_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_RESERVED_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_CHECKSUM_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_MACHINE_ID_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_DI_MCTYPE_MONITOR, buffer);
            }
        }

        if (QuenuedActions & READ_SOMECHARGEDATA)
        {
            QuenuedActions &= ~READ_SOMECHARGEDATA;
            if (iMAXb6GetMaxCurrent(&SomeChargeData) != -1)
            {
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", SomeChargeData.maxChargeCurrent);
                SetDlgItemText(Dialog, IDC_MC_MAXCHARGECURRENT_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", SomeChargeData.maxDischargeCurrent);
                SetDlgItemText(Dialog, IDC_MC_MAXDISCHARGECURRENT_MONITOR, buffer);

            }
            else
            {
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"-");
                SetDlgItemText(Dialog, IDC_MC_MAXCHARGECURRENT_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_MC_MAXDISCHARGECURRENT_MONITOR, buffer);
            }
        }

        if (QuenuedActions & READ_CHARGEDATA)
        {
            QuenuedActions &= ~READ_CHARGEDATA;
            if (iMAXb6GetChargeData(&ChargeData) != -1)
            {
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeData.CycleTime);
                SetDlgItemText(Dialog, IDC_CD_CYCLETIME_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeData.TimeLimitEnable);
                SetDlgItemText(Dialog, IDC_CD_TIMELIMITENABLE_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.TimeLimit);
                SetDlgItemText(Dialog, IDC_CD_TIMELIMIT_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeData.CapLimitEnable);
                SetDlgItemText(Dialog, IDC_CD_CAPLIMITENABLE_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.CapLimit);
                SetDlgItemText(Dialog, IDC_CD_CAPLIMIT_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeData.KeyBuzz);
                SetDlgItemText(Dialog, IDC_CD_KEYBUZZ_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeData.SysBuzz);
                SetDlgItemText(Dialog, IDC_CD_SYSBUZZ_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.InDClow);
                SetDlgItemText(Dialog, IDC_CD_INDCLOW_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeData.TempLimit);
                SetDlgItemText(Dialog, IDC_CD_TEMPLIMIT_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.Voltage);
                SetDlgItemText(Dialog, IDC_CD_VOLTAGE_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.CELL1);
                SetDlgItemText(Dialog, IDC_CD_CELL1_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.CELL2);
                SetDlgItemText(Dialog, IDC_CD_CELL2_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.CELL3);
                SetDlgItemText(Dialog, IDC_CD_CELL3_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.CELL4);
                SetDlgItemText(Dialog, IDC_CD_CELL4_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.CELL5);
                SetDlgItemText(Dialog, IDC_CD_CELL5_MONITOR, buffer);
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeData.CELL6);
                SetDlgItemText(Dialog, IDC_CD_CELL6_MONITOR, buffer);

            }
            else
            {
                swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"-");
                SetDlgItemText(Dialog, IDC_CD_CYCLETIME_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_TIMELIMITENABLE_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_TIMELIMIT_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CAPLIMITENABLE_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CAPLIMIT_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_KEYBUZZ_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_SYSBUZZ_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_INDCLOW_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_TEMPLIMIT_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_VOLTAGE_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CELL1_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CELL2_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CELL3_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CELL4_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CELL5_MONITOR, buffer);
                SetDlgItemText(Dialog, IDC_CD_CELL6_MONITOR, buffer);
            }
        }

        if (QuenuedActions & STOP_PROCESS)
        {
            QuenuedActions &= ~STOP_PROCESS;
            iMAXb6StopProcess();
        }

        if (QuenuedActions & START_PROCESS)
        {
            QuenuedActions &= ~START_PROCESS;
            GetDlgItemText(Dialog, IDC_PP_BATTTYPE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.BattType = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_CELL_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.Cell = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_LIPWMMODE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.LiPwmMode = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_NIPWMMODE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.NiPwmMode = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_PBPWMMODE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.PbPwmMode = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_CCCURRENT_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.CCurrent = (uint16_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_DCCURRENT_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.DCurrent = (uint16_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_CELLVOLTAGE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.CellVoltage = (uint16_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_ENDVOLTAGE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.EndVoltage = (uint16_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_R_PEAKCOUNT_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.R_PeakCount = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_CYCLETYPE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.CycleType = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_CYC_COUNT_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.Cyc_count = (uint8_t)wcstoul(buffer, NULL, 10);
            GetDlgItemText(Dialog, IDC_PP_TRICKLE_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            processParams.Trickle = (uint16_t)wcstoul(buffer, NULL, 10);
            iMAXb6StartProcess(&processParams);
        }

        if (iMAXb6GetChargeInfo(&ChargeInfo) != -1)
        {
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeInfo.workState);
            SetDlgItemText(Dialog, IDC_CI_WORKSTATE_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.ChargeMah);
            SetDlgItemText(Dialog, IDC_CI_CHARGEMAH_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.ChargeTimer);
            SetDlgItemText(Dialog, IDC_CI_CHARGETIMER_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.OutVoltage);
            SetDlgItemText(Dialog, IDC_CI_OUTVOLTAGE_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.Current);
            SetDlgItemText(Dialog, IDC_CI_CURRENT_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeInfo.ExtTemp);
            SetDlgItemText(Dialog, IDC_CI_EXTTEMP_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hhu", ChargeInfo.IntTemp);
            SetDlgItemText(Dialog, IDC_CI_INTTEMP_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.Intimpedance);
            SetDlgItemText(Dialog, IDC_CI_INTIMPEDANCE_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL1);
            SetDlgItemText(Dialog, IDC_CI_CELL1_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL2);
            SetDlgItemText(Dialog, IDC_CI_CELL2_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL3);
            SetDlgItemText(Dialog, IDC_CI_CELL3_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL4);
            SetDlgItemText(Dialog, IDC_CI_CELL4_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL5);
            SetDlgItemText(Dialog, IDC_CI_CELL5_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL6);
            SetDlgItemText(Dialog, IDC_CI_CELL6_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL7);
            SetDlgItemText(Dialog, IDC_CI_CELL7_MONITOR, buffer);
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"%hu", ChargeInfo.CELL8);
            SetDlgItemText(Dialog, IDC_CI_CELL8_MONITOR, buffer);

        }
        else
        {
            swprintf(buffer, sizeof(buffer) / sizeof(*buffer), L"-");
            SetDlgItemText(Dialog, IDC_CI_WORKSTATE_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CHARGEMAH_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CHARGETIMER_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_OUTVOLTAGE_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CURRENT_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_EXTTEMP_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_INTTEMP_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_INTIMPEDANCE_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL1_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL2_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL3_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL4_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL5_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL6_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL7_MONITOR, buffer);
            SetDlgItemText(Dialog, IDC_CI_CELL8_MONITOR, buffer);
        }

        if (QuenuedActions & SET_TEMPLIMIT)
        {
            QuenuedActions &= ~SET_TEMPLIMIT;
            GetDlgItemText(Dialog, IDC_SETTINGS_TEMPLIMIT_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            iMAXb6SetTempLimit((uint8_t)wcstoul(buffer, NULL, 10));

        }

        if (QuenuedActions & SET_BUZZ)
        {
            QuenuedActions &= ~SET_BUZZ;
            iMAXb6SetBuzz((uint8_t)IsDlgButtonChecked(Dialog, IDC_SETTINGS_KEYBUZZ_CHECK), (uint8_t)IsDlgButtonChecked(Dialog, IDC_SETTINGS_SYSBUZZ_CHECK));
        }

        if (QuenuedActions & SET_CAPLIMIT)
        {
            QuenuedActions &= ~SET_CAPLIMIT;
            GetDlgItemText(Dialog, IDC_SETTINGS_CAPLIMIT_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            iMAXb6SetCapLimit((uint8_t)IsDlgButtonChecked(Dialog, IDC_SETTINGS_CAPLIMITENABLE_CHECK), (uint16_t)wcstoul(buffer, NULL, 10));
        }

        if (QuenuedActions & SET_TIMELIMIT)
        {
            QuenuedActions &= ~SET_TIMELIMIT;
            GetDlgItemText(Dialog, IDC_SETTINGS_TIMELIMIT_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            iMAXb6SetTimelimit((uint8_t)IsDlgButtonChecked(Dialog, IDC_SETTINGS_TIMELIMITENABLE_CHECK), (uint16_t)wcstoul(buffer, NULL, 10));

        }

        if (QuenuedActions & SET_CYCLETIME)
        {
            QuenuedActions &= ~SET_CYCLETIME;
            GetDlgItemText(Dialog, IDC_SETTINGS_CYCLETIME_EDIT, buffer, sizeof(buffer) / sizeof(*buffer));
            iMAXb6SetCycleTime((uint8_t)wcstoul(buffer, NULL, 10));
        }

        Sleep(500);

    }
}

void PrintError()
{
    WCHAR *error_description = NULL;
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&error_description, 0, 0) != 0)
    {
        MessageBox(NULL, error_description, L"Error", MB_OK | MB_ICONERROR);
        HeapFree(GetProcessHeap(), 0, error_description);
    }
    else
    {
        MessageBox(NULL, L"Error, FormatMessage failed to get details", L"Error", MB_OK | MB_ICONERROR);
    }
}
