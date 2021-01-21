#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <mutex>


#if defined(__LINUX__)
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <linux/un.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h> 
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>  
#include <net/if.h>  
#include <sys/file.h>
#else

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p) do { if(p) { delete p; p = nullptr; } }while(0)
#endif

using namespace std;