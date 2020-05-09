/*
    This code is released in public domain
    I'm not responsable of any damage caused by this code
    I really don't care what will you do with this code
    Michele Pes 29 APR 2020
*/

#include "get_api_address.h"

/*
Magic header and rva offset are the ONLY differences between 32-bit and 64-bit!
IMAGE_NT_OPTIONAL_HDR??_MAGIC may be undefined in your compiler
*/
#ifdef _WIN64
#define HEADER_MAGIC    0x20BU
#define RVA_OFFSET      0x88U
#else
#define HEADER_MAGIC    0x10BU
#define RVA_OFFSET      0x78U
#endif

#define START_ADDRESS   ((1U << 31U) - (EXE_SECTOR * 2U)) /* 2GB - 128K */
#define EXE_SECTOR      (64U << 10U) /* 64K */

#define GETT(t, p)      (*((t*)(p)))
#define GET_WORD(p)     GETT(WORD, p)
#define GET_DWORD(p)    GETT(DWORD, p)

/* Remove any dependency from libc */
static INT ga_strcmp(LPCSTR s1, LPCSTR s2)
{
    while (*s1 == *s2 && *s1 != 0) {
        s1++;
        s2++;
    }
    return (INT)(*s1) - (INT)(*s2);
}

static FARPROC scan_block(LPCSTR name, LPCVOID pv)
{
    FARPROC fp = NULL;
    LPCSTR CONST mz = (LPCSTR)pv;
    LPCSTR CONST pe = mz + GET_DWORD(mz + 0x3CU);

    do {
        LPCSTR p_export_table;
        CONST DWORD* p_names;
        DWORD i, count, temp;

        /* Check for PE header, masking it */
        p_names = (CONST DWORD*)pe;
        if ((*p_names << 1U) != 0x00008aa0U) break;

        /* Check for magic header 32/64 */
        if (GET_WORD(pe + 0x18U) != HEADER_MAGIC) break;

        /* Check for valid RVA */
        temp = GET_DWORD(pe + RVA_OFFSET);
        if (temp == 0U) break;

        /* Now points to the export table, to be saved */
        p_export_table = mz + temp;

        /* Save number of exported functions */
        count = GET_DWORD(p_export_table + 0x18U);

        /* Now points to start of names */
        p_names = (CONST LPDWORD)(mz + GET_DWORD(p_export_table + 0x20U));

        /* Scan for our function name */
        for (i = 0U; fp == NULL && i < count; i++) {
            LPCSTR CONST api_name = mz + p_names[i];

            if (ga_strcmp(api_name, name) == 0) {
                CONST DWORD* p;
                CONST WORD* p2;

                /* Match found, caluclate function address */
                p = (CONST LPDWORD)(mz + GET_DWORD(p_export_table + 0x1CU));
                p2 = (CONST LPWORD)(mz + GET_DWORD(p_export_table + 0x24U));
                temp = GET_DWORD(p_export_table + 0x18U) - (count - i);
                fp = (FARPROC)(mz + p[p2[temp]]);
            }
        }

    } while (FALSE);

    return fp;
}

FARPROC get_api_address(LPCSTR name, LPCVOID its_page, LPCVOID* found_page)
{
    FARPROC fp = NULL;

    if (its_page != NULL) {
        fp = scan_block(name, its_page);
    }
    else {
        LPCSTR p = (LPCSTR)START_ADDRESS;

        do {

            __try {
                /* Mask "MZ" comparison. Access may fail */
                if ((*((WORD*)p) << 1U) == 0xb49a) {
                    fp = scan_block(name, p);
                }
            }
            __except(EXCEPTION_EXECUTE_HANDLER) {
                /* Empty handler, just continue to previous sector */
            }

            p -= EXE_SECTOR;

        } while (fp == NULL && p != NULL);

        if (found_page != NULL && fp != NULL) {
            *found_page = p + EXE_SECTOR;
        }
    }

    return fp;
}
