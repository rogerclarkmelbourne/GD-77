# GD-77, GD-77S, RD-5R, MD-760, DM-1801 and DM-860
GD-77, GD-77S, RD-5R, MD-760, DM-1801 and DM-860 reverse engineering

# Details
"Radioddity_GD-77" contains the original encrypted GD-77 firmware files collected by Roger Clark (https://github.com/rogerclarkmelbourne/Radioddity_GD-77.git) as they also are or have been available on the manufacturers web site.

"Radioddity_GD-77S" contains original encrypted GD-77S firmware files as they also are or have been available on the manufacturers web site.

"Baofeng_RD-5R" contains original encrypted RD-5R firmware files as they also are or have been available on the manufacturers web site.

"MD-760" contains original encrypted MD-760 firmware files as they also are or have been available on the manufacturers web site.

"DM-1801" contains original encrypted DM-1801 firmware files as they also are or have been available on the manufacturers web site.

"DM-860" contains original encrypted DM-860 firmware files as they also are or have been available on the manufacturers web site.

"GD-77_firmware_stripper" contains the source for the firmware stripping tool. It removes the header and seperates the raw encrytped firmware data as it is transfered by the uploader.

"GD-77_firmware_stripper_workdir" contains the executable and the batch. Run it to receive the results.

"GD-77_firmware_find_shift" searches for the firmware file decryption shift values.

"GD-77_firmware_find_shift_workdir" contains the executable and the batch. Run it to receive the results.

"GD-77_firmware_decrypt_encrypt" contains the source for the firmware decrypt/encrypt tool. It uses a keyfile and a sequence entry point for XORing and applies an additional XOR/negate/rotate step and with that decrypts or encrypts a firmware file.

"GD-77_firmware_decrypt_encrypt_workdir" contains the executable and the batch. Run it to receive the results.
 
"GD-77_recreate_uploadable_firmware" contains the toolchain to recreate an uploadable firmware file. It takes a decrypted (and maybe patched) firmware file, encrypts it again and copyies the original header in front. The result is an uploadable firmware file.

"GD-77_flash_readout" contains patched firmware and tools to get a flush flash readout.

"GD-77_cpu-id_calc" contains tools to calculate the data necessary to patch "full_flash.dat" so that the bootloader and the main application survive their CPU-UID checks.

"GD-77_MCUXpresso" contains the MCUXpresso projects.

# Experimental only
"GD-77_new_firmware" contains the MCUXpresso project and the tools that will lead to a new GD-77 firmware written from scratch.
