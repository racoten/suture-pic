# DFR remap for Suture
# Comprehensive (ntdll, kernel32, user32, ws2_32, winhttp, wininet, ole32, rpcrt4, psapi)
# plus advapi32 (GetUserNameA, etc.)
x64:
    run "dfr_comprehensive.spec"
    run "libs/libadvapi32.spec"
