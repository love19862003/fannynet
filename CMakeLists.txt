CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(FannyNet)

IF(UNIX)
  SET(CMAKE_C_COMPILER gcc)
  SET(CMAKE_CXX_COMPILER g++)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -Wno-unused-local-typedefs -ffunction-sections -fdata-sections")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexceptions -Wno-unused-local-typedefs -Wreorder -ffunction-sections -fdata-sections")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

  SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -D_DEBUG -O0 -Wall")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wpointer-arith -Wcast-qual")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wunreachable-code -Wundef -Woverloaded-virtual")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wdouble-promotion -Wunused-parameter -g -ggdb -fpic -fPIC")

  SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -DNDEBUG -Os -Wall")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wextra -Wpointer-arith")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wcast-qual -Wunreachable-code -Wundef")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wdouble-promotion -Woverloaded-virtual")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wunused-parameter -g -fpic -fPIC")

  EXECUTE_PROCESS(
    COMMAND gcc --version
    OUTPUT_VARIABLE GCCVERSIONSTR
  )
  STRING(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]).*" "\\1" GCCVERSION ${GCCVERSIONSTR})
  MESSAGE(STATUS  "gcc compiler:${GCCVERSION}")

  IF(EXISTS "${CMAKE_SOURCE_DIR}/.svn/")
    FIND_PACKAGE(Subversion)
    IF(SUBVERSION_FOUND)
      Subversion_WC_INFO(${CMAKE_SOURCE_DIR} FannyNet)
      SET(VERSION_REVISION ${FannyNet_WC_REVISION})
      MESSAGE(STATUS "FannyNet build svn--version ${VERSION_REVISION}")
      CONFIGURE_FILE("Version.h.in" "${CMAKE_SOURCE_DIR}/include/version.h")
    ENDIF(SUBVERSION_FOUND)
  ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/.svn/")

  IF(EXISTS "${CMAKE_SOURCE_DIR}/.git/")
    EXECUTE_PROCESS(
      COMMAND git rev-list HEAD -n 1
      OUTPUT_VARIABLE VERSION_REVISION_STR
    )
    STRING(REGEX MATCH "^[a-zA-Z0-9]+" VERSION_REVISION ${VERSION_REVISION_STR})
    MESSAGE(STATUS "FannyNet build git--version ${VERSION_REVISION}")
    CONFIGURE_FILE("Version.h.in" "${CMAKE_SOURCE_DIR}/include/version.h")
  ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/.git/")

ENDIF(UNIX)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/")
SET(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/")

INCLUDE_DIRECTORIES("./include/")
INCLUDE_DIRECTORIES("./src/")
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})

IF(UNIX)
  INCLUDE_DIRECTORIES("/usr/local/libxs/include")
  INCLUDE_DIRECTORIES("/usr/local/include")
  LINK_DIRECTORIES("/usr/local/lib/")
  LINK_DIRECTORIES("/usr/local/lib64/")
  LINK_DIRECTORIES("/usr/local/gperftools/lib/")

  SET(LINK_LIBS ${LINK_LIBS} boost_system)
  SET(LINK_LIBS ${LINK_LIBS} boost_thread)
  SET(LINK_LIBS ${LINK_LIBS} boost_context)
  SET(LINK_LIBS ${LINK_LIBS} boost_coroutine)
  SET(LINK_LIBS ${LINK_LIBS} pthread)
  SET(LINK_LIBS ${LINK_LIBS} tcmalloc)
ENDIF(UNIX)
AUX_SOURCE_DIRECTORY(./src/ SOURCE_NET)
ADD_LIBRARY(fannynet STATIC block.cpp connection.cpp manager.cpp net_buffer.cpp service.cpp tcp_object.cpp)

ADD_EXECUTABLE(test main.cpp)
ADD_DEPENDENCIES(fannynet)
TARGET_LINK_LIBRARIES(fanny_server fannynet ${LINK_LIBS})