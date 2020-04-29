This mini-library allows to retrieve API address in loaded modules **without**
calling standard API *GetProcAddress*.

Usually this code is found in assembly-written malware to avoid detection and to
bypass other PE-format restrictions on functions export.

This code is written in C with some extension to the standard specification and
can be compiled in both 32-bit and 64-bit  without modifying anything.

Actually VisualStudio and Digitalmars compiler support this code.

The code just export a single api:
```C
FARPROC get_api_address(LPCSTR name, LPCVOID its_page, LPCVOID* found_page);
```
See code for technical details

All this code is released in public domain
