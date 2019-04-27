@echo off
copy ..\Radioddity_GD-77\firmware\*.sgl . /y
copy ..\Radioddity_GD-77S\*.sgl . /y
copy ..\Baofeng_RD-5R\*.sgl . /y
copy ..\MD-760\*.sgl . /y
copy ..\DM-1801\*.sgl . /y
copy ..\DM-860\*.sgl . /y
firmware_stripper GD-77_V2.5.6
firmware_stripper GD-77_V2.6.1
firmware_stripper GD-77_V2.6.3
firmware_stripper GD-77_V2.6.6
firmware_stripper GD-77_V2.6.7
firmware_stripper GD-77_V2.6.8
firmware_stripper GD-77_V2.6.9
firmware_stripper GD-77_V3.0.4
firmware_stripper GD-77_V3.0.6
firmware_stripper GD-77_V3.1.0
firmware_stripper GD-77_V3.1.1
firmware_stripper GD-77_V3.1.2
firmware_stripper GD-77_V3.1.3
firmware_stripper GD-77_V3.1.5_frequency_offset_problem_fix
firmware_stripper GD-77_V3.1.6
firmware_stripper GD-77_V3.1.8
firmware_stripper GD-77_V3.2.1
firmware_stripper GD-77S_V1.2.0
firmware_stripper BF-5R_V2.0.9
firmware_stripper BF-5R_V2.1.0
firmware_stripper BF-5R_V2.1.6
firmware_stripper MD-760_V2.6.5
firmware_stripper DM-1801_V2.1.9
firmware_stripper BF-860_V3.1.9
pause
