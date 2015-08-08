@echo on
PUSHD %~DP0
set OPENOCD_DIR=C:\Program Files\GNU ARM Eclipse\OpenOCD\0.9.0-201505190955\
set OPENOCD="%OPENOCD_DIR%bin\openocd"
set OPENOCD_INTERFACE="%OPENOCD_DIR%scripts\interface\stlink-v2.cfg"
set OPENOCD_TARGET="%OPENOCD_DIR%scripts\target\stm32f4x_stlink.cfg"

@echo on


@echo STM32 FIRM Start
%OPENOCD% -d2 -f %OPENOCD_INTERFACE% -c "transport select hla_swd" -f %OPENOCD_TARGET% -c init -c targets -c "reset halt" -c "flash write_image erase cf2.hex" -c "verify_image cf2.hex" -c "reset run" -c shutdown
@echo STM32 FIRM End
	               
pause