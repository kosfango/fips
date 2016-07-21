
/*
 * Finds ASCIIZ address
 */
const char *get_hostname (struct sockaddr_in * addr, char *host, int len);

void copy_hostent(struct hostent *dest, struct hostent *src);

/*
 * Sets non-blocking mode for a given socket
 */
void setsockopts (SOCKET s);

/*
 * Find the port number (in the host byte order) by a port number string or
 * a service name. Find_port ("") will return binkp's port from
 * /etc/services or even (if there is no binkp entry) 24554.
 * Returns 0 on error.
 */
int find_port (char *s);
