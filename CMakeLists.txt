cmake_minimum_required(VERSION 3.16)
project(SoulShift LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the user's SFML 2.5/2.6 installation.
if(NOT SFML_DIR OR NOT EXISTS "${SFML_DIR}/SFMLConfig.cmake")
    set(_sfml_candidate_dirs
        "$ENV{SFML_ROOT}"
        "C:/SFML-2.6.2"
        "C:/SFML-2.6.2-GCC14"
        "C:/SFML"
        "C:/Program Files/SFML"
        "C:/Program Files (x86)/SFML"
    )
    foreach(_sfml_candidate IN LISTS _sfml_candidate_dirs)
        if(_sfml_candidate AND EXISTS "${_sfml_candidate}/lib/cmake/SFML/SFMLConfig.cmake")
            set(SFML_DIR "${_sfml_candidate}/lib/cmake/SFML")
            break()
        elseif(_sfml_candidate AND EXISTS "${_sfml_candidate}/SFMLConfig.cmake")
            set(SFML_DIR "${_sfml_candidate}")
            break()
        endif()
    endforeach()
endif()

# This project uses the regular (DLL) SFML libraries.
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

add_executable(SoulShift main.cpp)
target_link_libraries(SoulShift PRIVATE sfml-graphics sfml-window sfml-system)

# Copy SFML's DLLs next to SoulShift.exe.
get_filename_component(SFML_ROOT "${SFML_DIR}/../../.." ABSOLUTE)
set(SOULSHIFT_RUNTIME_DLLS
    sfml-graphics-2.dll
    sfml-window-2.dll
    sfml-system-2.dll
    libgcc_s_seh-1.dll
    libstdc++-6.dll
    libwinpthread-1.dll
)
foreach(_dll IN LISTS SOULSHIFT_RUNTIME_DLLS)
    set(_runtime_dll "${SFML_ROOT}/bin/${_dll}")
    if(NOT EXISTS "${_runtime_dll}")
        get_filename_component(_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(_runtime_dll "${_compiler_dir}/${_dll}")
    endif()
    if(NOT EXISTS "${_runtime_dll}")
        set(_runtime_dll "${CMAKE_CURRENT_SOURCE_DIR}/${_dll}")
    endif()
    if(EXISTS "${_runtime_dll}")
        add_custom_command(TARGET SoulShift POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_runtime_dll}"
                "$<TARGET_FILE_DIR:SoulShift>"
        )
    endif()
endforeach()
