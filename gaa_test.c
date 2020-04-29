#include <stdio.h>
#include "get_api_address.h"

int main(void)
{
    typedef VOID (*exit_t)(UINT);
    exit_t ep;
    LPCVOID page;

    ep = (exit_t)get_api_address("ExitProcess", NULL, &page);
    if (ep != NULL) {
        typedef DWORD (*gtc_t)(VOID);
        gtc_t gtc;

        puts("ExitProcess found, searching GetTickCount by one shot");

        gtc = (gtc_t)get_api_address("GetTickCount", page, NULL);
        if (gtc != NULL) {
            DWORD start, end, delta;

            puts("FOUND by one shot!!!");

            start = gtc();
            /* Some code... */
            end = gtc();

            if (start <= end) {
                delta = end - start;
            }
            else { /* Fix overflow */
                delta = (DWORD)(-1) - start + end + 1;
            }
            if (delta > 1000U) {
                ep(1); /* Too much time, i'm debugged, exit 1 */
            }
        }
    }

    return 0;
}
