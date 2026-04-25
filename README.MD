# AMIcore

```txt
   db    8b    d8 88  dP""b8  dP"Yb  88""Yb 888888 
  dPYb   88b  d88 88 dP   `" dP   Yb 88__dP 88__   
 dP__Yb  88YbdP88 88 Yb      Yb   dP 88"Yb  88""   
dP""""Yb 88 YY 88 88  YboodP  YbodP  88  Yb 888888 
```

![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![Framework](https://img.shields.io/badge/framework-ESP--IDF-red)
![Security](https://img.shields.io/badge/focus-Reverse%20Engineering-lightgrey)

---

## 📌 Project Overview

This project focuses on extracting and analyzing SPI flash memory from a motherboard using an ESP32-based reader.

The goal is to obtain raw firmware data, study its internal structure, and identify patterns such as headers, repeating regions, and possible firmware components. The project combines low-level hardware interfacing with data analysis techniques to better understand how embedded systems store and organize information.

![Hardware Setup](docs/images/protoboard.jpeg)
*Figure 1: ESP32 Hardware interface for Out-of-Band SPI extraction.*

---

## 🧠 Technical Deep Dive

### Hardware Interfacing

The system utilizes the **ESP-IDF SPI Master driver** to communicate with Flash chips. We obtain an unaltered bit-for-bit image of the firmware.

### Structural Discovery

Our analysis of the raw dump revealed several critical artifacts:

* **Legacy Headers:** Detected `AMIBIOS`, `AMIJPG`, and `AMIBOOT` signatures, identifying the vendor as American Megatrends.
* **Executable Recovery:** Identified and extracted a valid **PE (Portable Executable)** file embedded within the ROM.
* **Signature Logic:** Discovered heuristic code that scans for the `AMIBIOS` signature and implements a 2048-bit (256-byte) pointer decrement upon a match, likely used for memory alignment.

---

## 📊 Analysis Pipeline

1. **Phase 0 - Extraction:** ESP32 dumps SPI Flash via bit-banging or hardware SPI.
2. **Phase 1 - Processing:** Binary data is processed with tools: ImHex, Binwalk, xxd, dd.
3. **Phase 2 - Analyzing:** Data is divided by regions and analyzed with Binwalk, vbindiff, ImHex, strings, dd.
4. **Phase 3 - Emulation:** Bios is emulated with Bochs and log file is compared with official version
5. **Phase 3 - Reverse Engineering:** Code analysis using **Cutter** to map initialization vectors and jump tables.

![Heuristic Pattern Discovery](docs/images/directory_find_hex.jpg)
*Figure 2: Identifying memory offsets and signature patterns in ImHex.*

---

## 🛠️ Tech Stack

* **Firmware:** C (ESP-IDF) for high-performance SPI throughput.
* **Analysis Tools:**
  * **ImHex:** For hexadecimal structural visualization and pattern tagging.
  * **Cutter/Ghidra:** For x86 (16/32-bit) disassembly and decompilation.
  * **strings:** For extracting human-readable strings and identifying embedded metadata, version info and debug paths.
  * **dd:** For binary slicing — extracting specific regions by offset and size for isolated analysis.
  * **binwalk:** For entropy analysis, signature scanning and recursive extraction of embedded modules.
  * **xxd:** For raw hexadecimal inspection and manual byte-level analysis.
  * **vbindiff:** For visual side-by-side binary comparison between the dumped and official firmware.
  * **Bochs:** For x86 BIOS emulation, allowing behavioral comparison between firmware versions without physical hardware.

---

## 🔍 Key Findings

> [!IMPORTANT]
> **Signature-Based Navigation:** The firmware contains a specific routine that validates the `AMIBIOS` string. If found, the system performs a memory-offset adjustment (decrementing 2048 bits). This suggests the firmware uses a fixed-offset look-up table based on signature discovery rather than a standard file system.

![Signature check](/docs/images/check_signature.jpg)
*Figure 3: Signature check code in Cutter.*

---

> [!IMPORTANT]
> **MRC Region Identified:** A Memory Reference Code (MRC) module was identified at offset `0xE93D0`, containing the DDR memory initialization routines for the Intel G41 chipset. The module includes timing tables, frequency configuration and a PDB debug path confirming its origin: `D:\Project\G41T\MRC\mrc_091106\5331\OUT32\MEMINIT.pdb` (compiled November 2009, build 5331).

---

> [!IMPORTANT]
> **BootBlock Structure Identified:** The firmware contains two BootBlock instances — a primary and a secondary (newer version) — both signed with `AMIBOOT ROM` and `AMIBIOSC` headers. Each BootBlock includes the following embedded drivers:
>
> - **POST Code:** x86 real mode initialization routines, CPU MSR configuration and CMOS access
> - **IDE/ATA Driver:** Identified by string `IHATHC IDC-R7703` and `0080 a`
> - **CD-ROM Driver:** Identified by ISO 9660 signature `CD001`
> - **USB Driver:** Identified by signatures `USBC` (Command Block) and `USBS` (Status Block)
> - **Super I/O Table:** `$$CT BootBlock SIO Table` — hardware register initialization table for the Super I/O chip

## 🚀 Future Roadmap

-[ ] Analyze firmware data regions and microcodes.
-[ ] Emulate dumped bios and official bios to compare.

---

## ⚠️ Disclaimer

This project is strictly for educational purposes and security research. Unauthorized access to hardware or firmware may void warranties or violate terms of service.

---
