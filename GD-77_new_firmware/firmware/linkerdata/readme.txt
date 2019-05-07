The codec gets reused by just keeping the original content of the V3.1.1 firmware. The files fw_311_0x00004400_0x00007fff.dat and fw_311_0x00050000_0x0007afff.dat get placed within the flash by the linker and must contain original decrypted areas of the FW V3.1.1 firmware.

To avoid copyright issues the files in this repository contain only dummy data. Before bulding the correct data has to get manually copied in from a decrypted FW V3.1.1 file.

Copy the following parts:
- Copy 0x00000400 to 0x00003fff from a decrypted FW V3.1.1 file into fw_311_0x00004400_0x00007fff.dat.
- Copy 0x0004c000 to 0x00076fff from a decrypted FW V3.1.1 file into fw_311_0x00050000_0x0007afff.dat.
