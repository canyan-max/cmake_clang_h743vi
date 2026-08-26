# ---------- app ----------
set(APP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_main_task.c
)

add_library(app STATIC ${APP_SRC})
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
target_link_libraries(app
  PUBLIC
    service
  PRIVATE
    letter_shell
    FreeRTOS
)
