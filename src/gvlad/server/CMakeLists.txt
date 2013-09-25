ADD_DEFINITIONS( "-DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H" )
ADD_EXECUTABLE(ANNDaemon_client
	ANNDaemon_client.cpp
	ANNDaemon_constants.h ANNDaemon_constants.cpp
	ANNDaemon_types.h ANNDaemon_types.cpp
	ANNDaemon.h ANNDaemon.cpp
)
ADD_EXECUTABLE(ANNDaemon_server
	ANNDaemon_constants.h ANNDaemon_constants.cpp
	ANNDaemon_server.cpp
	ANNDaemon_types.h ANNDaemon_types.cpp
	ANNDaemon.h ANNDaemon.cpp
)

TARGET_LINK_LIBRARIES(ANNDaemon_client gvlad_lib thrift)
TARGET_LINK_LIBRARIES(ANNDaemon_server gvlad_lib thrift)