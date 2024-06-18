# basic cmake information
cmake_minimum_required(VERSION 3.28.0)
project(AndroidSystemSimplify
        VERSION 0.1.4
        LANGUAGES C)

# cmake definition include
include(GNUInstallDirs)

# make

# gcc args
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -mwindows)

# configure debug time file dependencies
configure_file($ENV{ANDROID_HOME}/platform-tools/adb.exe ${CMAKE_CURRENT_BINARY_DIR}/adb.exe COPYONLY)
file(COPY C:/msys64/ucrt64/share/themes/Fluent1 DESTINATION ${CMAKE_SOURCE_DIR}/share/themes)
file(COPY C:/msys64/ucrt64/share/themes/Fluent2 DESTINATION ${CMAKE_SOURCE_DIR}/share/themes)
file(COPY C:/msys64/ucrt64/etc/gtk-4.0 DESTINATION ${CMAKE_SOURCE_DIR}/etc)

# enable pkgconfig commands
find_package(PkgConfig REQUIRED)
include(FindPkgConfig)
# enable gtk library using pkgconfig
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk4)

# use glib-compile-resources with cmake
# step1
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
set(GRESOURCE_XML ${PROJECT_NAME}.gresource.xml)
set(GRESOURCE_C ${PROJECT_NAME}.gresource.c)
set(GRESOURCE_UI_IN ${PROJECT_NAME}.ui.in)
set(GRESOURCE_UI ${PROJECT_NAME}.ui)

# recompile gresource when ui file changed
set(NEW ${CMAKE_SOURCE_DIR}/resources/${GRESOURCE_UI})
set(OLD ${CMAKE_SOURCE_DIR}/resources/${GRESOURCE_UI_IN})
if(NOT EXISTS ${NEW} OR (${OLD} IS_NEWER_THAN ${NEW}))
        configure_file(${OLD} ${NEW})
        execute_process(
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${GLIB_COMPILE_RESOURCES} ${CMAKE_SOURCE_DIR}/resources/${GRESOURCE_XML} --target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C} --generate-source
        )
endif()
# step2
add_custom_command(
        OUTPUT ${GRESOURCE_C}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${GLIB_COMPILE_RESOURCES}
        ARGS 
        ${CMAKE_SOURCE_DIR}/resources/${GRESOURCE_XML} --target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C} --generate-source
        VERBATIM
        MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/resources/${GRESOURCE_XML}
)
# step3
add_custom_target(
        ${PROJECT_NAME}-gresource
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
)
# step4
set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
        PROPERTIES GENERATED TRUE
)

# prepare main executable src files
set(
        SRC_FILES
        ${CMAKE_SOURCE_DIR}/src/main.c
        ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
)

# prepare device_func library
add_library(
        ass_lib
        ${CMAKE_SOURCE_DIR}/include/program_control.h
        ${CMAKE_SOURCE_DIR}/src/program_control.c
        ${CMAKE_SOURCE_DIR}/include/device_func.h
        ${CMAKE_SOURCE_DIR}/src/device_func.c
        ${CMAKE_SOURCE_DIR}/include/app_func.h
        ${CMAKE_SOURCE_DIR}/src/app_func.c
        )
target_include_directories(# add include path to search for header files
        ass_lib
        PUBLIC 
                ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(# device_func lib also need gtk lib
        ass_lib
        PkgConfig::GTK
)

# main executable
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(# link gtk lib device_func lib
        ${PROJECT_NAME}
        PkgConfig::GTK
        ass_lib)

# step5
add_dependencies(${PROJECT_NAME} ${CMAKE_PROJECT_NAME}-gresource)

# test
enable_testing()
include(CTest)

# pack
# install main executable
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR})


# install third-party libraries
include(InstallRequiredSystemLibraries)
# run /scripts/copy...sh before install
set(${PROJECT_SOURCE_DIR} .)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/install/bin/
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/install/lib/
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

# install theme and icons
install(DIRECTORY ${PROJECT_SOURCE_DIR}/install/share/
        DESTINATION ${CMAKE_INSTALL_DATADIR})
configure_file(${CMAKE_SOURCE_DIR}/etc/bloatware_data.txt ${CMAKE_INSTALL_SYSCONFDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/install/etc/
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR})

# cpack project info
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# cpack package info
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")

# cpack generator info
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "ZIP")

# set archive generator var
set(CPACK_ARCHIVE_FILE_NAME "安卓系统精简器-${CPACK_PROJECT_VERSION}")

# load and RUN cpack
include(CPack)