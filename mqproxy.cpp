/**
 * mqproxy - simple ZeroMQ proxy and forwarder
 * Copyright (c) 2019 - 2020 Viktor Garske <info@v-gar.de>
 */
#include <iostream>
#include <sstream>
#include <regex>

#include <cstdlib>

#include <zmq.hpp>

#define RET_SUCCESS 	0
#define RET_ERROR 	1
#define RET_ERROR_REGEX	2

const char  DEFAULT_SCHEME[] = "tcp";
const char* DEFAULT_FRONTEND_ADDRESS = "127.0.0.1";
const char* DEFAULT_FRONTEND_PORT = "5570";
const char* DEFAULT_BACKEND_ADDRESS = "127.0.0.1";
const char* DEFAULT_BACKEND_PORT = "5571";

const std::regex regex_ip_address(
		R"((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))"
		R"((\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3})");

struct BindConfig {
	const char*	frontend_address;
	const char*	frontend_port;
	const char*	backend_address;
	const char*	backend_port;

	BindConfig(const char* frontend_address,
			const char* frontend_port,
			const char* backend_address,
			const char* backend_port)
	: frontend_address{frontend_address},
		frontend_port{frontend_port},
		backend_address{backend_address},
		backend_port{backend_port}
	{}

	BindConfig(const char* frontend_address,
			const char* backend_address)
	: BindConfig(frontend_address,
			DEFAULT_FRONTEND_PORT, 
			backend_address,
			DEFAULT_BACKEND_PORT)
	{}

	BindConfig()
	: BindConfig(DEFAULT_BACKEND_ADDRESS,
			DEFAULT_FRONTEND_PORT, 
			DEFAULT_BACKEND_ADDRESS,
			DEFAULT_BACKEND_PORT)
	{}
};

inline void usage()
{
	std::cout << "Usage: mqproxy <bind frontend> <port frontend> "
		<< "<bind backend> <port backend>\n"
		<< "       mqproxy <bind frontend> <bind backend>"
		<< std::endl;
}

bool is_ip_address(const std::string& address)
{
	return regex_match(address, regex_ip_address);
}

inline signed char parse_args(struct BindConfig& config,
		int argc, char* argv[])
{
	// Parse arguments
	if (argc == 5) {
		// Set bind and port
		config = BindConfig(
				argv[1],
				argv[2],
				argv[3],
				argv[4]
				);
	} else if (argc == 3) {
		// Set bind and default port
		if (!is_ip_address(argv[1]) ||
				!is_ip_address(argv[2]))
			return RET_ERROR_REGEX;

		config = BindConfig(argv[1], argv[2]);
	} else {
		return RET_ERROR;
	}

	return RET_SUCCESS;
}

inline std::tuple<std::string, std::string> build_urls(
		struct BindConfig& config)
{
	std::stringstream frontend_url;
	frontend_url << DEFAULT_SCHEME
		<< "://"
		<< config.frontend_address
		<< ":"
		<< config.frontend_port;
	std::stringstream backend_url;
	backend_url << DEFAULT_SCHEME
		<< "://"
		<< config.backend_address
		<< ":"
		<< config.backend_port;

	return std::make_tuple(frontend_url.str(),
			backend_url.str());
}

signed char start_proxy(struct BindConfig& config)
{
	zmq::context_t ctx(1);
	zmq::socket_t frontend(ctx, ZMQ_XSUB);
	zmq::socket_t backend(ctx, ZMQ_XPUB);

	std::string frontend_url, backend_url;
	std::tie(frontend_url, backend_url) = build_urls(config);

	std::cout << "Binding frontend to "
		<< frontend_url
		<< " and backend to " 
		<< backend_url
		<< "..."
		<< std::endl;

	int bind_f = zmq_bind(frontend, frontend_url.c_str());
	int bind_b = zmq_bind(backend, backend_url.c_str());

	if (bind_f != 0 || bind_f != 0) {
		std::cerr << "Error while binding!"
			<< std::endl;
		return RET_ERROR;
	}

	zmq::proxy(frontend, backend);

	return RET_SUCCESS;
}

int main(int argc, char* argv[])
{
	struct BindConfig config;

	auto args_result = parse_args(config, argc, argv);
	if (args_result != RET_SUCCESS) {
		if (args_result == RET_ERROR)
			std::cerr << "Error: missing arguments.\n";
		if (args_result == RET_ERROR_REGEX)
			std::cerr << "Error: invalid arguments.\n";
		usage();
		return 1;
	}

	auto start_result = start_proxy(config);

	if (start_result != RET_SUCCESS)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
