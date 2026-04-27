import sys

if  (not sys.argv[1] or not sys.argv[2]):
    print("Invalid command!")
    print("Usage: python3 log_cleaner.py INPUT OUTPUT")

log_pfile = open(f"{sys.argv[1]}", "r")
out_file = open(f"{sys.argv[2]}", "w")

kwords = ["CPU", "CMOS", "KBD", "UNMAP", "APIC", "DMA", "SYS"]

log_data = log_pfile.readlines()

for ln in log_data:
    for wr in kwords:
        if wr in ln:
            out_file.write(f"{ln}\n\n")

log_pfile.close()
out_file.close()