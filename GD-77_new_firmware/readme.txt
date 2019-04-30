Disclaimer: Do not use for anything right now! The current state is
            highly experimental only.

The MCUXpresso project will become a new GD-77 firmware, written from
scratch and beeing 100% open source using 100% free tools.

The MCUXpresso project produces a full flash image replacement (starting
at 0x00000000 without a bootloader) that is intended for getting flashed
from within the IDE. Later on standalone firmware files for use with the
regular GD-77 updater can get created.

------------

firmware:
  Most current development version.

basic_firmware_with_testmode:
  Baseline with testmode.

basic_firmware_without_testmode
  Baseline without testmode.

TestTool:
  Tool to communicate with the firmware using an USB COM port.

TestTool_Binary:
  Precompiled executable for TestTool.
