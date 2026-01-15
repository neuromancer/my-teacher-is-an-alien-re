#include <windows.h>
#include "globals.h"

extern "C" {

/* Function start: 0x424FC0 */
void SetTimeSeed(int param_1) {
    DAT_0043bc88 = param_1;
}

/* Function start: 0x425000 */
int GetCurrentTimestamp(int* param_1);

/* Function start: 0x42a840 */
// DateTimeToTimestamp - converts date/time to Unix timestamp
// NOTE: Stub approximation. Original uses DAT_0043cc6c month-day table,
// leap year handling, and timezone adjustments (DAT_0043cc08/0c)
int DateTimeToTimestamp(int year, int month, int day, int hour, int minute, int second) {
    return ((year - 1970) * 365 + month * 30 + day) * 86400 + hour * 3600 + minute * 60 + second;
}

/* Function start: 0x425000 */
int GetCurrentTimestamp(int* param_1) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    int result = DateTimeToTimestamp(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    if (param_1 != 0) {
        *param_1 = result;
    }
    return result;
}

} // extern "C"
