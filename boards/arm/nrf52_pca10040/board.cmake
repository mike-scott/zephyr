set(BOARD_FLASH_RUNNER nrfjprog)

set(NRF_FAMILY NRF52)

set_property(GLOBAL APPEND PROPERTY FLASH_SCRIPT_ENV_VARS
  NRF_FAMILY
  )
