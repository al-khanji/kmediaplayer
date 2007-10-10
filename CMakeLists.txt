set(BIN_NAME kmediaplayer)
project(${BIN_NAME})

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include_directories(${KDE4_INCLUDES})
add_definitions(${KDE4_DEFINITIONS})

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall)
endif(CMAKE_COMPILER_IS_GNUCXX)

set(APP_SRC
    main.cpp
    mainwindow.cpp
)

kde4_add_ui_files(APP_SRC mainwindow.ui)

kde4_add_executable(${BIN_NAME} ${APP_SRC})
target_link_libraries(${BIN_NAME} kio phonon ${KDE4_KDEUI_LIBS})

install(TARGETS ${BIN_NAME} DESTINATION ${BIN_INSTALL_DIR})
install(FILES kmediaplayer.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})