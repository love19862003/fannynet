ADD_EXECUTABLE(test test.cpp)
ADD_DEPENDENCIES(test fannynet)
TARGET_LINK_LIBRARIES(test fannynet ${LINK_LIBS})
