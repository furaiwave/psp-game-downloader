cmake_minimum_required(VERSION 3.10)
project(PSPLoader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg-master/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

#  Библиотеки 

find_package(Crow CONFIG REQUIRED)

message(STATUS "PSP Loader Configuration")
message(STATUS "Crow found: ${Crow_FOUND}")
message(STATUS "Using WinUSB (Windows native)")

#  Исходники 

set(SOURCES
    # Main
    src/main.cpp
    
    # HTTP
    src/http/Server.cpp
    
    # Controllers
    src/controllers/PSPController.cpp
    src/controllers/UploadController.cpp
    
    # PSP
    src/psp/PSPDevice.cpp
    src/psp/WinUSBConnection.cpp
    src/psp/FileTransfer.cpp
    
    # Services
    src/services/TransferService.cpp
    
    # Utils
    src/utils/Logger.cpp
    src/utils/Config.cpp
)

set(HEADERS
    # HTTP
    src/http/Server.h
    
    # Controllers
    src/controllers/PSPController.h
    src/controllers/UploadController.h
    
    # PSP
    src/psp/PSPDevice.h
    src/psp/WinUSBConnection.h
    src/psp/FileTransfer.h
    
    # Services
    src/services/TransferService.h
    
    # Utils
    src/utils/Logger.h
    src/utils/Config.h
)

#  Создание программы 

add_executable(psp_loader ${SOURCES} ${HEADERS})

target_include_directories(psp_loader PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
)

# Линковка: Crow + WinUSB
target_link_libraries(psp_loader PRIVATE 
    Crow::Crow
    setupapi.lib    # Для SetupDiGetClassDevs
    winusb.lib      # WinUSB API
)

#  Windows 

if(WIN32)
    # Копируем DLL
    add_custom_command(TARGET psp_loader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:psp_loader>
        $<TARGET_FILE_DIR:psp_loader>
        COMMAND_EXPAND_LISTS
    )
    
    # Создаём temp и logs директории
    add_custom_command(TARGET psp_loader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:psp_loader>/temp
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:psp_loader>/logs
    )
    
    # Копируем config.json
    add_custom_command(TARGET psp_loader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_SOURCE_DIR}/config.json
        $<TARGET_FILE_DIR:psp_loader>/config.json
    )
endif()

if(MSVC)
    target_compile_options(psp_loader PRIVATE /W3)
    target_compile_definitions(psp_loader PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
    )
endif()