# Allowed Functions Documentation

This document describes all the system functions allowed for use in the webserv project, organized by functional categories.

# Process Management

## execve

### Prototype
```c
int execve(const char *pathname, char *const argv[], char *const envp[]);
```

### Arguments
- `pathname`: Path to the executable file
- `argv[]`: Array of argument strings (NULL-terminated)
- `envp[]`: Array of environment strings (NULL-terminated)

### Return
Does not return on success; returns -1 on error

### Description
Executes a program by replacing the current process image. Used for CGI script execution in webserv.

## fork

### Prototype
```c
pid_t fork(void);
```

### Arguments
None

### Return
Returns child PID to parent, 0 to child, -1 on error

### Description
Creates a new process by duplicating the current process. Required for executing CGI scripts in separate processes.

## waitpid

### Prototype
```c
pid_t waitpid(pid_t pid, int *status, int options);
```

### Arguments
- `pid`: Process ID to wait for
- `status`: Pointer to store exit status
- `options`: Wait options (WNOHANG, etc.)

### Return
Returns child PID on success, 0 if WNOHANG and no child, -1 on error

### Description
Waits for child process state changes. Essential for managing CGI child processes.

## kill

### Prototype
```c
int kill(pid_t pid, int sig);
```

### Arguments
- `pid`: Process ID
- `sig`: Signal number

### Return
Returns 0 on success, -1 on error

### Description
Sends signals to processes. Used for terminating CGI processes or handling timeouts.

## signal

### Prototype
```c
sighandler_t signal(int signum, sighandler_t handler);
```

### Arguments
- `signum`: Signal number
- `handler`: Signal handler function

### Return
Returns previous handler on success, SIG_ERR on error

### Description
Sets signal handlers. Used for graceful server shutdown and signal management.

# Inter-Process Communication

## pipe

### Prototype
```c
int pipe(int pipefd[2]);
```

### Arguments
- `pipefd[2]`: Array to store file descriptors (pipefd[0] for read, pipefd[1] for write)

### Return
Returns 0 on success, -1 on error

### Description
Creates a pipe for inter-process communication. Essential for CGI communication between parent and child processes.

## socketpair

### Prototype
```c
int socketpair(int domain, int type, int protocol, int sv[2]);
```

### Arguments
- `domain`: Protocol family (AF_UNIX)
- `type`: Socket type (SOCK_STREAM)
- `protocol`: Protocol (usually 0)
- `sv[2]`: Array to store socket descriptors

### Return
Returns 0 on success, -1 on error

### Description
Creates a pair of connected sockets for bi-directional communication between processes.

## dup

### Prototype
```c
int dup(int oldfd);
```

### Arguments
- `oldfd`: File descriptor to duplicate

### Return
Returns new file descriptor on success, -1 on error

### Description
Creates a duplicate of a file descriptor. Useful for redirecting input/output in CGI processes.

## dup2

### Prototype
```c
int dup2(int oldfd, int newfd);
```

### Arguments
- `oldfd`: Source file descriptor
- `newfd`: Target file descriptor number

### Return
Returns new file descriptor on success, -1 on error

### Description
Duplicates a file descriptor to a specific descriptor number. Essential for CGI stdin/stdout redirection.

# Error Handling

## strerror

### Prototype
```c
char *strerror(int errnum);
```

### Arguments
- `errnum`: Error number (usually errno)

### Return
Returns a pointer to error message string

### Description
Converts error numbers to human-readable error messages for debugging and logging.

## gai_strerror

### Prototype
```c
const char *gai_strerror(int errcode);
```

### Arguments
- `errcode`: Error code from getaddrinfo()

### Return
Returns a pointer to error message string

### Description
Converts getaddrinfo() error codes to human-readable messages for network-related error handling.

## errno

### Prototype
```c
extern int errno;
```

### Arguments
None (global variable)

### Return
Current error number

### Description
Global variable that contains the error code of the last failed system call. Used throughout webserv for error checking.

# Network Programming

## socket

### Prototype
```c
int socket(int domain, int type, int protocol);
```

### Arguments
- `domain`: Protocol family (AF_INET, AF_INET6)
- `type`: Socket type (SOCK_STREAM, SOCK_DGRAM)
- `protocol`: Protocol (usually 0)

### Return
Returns socket descriptor on success, -1 on error

### Description
Creates a communication endpoint (socket). Foundation of all network communication in webserv.

## bind

### Prototype
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### Arguments
- `sockfd`: Socket descriptor
- `addr`: Address structure
- `addrlen`: Address structure size

### Return
Returns 0 on success, -1 on error

### Description
Binds a socket to a specific address and port. Required for server socket setup before listening.

## listen

### Prototype
```c
int listen(int sockfd, int backlog);
```

### Arguments
- `sockfd`: Socket descriptor
- `backlog`: Maximum pending connections

### Return
Returns 0 on success, -1 on error

### Description
Marks a socket as passive, ready to accept connections. Essential for server socket setup.

## accept

### Prototype
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

### Arguments
- `sockfd`: Listening socket descriptor
- `addr`: Buffer for client address
- `addrlen`: Size of address buffer

### Return
Returns new socket descriptor on success, -1 on error

### Description
Accepts incoming connections on a listening socket. Creates new socket for each client connection.

## connect

### Prototype
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### Arguments
- `sockfd`: Socket descriptor
- `addr`: Server address structure
- `addrlen`: Address structure size

### Return
Returns 0 on success, -1 on error

### Description
Establishes connection to a server. Primarily used for client-side connections (may be used for proxying).

## send

### Prototype
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

### Arguments
- `sockfd`: Socket descriptor
- `buf`: Buffer containing data to send
- `len`: Length of data
- `flags`: Send flags

### Return
Returns number of bytes sent on success, -1 on error

### Description
Sends data through a socket connection. Used for sending HTTP responses to clients.

## recv

### Prototype
```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

### Arguments
- `sockfd`: Socket descriptor
- `buf`: Buffer to store received data
- `len`: Buffer size
- `flags`: Receive flags

### Return
Returns number of bytes received, 0 on connection close, -1 on error

### Description
Receives data from a socket connection. Used for reading HTTP requests from clients.

## setsockopt

### Prototype
```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

### Arguments
- `sockfd`: Socket descriptor
- `level`: Protocol level (SOL_SOCKET)
- `optname`: Option name (SO_REUSEADDR, etc.)
- `optval`: Option value
- `optlen`: Option value size

### Return
Returns 0 on success, -1 on error

### Description
Sets socket options. Commonly used to set SO_REUSEADDR for server sockets.

## getsockname

### Prototype
```c
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

### Arguments
- `sockfd`: Socket descriptor
- `addr`: Buffer for address
- `addrlen`: Address buffer size

### Return
Returns 0 on success, -1 on error

### Description
Retrieves the local address of a socket. Useful for determining assigned port numbers.

# Network Utilities

## htons

### Prototype
```c
uint16_t htons(uint16_t hostshort);
```

### Arguments
- `hostshort`: 16-bit number in host byte order

### Return
Returns 16-bit number in network byte order

### Description
Converts 16-bit integers from host to network byte order. Used for port numbers in socket programming.

## htonl

### Prototype
```c
uint32_t htonl(uint32_t hostlong);
```

### Arguments
- `hostlong`: 32-bit number in host byte order

### Return
Returns 32-bit number in network byte order

### Description
Converts 32-bit integers from host to network byte order. Used for IP addresses in socket programming.

## ntohs

### Prototype
```c
uint16_t ntohs(uint16_t netshort);
```

### Arguments
- `netshort`: 16-bit number in network byte order

### Return
Returns 16-bit number in host byte order

### Description
Converts 16-bit integers from network to host byte order. Used when receiving port numbers from network.

## ntohl

### Prototype
```c
uint32_t ntohl(uint32_t netlong);
```

### Arguments
- `netlong`: 32-bit number in network byte order

### Return
Returns 32-bit number in host byte order

### Description
Converts 32-bit integers from network to host byte order. Used when receiving IP addresses from network.

## getaddrinfo

### Prototype
```c
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
```

### Arguments
- `node`: Hostname or IP address
- `service`: Port number or service name
- `hints`: Address criteria
- `res`: Pointer to store result list

### Return
Returns 0 on success, non-zero error code on failure

### Description
Resolves hostnames and service names to socket addresses. Used for address resolution in network programming.

## freeaddrinfo

### Prototype
```c
void freeaddrinfo(struct addrinfo *res);
```

### Arguments
- `res`: Address info structure to free

### Return
None (void function)

### Description
Frees memory allocated by getaddrinfo(). Essential for preventing memory leaks.

## getprotobyname

### Prototype
```c
struct protoent *getprotobyname(const char *name);
```

### Arguments
- `name`: Protocol name ("tcp", "udp")

### Return
Returns protocol entry structure, NULL on error

### Description
Looks up protocol information by name. May be used for protocol-specific socket operations.

# I/O Multiplexing

## select

### Prototype
```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

### Arguments
- `nfds`: Highest file descriptor number + 1
- `readfds`: Set of file descriptors to check for readability
- `writefds`: Set of file descriptors to check for writability
- `exceptfds`: Set of file descriptors to check for exceptions
- `timeout`: Timeout specification

### Return
Returns number of ready descriptors, 0 on timeout, -1 on error

### Description
Multiplexes I/O operations by monitoring multiple file descriptors. Alternative to epoll for handling multiple connections.

## poll

### Prototype
```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```

### Arguments
- `fds`: Array of pollfd structures
- `nfds`: Number of file descriptors
- `timeout`: Timeout in milliseconds

### Return
Returns number of ready descriptors, 0 on timeout, -1 on error

### Description
More efficient alternative to select() for I/O multiplexing. Monitors multiple file descriptors for I/O events.

## epoll_create

### Prototype
```c
int epoll_create(int size);
```

### Arguments
- `size`: Size hint (ignored in modern kernels)

### Return
Returns epoll file descriptor on success, -1 on error

### Description
Creates an epoll instance for efficient I/O event monitoring. Linux-specific alternative to select/poll.

## epoll_ctl

### Prototype
```c
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

### Arguments
- `epfd`: Epoll file descriptor
- `op`: Operation (EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL)
- `fd`: Target file descriptor
- `event`: Event specification

### Return
Returns 0 on success, -1 on error

### Description
Controls epoll instance by adding, modifying, or removing file descriptors from monitoring.

## epoll_wait

### Prototype
```c
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

### Arguments
- `epfd`: Epoll file descriptor
- `events`: Buffer for returned events
- `maxevents`: Maximum number of events
- `timeout`: Timeout in milliseconds

### Return
Returns number of ready events, 0 on timeout, -1 on error

### Description
Waits for I/O events on an epoll instance. Core function for epoll-based event loops.

## kqueue

### Prototype
```c
int kqueue(void);
```

### Arguments
None

### Return
Returns kqueue descriptor on success, -1 on error

### Description
Creates a kernel event queue. BSD-specific alternative to epoll for efficient event monitoring.

## kevent

### Prototype
```c
int kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);
```

### Arguments
- `kq`: kqueue descriptor
- `changelist`: Array of events to register
- `nchanges`: Number of changes
- `eventlist`: Buffer for returned events
- `nevents`: Maximum events to return
- `timeout`: Timeout specification

### Return
Returns number of events, 0 on timeout, -1 on error

### Description
Registers events and waits for them on a kqueue. Used for BSD-style event monitoring.

# File Operations

## open

### Prototype
```c
int open(const char *pathname, int flags, mode_t mode);
```

### Arguments
- `pathname`: File path
- `flags`: Open flags (O_RDONLY, O_WRONLY, etc.)
- `mode`: File permissions (when creating)

### Return
Returns file descriptor on success, -1 on error

### Description
Opens files for reading or writing. Used for serving static files and file uploads.

## close

### Prototype
```c
int close(int fd);
```

### Arguments
- `fd`: File descriptor to close

### Return
Returns 0 on success, -1 on error

### Description
Closes a file descriptor. Essential for resource management and cleaning up sockets.

## read

### Prototype
```c
ssize_t read(int fd, void *buf, size_t count);
```

### Arguments
- `fd`: File descriptor
- `buf`: Buffer to store data
- `count`: Maximum bytes to read

### Return
Returns number of bytes read, 0 on EOF, -1 on error

### Description
Reads data from a file descriptor. Used for reading from sockets, files, and pipes.

## write

### Prototype
```c
ssize_t write(int fd, const void *buf, size_t count);
```

### Arguments
- `fd`: File descriptor
- `buf`: Data buffer
- `count`: Number of bytes to write

### Return
Returns number of bytes written, -1 on error

### Description
Writes data to a file descriptor. Used for writing to sockets, files, and pipes.

## access

### Prototype
```c
int access(const char *pathname, int mode);
```

### Arguments
- `pathname`: File path
- `mode`: Access mode (R_OK, W_OK, X_OK, F_OK)

### Return
Returns 0 if accessible, -1 otherwise

### Description
Checks file accessibility and permissions. Used for validating file paths and permissions.

## stat

### Prototype
```c
int stat(const char *pathname, struct stat *statbuf);
```

### Arguments
- `pathname`: File path
- `statbuf`: Buffer for file information

### Return
Returns 0 on success, -1 on error

### Description
Retrieves file information and metadata. Essential for serving files and directory listings.

## fcntl

### Prototype
```c
int fcntl(int fd, int cmd, ...);
```

### Arguments
- `fd`: File descriptor
- `cmd`: Command (F_GETFL, F_SETFL, etc.)
- `...`: Additional arguments depending on command

### Return
Returns value depends on command, -1 on error

### Description
Performs various operations on file descriptors. Essential for setting non-blocking mode on sockets.

# Directory Operations

## chdir

### Prototype
```c
int chdir(const char *path);
```

### Arguments
- `path`: Directory path

### Return
Returns 0 on success, -1 on error

### Description
Changes the current working directory. May be used for CGI script execution in correct directory.

## opendir

### Prototype
```c
DIR *opendir(const char *name);
```

### Arguments
- `name`: Directory path

### Return
Returns directory stream pointer on success, NULL on error

### Description
Opens a directory for reading. Used for implementing directory listing functionality.

## readdir

### Prototype
```c
struct dirent *readdir(DIR *dirp);
```

### Arguments
- `dirp`: Directory stream pointer

### Return
Returns directory entry pointer, NULL on end or error

### Description
Reads directory entries one by one. Used for generating directory index pages.

## closedir

### Prototype
```c
int closedir(DIR *dirp);
```

### Arguments
- `dirp`: Directory stream pointer

### Return
Returns 0 on success, -1 on error

### Description
Closes a directory stream. Essential for resource management when reading directories.
