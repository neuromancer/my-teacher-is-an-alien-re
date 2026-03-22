#include <windows.h>
#include "globals.h"

extern "C" {

/* Function start: 0x42a840 */ /* DEMO ONLY - no full game match */
// DateTimeToTimestamp - converts date/time to Unix timestamp
// NOTE: Stub approximation. Original uses DAT_0043cc6c month-day table,
// leap year handling, and timezone adjustments (DAT_0043cc08/0c)
int DateTimeToTimestamp(int year, int month, int day, int hour, int minute, int second) {
    return ((year - 1970) * 365 + month * 30 + day) * 86400 + hour * 3600 + minute * 60 + second;
}

} // extern "C"
