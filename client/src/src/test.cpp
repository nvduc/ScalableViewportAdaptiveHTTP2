//#include "nghttp.h"

#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif // HAVE_FCNTL_H
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif // HAVE_NETINET_IN_H
#include <netinet/tcp.h>
#include <getopt.h>

#include <cassert>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <tuple>
#include <vector>

#include <openssl/err.h>
#include <math.h>

#ifdef HAVE_JANSSON
#include <jansson.h>
#endif // HAVE_JANSSON

#include "app_helper.h"
#include "HtmlParser.h"
//#include "util.h"
#include "base64.h"
#include "ssl.h"
#include "template.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
//#include <chrono>
//#include <thread>
//#include <mutex>
//#include <boost/algorithm/string.hpp>
//#include <boost/bind.hpp>
//#include <nghttp2/asio_http2_server.h>
#include <netdb.h>
#include <netinet/in.h>

#ifndef O_BINARY
#define O_BINARY (0)
#endif // O_BINARY
using namespace std;
int main(){
	ev_timer tev;
}