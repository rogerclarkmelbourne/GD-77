@echo off
copy ..\GD-77_firmware_stripper_workdir\*.raw . /y
firmware_find_shift GD-77_V2.5.6_1137_075801 datafile search_startpattern > results_search_startpattern.txt
firmware_find_shift GD-77_V2.6.1_6901_075801 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V2.6.3_2333_075801 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V2.6.6_0F10_075801 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V2.6.7_60F3_075801 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V2.6.8_2780_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V2.6.9_02DA_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.0.4_121E_078001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.0.6_0BF2_078001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.1.0_5D9C_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.1.1_73F3_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.1.2_4D78_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.1.3_61F7_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.1.5_frequency_offset_problem_fix_5EFE_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.1.6_5CE3_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.1.8_0CD1_077001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77_V3.2.1_1823_077801 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift GD-77S_V1.2.0_1A0C2_050001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift BF-5R_V2.0.9_1AE1B_078001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift BF-5R_V2.1.0_1BB37_078001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift BF-5R_V2.1.6_BB8A_078001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift MD-760_V2.6.5_7BFC_075801 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift DM-1801_V2.1.9_1FF08_078001 datafile search_startpattern >> results_search_startpattern.txt
firmware_find_shift BF-860_V3.1.9_FDBC4_078001 datafile search_startpattern >> results_search_startpattern.txt
pause
