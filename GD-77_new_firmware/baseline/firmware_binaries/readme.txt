„ready for flashing“ versions of the current firmware variants that are compatible with the original firmware updater. The PC testtool is in https://github.com/talentraspel/GD-77/tree/master/GD-77_new_firmware/TestTool_Binary.

basic_firmware_without_testmode.sgl:
  - Timed splash screen at power on.
  - Timed shutdown screen at power off.
  - Power off gets interrupted by just turning power on again.
  - Display light stays on for 4 seconds when something happens on the display.
  - Multilevel menu system with example structure. Enter/Exit Menu with SK2, navigate menu with up/down/left/right.
  - COM port (compatible to existing Windows 10 driver) gets detected when programming cable is attached to PC.

basic_firmware_with_testmode.sgl:
  - Everything from „basic_firmware_without_testmode.sgl“.
  - Manual Testmode. Enter with SK1, exit with 2 seconds SK1.
    - Pressed keys and buttons are showed in the display.
    - Green and red LED are coupled to green and red key.
    - Holding PTT activates a display test.
  - PC testmode active when beeing in manual testmode.
    - Testtool can connect to COM port and performs a fast/simple live data exchange protocol.
    - Pressed keys and buttons are showed in the testtool.
    - Testtool allows to toggle LEDs.
    - Testtool allows to toggle display light and activate a test message.

The current versions have no useful functionality beside tests/examples and are for stabilizing this project stage.
