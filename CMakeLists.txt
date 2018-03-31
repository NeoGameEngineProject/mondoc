PROJECT(MONDOC)

## Let's use a reasonable modern version
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
SET(CMAKE_HAS_SORT 1)
SET(CMAKE_MODULE_PATH
  ${MONDOC_SOURCE_DIR}/CMake
  ${CMAKE_MODULE_PATH}
)

INCLUDE("CopyFiles")

## Find source and headers
FILE(GLOB_RECURSE HEADERS src/*.h)
FILE(GLOB_RECURSE SOURCES src/*.c*)

IF(CMAKE_HAS_SORT)
  LIST(SORT HEADERS)
  LIST(SORT SOURCES)
ENDIF(CMAKE_HAS_SORT)

ADD_EXECUTABLE(mondoc ${SOURCES} ${HEADERS})

INSTALL(TARGETS mondoc DESTINATION bin)
INSTALL(FILES style.css navigation.js highlight.pack.js DESTINATION share/mondoc)
#INSTALL(FILES style.css syntax-style.css lunr.js highlight.pack.js searchprovider.js DESTINATION share/mondoc)
# INSTALL(DIRECTORY styles DESTINATION share/mondoc)

SET(CPACK_PACKAGE_NAME "mondoc")
SET(CPACK_PACKAGE_VENDOR "Yannick Pflanzer")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Mondoc, a Lua documentation tool")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/Data/neo-icon.png")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY /usr)

IF(WIN32)
SET(CPACK_GENERATOR "NSIS;ZIP")
ENDIF()

IF(UNIX AND NOT APPLE)
  SET(CPACK_PACKAGE_CONTACT Yannick Pflanzer <yannick.pflanzer@scary-squid.de>)
  SET(CPACK_GENERATOR "STGZ;TGZ;DEB;RPM;")
  SET(CPACK_STRIP_FILES ON)

  IF(BUILD_PACKAGE)
	MESSAGE("-- Creating package to install to " ${CPACK_PACKAGE_INSTALL_DIRECTORY})
	ADD_DEFINITIONS(-DRESOURCE_DIR="${CPACK_PACKAGE_INSTALL_DIRECTORY}/share/mondoc/")
  ENDIF()
ENDIF()

IF(BUILD_PACKAGE)
  INCLUDE(CPack)
ENDIF()

if(ENABLE_ADDRESS_SANITIZER)
	add_definitions(-fsanitize=address)
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(TestLuaSource ${SOURCES} ${HEADERS} test/TestLuaSource.cpp)
target_link_libraries(TestLuaSource ${GTEST_BOTH_LIBRARIES})

add_test(LuaSourceTest TestLuaSource)
include(CTest)

set_property(TARGET mondoc PROPERTY CXX_STANDARD 11)
set_property(TARGET TestLuaSource PROPERTY CXX_STANDARD 11)

copy_files(*.css ${CMAKE_BINARY_DIR} copy-css)
copy_files(*.js ${CMAKE_BINARY_DIR} copy-js)

add_dependencies(TestLuaSource copy-css copy-js)
