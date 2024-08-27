       
       
       
       
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
__MSP_STACK_SIZE = (4 << 10);
__MIN_HEAP_SIZE = (90 << 10);
__MPU_PROTECT_SIZE = 0x0;
__SWAP_SIZE = (2048);
__CPU0_APP_RAM_BASE = (0x28000000 + (0));
__TOTAL_RAM_SIZE = 0xA0000;
__CPU0_APP_RAM_SIZE = 0xA0000;
__CPU0_APP_VIRTUAL_CODE_START = 0x10000;
__CPU0_APP_VIRTUAL_CODE_SIZE = 0x220000;
__CPU0_APP_IRAM_OFFSET = 0x20000000;
__CPU0_APP_IRAM_BASE = __CPU0_APP_RAM_BASE - __CPU0_APP_IRAM_OFFSET;
__CPU0_APP_IRAM_SIZE = __CPU0_APP_RAM_SIZE;
MEMORY
{
  FLASH (rx) : ORIGIN = ((0x02000000 + (0)) + __CPU0_APP_VIRTUAL_CODE_START), LENGTH = __CPU0_APP_VIRTUAL_CODE_SIZE
  IRAM (rx) : ORIGIN = __CPU0_APP_IRAM_BASE, LENGTH = __CPU0_APP_IRAM_SIZE
  RAM (rwx) : ORIGIN = __CPU0_APP_RAM_BASE, LENGTH = __TOTAL_RAM_SIZE - __SWAP_SIZE
  SWAP (rwx) : ORIGIN = __CPU0_APP_RAM_BASE + __TOTAL_RAM_SIZE - __SWAP_SIZE, LENGTH = __SWAP_SIZE
  ITCM (rwx) : ORIGIN = (0x00000000 + (0)) + __MPU_PROTECT_SIZE, LENGTH = 0x4000 - __MPU_PROTECT_SIZE
  DTCM (rwx) : ORIGIN = (0x20000000 + (0)) + 0, LENGTH = 0x3FE0
  PSRAM (rwx) : ORIGIN = (0x60000000 + (0)), LENGTH = 0x4000000
}
ENTRY(Reset_Handler)
SECTIONS
{
    ASSERT((. == ALIGN(512)), "vector table address align fault.")
    .vectors :
    {
        __vector_table = .;
        KEEP(*(.vectors))
        . = ALIGN(512);
    } > FLASH
    .gnu.sgstubs ALIGN(32) : ALIGN(32)
    {
        *(.gnu.sgstubs*)
        . = ALIGN(32);
    } > FLASH
    .text :
    {
        . = ALIGN(4);
        _stext = .;
 EXCLUDE_FILE
  (
  *tasks.c.obj *queue.c.obj *list.c.obj *timers.c.obj *port.c.obj *rtos_pub.c.obj *heap_4.c.obj *mem_arch.c.obj *gpio_driver_base.c.obj *flash_driver.c.obj *ckmn_driver.c.obj *arch_interrupt.c.obj *hal_machw.c.obj *interrupts.c.obj
  *rw_task.c.obj *me_strategy.c.obj *chan.c.obj *vif_mgmt.c.obj *rxl_cntrl.c.obj *rxl_hwdesc.c.obj *mm_timer.c.obj *ke_event.c.obj *rwnx.c.obj *rwnx_rx.c.obj *ps.c.obj *mm.c.obj *rf_cntrl.c.obj *mcc.c.obj
  *phy_karst_bk7236.c.obj *crm.c.obj *bk7236_cal.c.obj *manual_cal_bk7236.c.obj *bk_cal_common.c.obj *nv_parameters.c.obj *macif_fhost.c.obj *rc_driver.c.obj *sys_wifi_driver.c.obj *wifi_v2.c.obj *bk_feature.c.obj
  *pm.c.obj *sys_ps_driver.c.obj *sys_pm_hal.c.obj *bk_wifi_adapter.c.obj *net_param.c.obj *wifi_api_wrapper.c.obj *bk_rf_adapter.c.obj *bk_phy_adapter.c.obj
  *shell_task.c.obj *shell_uart.c.obj *uart_driver.c.obj *uart_hal.c.obj
  )
 *(.text.* *.rodata *.rodata.*)
        . = ALIGN(4);
        __devconfig_start = .;
        *(".devconfig.*")
        KEEP(*(SORT_BY_NAME(".devconfig*")))
        __devconfig_end = .;
        . = ALIGN(4);
        __apps_start = .;
        KEEP (*(.apps_data))
        __apps_end = .;
        _etext = .;
        . = ALIGN(4);
    } > FLASH
    .a_device_null :
    {
      KEEP(*(.a_deviceobj_null))
    } > FLASH
    .a_devices :
    {
      __device_start = .;
      KEEP(*(.a_deviceobj_*))
      __device_end = .;
    } > FLASH
    .a_init_entries :
    {
      __a_init_start = .;
      KEEP(*(.a_init_entry_*))
      __a_init_end = .;
    } > FLASH
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > FLASH
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__iram_flash_begin)
        LONG (__iram_start__)
        LONG ((__iram_end__ - __iram_start__) / 4)
        LONG (__data_flash_begin)
        LONG (__data_start__)
        LONG ((__data_end__ - __data_start__) / 4)
        LONG (__video_cache_text)
        LONG (__video_cache_data_start__)
        LONG ((__video_cache_data_end__ - __video_cache_data_start__) / 4)
        LONG (__itcm_text)
        LONG (__itcm_start__)
        LONG ((__itcm_end__ - __itcm_start__) / 4)
        LONG (__dtcm_content)
        LONG (__dtcm_start__)
        LONG ((__dtcm_end__ - __dtcm_start__) / 4)
        __copy_table_end__ = .;
    } > FLASH
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (_bss_start)
        LONG ((_bss_end - _bss_start) / 4)
        LONG (_heap_start)
        LONG ((_heap_end - _heap_start) / 4)
        LONG (__video_cache_bss_start__)
        LONG ((__video_cache_bss_end__ - __video_cache_bss_start__) / 4)
        __zero_table_end__ = .;
    } > FLASH
    .itcm :
    {
        . = ALIGN(4);
        PROVIDE(__itcm_text = LOADADDR(.itcm));
        __itcm_start__ = .;
        KEEP(*(.null_trap_handler))
        *(.itcm_section*)
        *(.itcm_sec_code*)
        . = ALIGN(4);
        __itcm_end__ = .;
    } > ITCM AT > FLASH
    .dtcm :
    {
        . = ALIGN(4);
        PROVIDE(__dtcm_content = LOADADDR(.dtcm));
        __dtcm_start__ = .;
        *(.dtcm)
        *(.dtcm_section*)
        *(.dtcm_sec_data*)
        __dtcm_end__ = .;
    } > DTCM AT > FLASH
    .iram :
    {
        . = ALIGN(512);
        PROVIDE(__iram_flash_begin = LOADADDR(.iram));
        __iram_start__ = .;
        . = ALIGN(512);
        __vector_iram_table = .;
        KEEP(*(.vectors_iram))
        . = ALIGN(512);
        *(.itcm)
        *(.iram)
        *(.iram.*)
        *(.interrupt)
        *tasks.c.obj(.text.* *.rodata *.rodata.*)
        *queue.c.obj(.text.* *.rodata *.rodata.*)
        *list.c.obj(.text.* *.rodata *.rodata.*)
        *timers.c.obj(.text.* *.rodata *.rodata.*)
        *port.c.obj(.text.* *.rodata *.rodata.*)
        *rtos_pub.c.obj(.text.* *.rodata *.rodata.*)
        *heap_4.c.obj(.text.* *.rodata *.rodata.*)
        *mem_arch.c.obj(.text.* *.rodata *.rodata.*)
        *gpio_driver_base.c.obj(.text.* *.rodata *.rodata.*)
        *flash_driver.c.obj(.text.* *.rodata *.rodata.*)
        *ckmn_driver.c.obj(.text.* *.rodata *.rodata.*)
        *arch_interrupt.c.obj(.text.* *.rodata *.rodata.*)
        *hal_machw.c.obj(.text.* *.rodata *.rodata.*)
        *interrupts.c.obj(.text.* *.rodata *.rodata.*)
        *rw_task.c.obj(.text.* *.rodata *.rodata.*)
        *ke_event.c.obj(.text.* *.rodata *.rodata.*)
        *me_strategy.c.obj(.text.* *.rodata *.rodata.*)
        *chan.c.obj(.text.* *.rodata *.rodata.*)
        *vif_mgmt.c.obj(.text.* *.rodata *.rodata.*)
        *rxl_cntrl.c.obj(.text.* *.rodata *.rodata.*)
        *rxl_hwdesc.c.obj(.text.* *.rodata *.rodata.*)
        *mm_time.c.obj(.text.* *.rodata *.rodata.*)
        *rwnx.c.obj(.text.* *.rodata *.rodata.*)
        *rwnx_rx.c.obj(.text.* *.rodata *.rodata.*)
        *ps.c.obj(.text.* *.rodata *.rodata.*)
        *mm.c.obj(.text.* *.rodata *.rodata.*)
        *rf_cntrl.c.obj(.text.* *.rodata *.rodata.*)
        *mcc.c.obj(.text.* *.rodata *.rodata.*)
        *phy_karst_bk7236.c.obj(.text.* *.rodata *.rodata.*)
        *crm.c.obj (.text.* *.rodata *.rodata.*)
        *bk7236_cal.c.obj(.text.* *.rodata *.rodata.*)
        *manual_cal_bk7236.c.obj(.text.* *.rodata *.rodata.*)
        *bk_cal_common.c.obj(.text.* *.rodata *.rodata.*)
        *nv_parameters.c.obj(.text.* *.rodata *.rodata.*)
        *macif_fhost.c.obj(.text.* *.rodata *.rodata.*)
        *rc_driver.c.obj(.text.* *.rodata *.rodata.*)
        *sys_wifi_driver.c.obj(.text.* *.rodata *.rodata.*)
        *wifi_v2.c.obj(.text.* *.rodata *.rodata.*)
        *bk_feature.c.obj(.text.* *.rodata *.rodata.*)
        *pm.c.obj(.text.* *.rodata *.rodata.*)
        *sys_ps_driver.c.obj(.text.* *.rodata *.rodata.*)
        *sys_pm_hal.c.obj(.text.* *.rodata *.rodata.*)
        *bk_wifi_adapter.c.obj(.text.* *.rodata *.rodata.*)
        *net_param.c.obj(.text.* *.rodata *.rodata.*)
        *wifi_api_wrapper.c.obj(.text.* *.rodata *.rodata.*)
        *bk_rf_adapter.c.obj(.text.* *.rodata *.rodata.*)
        *bk_phy_adapter.c.obj(.text.* *.rodata *.rodata.*)
        *shell_task.c.obj(.text.* *.rodata *.rodata.*)
        *shell_uart.c.obj(.text.* *.rodata *.rodata.*)
        *uart_driver.c.obj(.text.* *.rodata *.rodata.*)
        *uart_hal.c.obj(.text.* *.rodata *.rodata.*)
        __iram_end__ = .;
    } > IRAM AT > FLASH
    .ramcode :
    {
        . = . + SIZEOF(.iram);
        . = ALIGN(512);
    } > RAM AT > FLASH
    .data :
    {
        . = ALIGN(512);
        PROVIDE(__etext = LOADADDR(.data));
        PROVIDE(__data_flash_begin = LOADADDR(.data));
        . = ALIGN(4);
        __data_start__ = .;
        *(.data)
        *(".data.*")
        *(.sdata)
        *(*ts2.data *ts2.data.*)
     *(*ts2.sdata )
        *(.video_spec_data*)
        *(.gnu.linkonce.d*)
        __data_end__ = .;
    } > RAM AT > FLASH
    _data_flash_begin = __data_flash_begin;
    _data_ram_begin = __data_start__;
    _data_ram_end = .;
    s_rom_end = LOADADDR(.data) + SIZEOF(.data);
    .uninitialized (NOLOAD):
    {
        . = ALIGN(32);
        __uninitialized_start = .;
        *(.uninitialized)
        *(".uninitialized.*")
        KEEP(*(.keep.uninitialized))
        . = ALIGN(32);
        __uninitialized_end = .;
    } > RAM
    .bss (NOLOAD):
    {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _nocache_start = .;
        *(.sram_nocache*)
        _nocache_end = .;
        . = ALIGN(4);
        _bss_end = .;
    } > RAM
    bss_size = _bss_end - _bss_start;
 _msp_total_size = __MSP_STACK_SIZE;
    .heap (COPY) :
    {
        . = ALIGN(8);
        _heap_start = .;
        . = . + (ORIGIN(RAM) + LENGTH(RAM) - _msp_total_size - _heap_start - 8);
        . = ALIGN(8);
        _heap_end = .;
    } > RAM
    .stack (ORIGIN(RAM) + LENGTH(RAM) - __MSP_STACK_SIZE) (COPY) :
    {
        . = ALIGN(8);
        _sstack = .;
        __StackLimit = .;
        . = . + __MSP_STACK_SIZE;
        . = ALIGN(8);
        __StackTop = .;
        _estack = .;
    } > RAM
    .swap ORIGIN(SWAP):
    {
        . = ALIGN(8);
        _swap_start = .;
        * (.swap_data)
        * (.swap_data*)
        _swap_end = .;
    } > SWAP AT > FLASH
    .video.cache.data :
    {
        . = ALIGN(4);
        PROVIDE(__video_cache_text = LOADADDR(.video.cache.data));
        __video_cache_data_start__ = .;
        *(.video_cache_data)
        *(.video_cache_data*)
        . = ALIGN(4);
        __video_cache_data_end__ = .;
    } > PSRAM AT > FLASH
    .video_cache_bss (NOLOAD):
    {
        . = ALIGN(4);
        __video_cache_bss_start__ = .;
        *(.video_cache_bss)
        *(.video_cache_bss*)
        . = ALIGN(4);
        __video_cache_bss_end__ = .;
    } > PSRAM
 . = __exidx_end + SIZEOF(.data) + 0x40000;
 . = ALIGN(0x8);
    .ty_text_section ALIGN(0x10000) :
    {
        _ty_section_start = .;
  PROVIDE_HIDDEN (__init_array_start = .);
  KEEP (*(SORT(*.init_array.*)))
  KEEP (*(*.init_array*))
  PROVIDE_HIDDEN (__init_array_end = .);
  _ty_section_caa_end = .;
  *libCHIP.a:*(.text .text.*)
  *(*ts2.text *ts2.text.*)
  _ty_section_text_end = .;
  *libCHIP.a:*(.rodata .rodata.*)
  *(*ts2.rodata *ts2.rodata.*)
  _ty_section_end = .;
    } >FLASH
 ty_section_size = SIZEOF(.ty_text_section);
    __MSPTop = ORIGIN(RAM) + LENGTH(RAM);
    __MSPLimit = __MSPTop - __MSP_STACK_SIZE;
    ASSERT((s_rom_end < ORIGIN(FLASH) + LENGTH(FLASH)), "ROM overflow!!!")
    ASSERT(((ORIGIN(RAM) + LENGTH(RAM)) > (_heap_start + __MSP_STACK_SIZE + 8)), "Stack overflowed with bss")
    ASSERT(((_heap_end - _heap_start) >= __MIN_HEAP_SIZE), "Heap smaller than minimize size 90K!!!")
}
