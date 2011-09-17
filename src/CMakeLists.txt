cmake_minimum_required(VERSION 2.4 FATAL_ERROR)

project(dhtperf)
set(DHTPERF_VERSION 0.10.0)

add_definitions(-Wall -Wextra -pedantic -O2 -g0)

#if(NOT DEFINED STATIC_BUILD)
#    message("You're using a STATIC build. To turn it off, pass "
#	    "-DSTATIC_BUILD=OFF as a parameter to cmake")
#    set(STATIC_BUILD TRUE)
#endif(NOT DEFINED STATIC_BUILD)

if(STATIC_BUILD)
    set(Boost_USE_STATIC_LIBS ON)
endif(STATIC_BUILD)

# This macro doesn't seem to work correctly on cmake 2.4
find_package(Boost 1.37.0 REQUIRED COMPONENTS date_time
					      program_options
					      regex
					      system
					      thread)

if(NOT DEFINED Boost_LIBRARIES)
    # FindBoost module is too old
    set(Boost_LIBRARIES -L${Boost_LIBRARY_DIRS}
			-lboost_date_time-mt
			-lboost_program_options-mt
			-lboost_regex-mt
			-lboost_system-mt
			-lboost_thread-mt)
endif(NOT DEFINED Boost_LIBRARIES)

configure_file(config.h.cmake config.h)
# This is just for config.h:
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(common_sources
	metrics/NodeLatency.cpp
	metrics/NodeSuccessRate.cpp
	protocol.cpp)

set(master_sources
	master/dhtperf-master.cpp
	master/ControllerConnection.cpp
	master/DhtperfMaster.cpp
	master/MasterWorkload.cpp
	master/MasterWorkloadProfile.cpp
	master/ResultProcessor.cpp
	metrics/GeneralLatency.cpp
	metrics/GeneralSuccessRate.cpp
	metrics/NodesStatus.cpp)

set(controller_sources
	controller/dhtperf-controller.cpp
	controller/ControllerWorkloadProfile.cpp
	controller/DhtperfController.cpp
	controller/DhtNode.cpp
	controller/dhtnodes/DummyDht.cpp
	controller/dhtnodes/GenericRubyWrapper.cpp
	controller/dhtnodes/KaddemoWrapper.cpp
	controller/dhtnodes/KaddemoIdWrapper.cpp
	controller/dhtnodes/OpenchordWrapper.cpp
	controller/dhtnodes/OwdhtshellWrapper.cpp
	controller/dhtnodes/PastConsoleWrapper.cpp
	controller/dhtnodes/RWPopen.cpp
	controller/dhtnodes/Wrapper.cpp
	controller/profiles/Exact.cpp
	controller/profiles/FullDictionary.cpp
	controller/profiles/Interval.cpp
	controller/profiles/Now.cpp
	controller/profiles/NTimesRandom.cpp
	controller/profiles/PoissonRandom.cpp
	controller/profiles/WeibullRandom.cpp
	controller/Utils.cpp
	events.cpp)

set(dhtperfd_sources
	service/dhtperfd.cpp)

set(dhtperfd_client_sources
	service/dhtperfd-client.cpp)

add_library(dhtperf-common STATIC ${common_sources})
add_executable(dhtperf-master ${master_sources})
add_executable(dhtperf-controller ${controller_sources})
add_executable(dhtperfd ${dhtperfd_sources})
add_executable(dhtperfd-client ${dhtperfd_client_sources})

set(common_libraries ${Boost_LIBRARIES} dhtperf-common -lpthread)

if(STATIC_BUILD)
    set(common_libraries -pthread ${common_libraries})
    set(CMAKE_EXE_LINKER_FLAGS -static)

    target_link_libraries(dhtperf-common -pthread ${Boost_LIBRARIES})

    foreach(target dhtperf-common dhtperf-master dhtperf-controller)
	set_target_properties(${target} PROPERTIES LINK_SEARCH_END_STATIC TRUE)
    endforeach(target)
endif(STATIC_BUILD)

foreach(target dhtperf-master dhtperf-controller)
    target_link_libraries(${target} ${common_libraries})
endforeach(target)

target_link_libraries(dhtperfd        -lboost_system-mt -lpthread)
target_link_libraries(dhtperfd-client -lboost_system-mt -lboost_thread-mt -lpthread)
