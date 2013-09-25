/*
 * ANNDaemon_client.cpp
 *
 *  Created on: Jul 6, 2013
 *      Author: zxwang
 */

#include "ANNDaemon.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::gvlad;

using boost::shared_ptr;

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cout << "Usage: ANNDaemon_client port image_path k" << std::endl;
		return 0;
	}
	shared_ptr<TSocket> socket(new TSocket("localhost", atoi(argv[1])));
	shared_ptr<TBufferedTransport> transport(new TBufferedTransport(socket));
	shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
	ANNDaemonClient client(protocol);
	try {
		transport->open();
		std::vector<Neighbor> result_vector;
		client.query(result_vector, argv[2], atoi(argv[3]));
		for (size_t i = 0; i < result_vector.size(); ++i) {
			std::cout << "#" << i << " : " << result_vector[i].id << std::endl;
		}
		transport->close();
	} catch (TException &tx) {
		printf("ERROR: %s\n", tx.what());
	}
	return 0;
}

