cmake_minimum_required(VERSION 3.24)
project(cttimer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Multimedia DBus)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(cttimer
    src/main.cpp
    src/DurationParser.cpp
    src/DurationParser.h
    src/SettingsManager.cpp
    src/SettingsManager.h
    src/TimerController.cpp
    src/TimerController.h
    src/resources.qrc
)

target_link_libraries(cttimer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
    Qt6::DBus
)

target_compile_definitions(cttimer PRIVATE
    CTTIMER_VERSION="${PROJECT_VERSION}"
)

# Freesound requires authentication to download the selected CC0 original.
# If the user places notify4.wav here, embed it directly into the executable.
set(CTTIMER_NOTIFY4 "${CMAKE_CURRENT_SOURCE_DIR}/assets/sounds/notify4.wav")
if(EXISTS "${CTTIMER_NOTIFY4}")
    qt_add_resources(cttimer "cttimer_audio"
        PREFIX "/sounds"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}/assets/sounds"
        FILES "${CTTIMER_NOTIFY4}"
    )
    message(STATUS "cttimer: embedding assets/sounds/notify4.wav")
else()
    message(WARNING "cttimer: assets/sounds/notify4.wav is absent; default alarm audio will be unavailable")
endif()

install(TARGETS cttimer RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES data/io.controlthings.cttimer.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES data/io.controlthings.cttimer.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
install(FILES assets/icons/io.controlthings.cttimer.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

option(CTTIMER_BUILD_TESTS "Build cttimer tests" ON)
if(CTTIMER_BUILD_TESTS)
    enable_testing()
    find_package(Qt6 6.8 QUIET COMPONENTS Test)
    if(TARGET Qt6::Test)
        qt_add_executable(test_duration_parser
            tests/test_duration_parser.cpp
            src/DurationParser.cpp
            src/DurationParser.h
        )
        target_link_libraries(test_duration_parser PRIVATE Qt6::Core Qt6::Test)
        target_include_directories(test_duration_parser PRIVATE src)
        add_test(NAME duration_parser COMMAND test_duration_parser)
    endif()
endif()
