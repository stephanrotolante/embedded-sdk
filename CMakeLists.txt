cmake_minimum_required(VERSION 3.19)

if(NOT DEFINED ENV{WIFI_SSID} OR NOT DEFINED ENV{WIFI_PASSWORD} OR NOT DEFINED ENV{LIVEKIT_URL})
	message(FATAL_ERROR "Env variables WIFI_SSID, WIFI_PASSWORD and LIVEKIT_URL must be set")
endif()

add_compile_definitions(WIFI_SSID="$ENV{WIFI_SSID}")
add_compile_definitions(WIFI_PASSWORD="$ENV{WIFI_PASSWORD}")
add_compile_definitions(LIVEKIT_URL="$ENV{LIVEKIT_URL}")

execute_process(
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND "protoc" "--c_out=deps/livekit-protocol-generated" "-I=./deps/livekit-protocol" "livekit_rtc.proto" "livekit_models.proto"
  COMMAND_ERROR_IS_FATAL ANY
)



if(NOT DEFINED IDF_TARGET)
	add_compile_definitions(TARGET_LINUX)
	set(IDF_TARGET "linux")
endif()

if(NOT IDF_TARGET STREQUAL "linux")
	include($ENV{IDF_PATH}/tools/cmake/project.cmake)
else()
    	set(SOURCES
        	main/main.cpp
        	# Add more source files here
    	)

    	# Add the executable for the linux target
    	add_executable(embeded-sdk-linux ${SOURCES})
endif()
project(src)





