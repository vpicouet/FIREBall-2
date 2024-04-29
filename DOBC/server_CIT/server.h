#ifndef SERVER_H_
#define SERVER_H_

void error(const char *msg);

int open_server();
int close_server();
int listen_server();

#endif // SERVER_H_
