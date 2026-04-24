# 🔍 Hardware and firmware reconnaissance

## 🧾 1. Device Information

**Manufacturer**: ECS ELITGROUP

**Model**: G41T-R3

**Revision**: V1.0A

**Device type**: LGA775 Motherboard

**Date**: 2010-05-14

<img src="/docs/images/g41t-r3.jpg" alt="Motherboard G41T-R3" width="800" height="600" style="max-width: 100%; height: auto;">

*Figure 1: G41T-R3 Motherboard illustration.*

## 🔌 2. Memory Chip (BIOS / Flash)

**Chip model**: EN25F80

**Manufacturer**: Eon Silicon Solution Inc.

**Type**: Serial Flash Memory

**Capacity**: 8M-bit (1024K-byte)

**Datasheet**: [Link to datasheet](https://www.alldatasheet.com/datasheet-pdf/view/207645/EON/EN25F80.html)

## 🧠 3. Firmware / BIOS Info

**Firmware version**: ~080015 Core Version

**Date**: 2010-06-17

**Firmware vendor**: American Megatrends (AMI)

**Type**: legacy BIOS (non-UEFI)

## 📦 4. Dump Information

**File size**: 1MB

**Dump method**: Microcontroller-based (ESP32)

**Tool used**: ImHex, xxd, dd, diff, binwalk, strings, file, ghidra

**Dump verified**: Yes

**Hash**: `MD5-86ac36b8614648753867f2a507d4200a`

## 🔍 5. Initial Observations

**binwalk results**: 411 signatures

**Detected patterns**: LZMA

*Relevant strings*:

-`D:\Project\G41T\MRC\mrc_091106\5331\OUT32\MEMINIT.pdb`
-`POSITIABACAXISWINDOWS`
-`AMIBIOSC0800Z`
-`AMIEBBLK`
-`iPnoee rDCR-MOA ATIP`
-`$$CT\x02BootBlock SIO Table`
-`IHATHC IDC-R7703`


## 🧩 6. Initial Structure (raw view)

**Distinct regions present**: Yes

**Possible areas identified**: MRC, BootBlock register table Header and code

**bootloader**: Reset vector located at `0x0FFFF0`, which redirects execution to BIOS initialization code

**code**: Intel x86 real-mode and possibly 32-bit protected-mode BIOS code

Offsets of interesting regions:

- `0x22C0 - 0xE93D0` - MRC
- `0xFFFE0 - 0xFFFF0`
- `0xF64C0 - 0xF7F90` - Suspected filesystem-related routines/real-mode x86

## 🧠 7. Initial Hypotheses

- The dumped firmware appears to be a valid legacy BIOS image containing structured x86 real-mode code and identifiable data regions. Based on similarities with official BIOS versions, it may be possible to map, compare, and partially reconstruct this version.
