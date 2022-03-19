add_library(MQDispatcher INTERFACE MQDispatcher.hpp
                                   #interfaces.h
                                   )

target_include_directories(MQDispatcher INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})