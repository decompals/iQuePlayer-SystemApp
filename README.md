# iQue Player System App

This repository contains an in-progress decompilation of the iQue Player System App (SA). The System App consists of two binaries:
 - `sa1`: Handles USB communication when the console is powered on with the USB plugged in.
 - `sa2`: The menu itself.

There are many versions of sa1 and sa2, this repository currently only targets the 1106 version of sa1.
`SHA1 d5db3d072ff2028f346c731e3f0f1531e23c4d9d`

## Building

Clone the repository and `make setup`.

Requires `make`, `python3` and `binutils-mips-linux-gnu`, alternatively a different MIPS toolchain can be used by overriding `CROSS` in the makefile.
