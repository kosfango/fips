# FIPS version 02.01alpha1
FIDO Integrated Point System.

## How to build
Use MS Visual studio 2015. Open Light.sln for start.

1. Configure deployment directory using Project Settings->Linker (for example D:\fipstest)

2. Put files from defs\fipstest into deployment directory fips.exe

3. Put files supercom.dll, protocol.dll and *.dll from .\Unipack directory into deployment directory fips.exe

4. Create fips.ini into deployment directory and put next lines:

[Common]

Initialization=1

Floatbar=1

This is temporary fix