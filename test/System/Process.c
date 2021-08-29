#include <System\Process.h>
#include <stdio.h>
#include <tchar.h>

BOOL CreateUserAccountProcessTestC()
{
    BOOL result = TRUE;
    PVOID OldValue = NULL;
    Wow64DisableWow64FsRedirection(&OldValue);
    {
        PWTS_SESSION_INFO sessionInfo = NULL;
        DWORD count = 0;
        if (WTSEnumerateSessions(WTS_CURRENT_SERVER, 0, 1, &sessionInfo, &count))
        {
            BOOL ret = FALSE;
            BOOL isLocalSystem = IsLocalSystemToken(GetCurrentProcessToken());
            PROCESS_INFORMATION pi;
            STARTUPINFO si;
            PTSTR cmd = _tcsdup(TEXT("CMD /C WHOAMI /ALL"));
            ZeroMemory(&si, sizeof(STARTUPINFO));
            si.cb = sizeof(si);
            if (cmd)
            {
                DWORD i;
                for (i = 0; i < count; ++i)
                {
                    if (sessionInfo[i].State == WTSListen)
                        continue;
#if _MSC_VER > 1600
                    printf("CreateUserAccountProcess (SessionId: %d, pWinStationName: %Ts, State: %d)\n",
#else
#ifdef _UNICODE
                    wprintf(L"CreateUserAccountProcess (SessionId: %d, pWinStationName: %s, State: %d)\n",
#else
                    printf("CreateUserAccountProcess (SessionId: %d, pWinStationName: %s, State: %d)\n",
#endif
#endif
                           sessionInfo[i].SessionId, sessionInfo[i].pWinStationName, sessionInfo[i].State);
                    ret = CreateUserAccountProcess(sessionInfo[i].SessionId, NULL, cmd, NULL, NULL, FALSE, 0, NULL,
                                                   NULL, &si, &pi);
                    if (ret)
                    {
                        WaitForSingleObject(pi.hProcess, INFINITE);
                    }
                    else
                    {
                        printf("Failed to CreateUserAccountProcess : %d\n", GetLastError());
                    }

                    if (isLocalSystem || WTSGetActiveConsoleSessionId() == sessionInfo[i].SessionId)
                    {
                        result = ret;
                    }
                }

                WTSFreeMemory(sessionInfo);
            }
            free(cmd);
        }
    }
    Wow64RevertWow64FsRedirection(OldValue);

    return result;
}

BOOL CreateSystemAccountProcessTestC()
{
    if (!IsUserAdmin(NULL))
        return TRUE;

    {
        BOOL ret = FALSE;
        PVOID OldValue = NULL;
        Wow64DisableWow64FsRedirection(&OldValue);
        {
            PWTS_SESSION_INFO sessionInfo = NULL;
            DWORD count = 0;
            if (WTSEnumerateSessions(WTS_CURRENT_SERVER, 0, 1, &sessionInfo, &count))
            {
                PROCESS_INFORMATION pi;
                STARTUPINFO si;
                PTSTR cmd = _tcsdup(TEXT("CMD /C QUERY SESSION"));
                ZeroMemory(&si, sizeof(STARTUPINFO));
                si.cb = sizeof(si);
                if (cmd)
                {
                    DWORD i;
                    for (i = 0; i < count; ++i)
                    {
                        if (sessionInfo[i].State == WTSListen)
                            continue;
#if _MSC_VER > 1600
                        printf("CreateSystemAccountProcess (SessionId: %d, pWinStationName: %Ts, State: %d)\n",
#else
#ifdef _UNICODE
                        wprintf(L"CreateSystemAccountProcess (SessionId: %d, pWinStationName: %s, State: %d)\n",
#else
                        printf("CreateSystemAccountProcess (SessionId: %d, pWinStationName: %s, State: %d)\n",
#endif
#endif
                               sessionInfo[i].SessionId, sessionInfo[i].pWinStationName, sessionInfo[i].State);
                        ret = CreateSystemAccountProcess(sessionInfo[i].SessionId, NULL, cmd, NULL, NULL, FALSE, 0,
                                                         NULL, NULL, &si, &pi);
                        if (ret)
                        {
                            WaitForSingleObject(pi.hProcess, INFINITE);
                        }
                        else
                        {
                            printf("Failed to CreateSystemAccountProcess : %d\n", GetLastError());
                        }
                    }

                    WTSFreeMemory(sessionInfo);
                }
                free(cmd);
            }
        }
        Wow64RevertWow64FsRedirection(OldValue);
        return ret;
    }
}
