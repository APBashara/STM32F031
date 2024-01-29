/* define end of ram and linit stack memory */
/* 4KB RAM on STM32F031K6 */
/* SRAM starts at 0x2000 0000 */
/* pg 51 of RM0091 */

_estack = 0x20001000;

MEMORY
{
    FLASH ( rx )      : ORIGIN = 0x08000000, LENGTH = 32K
    RAM ( rxw )       : ORIGIN = 0x20000000, LENGTH = 4K
}