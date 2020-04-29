/*
    This code is released in public domain
    I'm not responsable of any damage caused by this code
    I really don't care what will you do with this code
    Michele Pes 29 APR 2020
*/

#ifndef GET_API_ADDRESS_H_
#define GET_API_ADDRESS_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
name:       The name of the API to find (ex.: "ExitProcess")
its_page:   The valid page, in the first invocation must be NULL.
            If not NULL, found_page will be ignored
found_page: If not NULL and API found, will receive the page of the API.
            This can be passed as its_page parameter in next calls to find other
            APIs in the same module but much more faster

One shot:
FARPROC ep = get_api_address("ExitProcess", NULL, NULL);

Get 2 APIs from kernel (assumed always loaded):
LPVOID page;
FARPROC ep = get_api_address("ExitProcess", NULL, &page);
FARPROC gpa = get_api_address("GetProcAddress", page, NULL);
*/

FARPROC get_api_address(LPCSTR name, LPCVOID its_page, LPCVOID* found_page);

#endif /* GET_API_ADDRESS_H_ */
