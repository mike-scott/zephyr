set_ifndef(OPENSDA_FW jlink)

if(OPENSDA_FW STREQUAL jlink)
  set_ifndef(BOARD_DEBUG_RUNNER jlink)
elseif(OPENSDA_FW STREQUAL daplink)
  set_ifndef(BOARD_DEBUG_RUNNER pyocd)
  set_ifndef(BOARD_FLASH_RUNNER pyocd)
endif()

set(JLINK_DEVICE MKW41Z512xxx4)
set(PYOCD_TARGET kw41z4)

include($ENV{ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include($ENV{ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
