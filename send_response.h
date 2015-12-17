#include <stdbool.h>
#include "httpheaders.h"
#include "httpdata.h"

bool send_response(int connection_socket, HTTPResponseData *);