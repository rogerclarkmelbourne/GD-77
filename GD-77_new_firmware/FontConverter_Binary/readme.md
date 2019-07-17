Quick and dirty bitmap font converter for use with the binary font files created by RaysFontEditor.exe

RaysFontEditor can be currently downloaded from here

http://www.rayslogic.com/Software/RaysFontEditor/RaysFontEditor_24Aug12.zip

The converter currently only supports font files which are 8 pixels wide and 16 pixels tall

In RaysFontEditor, use the Capture font, to read in a font you have installed. Make sure the size is set to 8 x 16

Then export the font, and select format Binary File. Start Top-Left, Scan Horizontal

Only output the character range from 32 to 127, as other ranges may not be correctly supported by the OpenGD77 UC1701 screen driver.

Drag and drop the binary file onto the exe, and it should generate a .h (header) file, which can then be used with the OpenGD77 firmware
By adding it to /include/display/UC1701_charset.h and modifying /source/display/UC1701.c to use the font.

