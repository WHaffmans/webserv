Welcome to WebServ Examples Directory

This directory contains example files and configurations to demonstrate 
various features of the WebServ HTTP server.

Files in this directory:
========================

webserv.conf        - Complete example configuration file
README.md          - This file
test.txt           - Simple text file for testing
sample.html        - Basic HTML page
large-file.dat     - Larger file for testing file serving performance

Configuration Examples:
======================

The webserv.conf file demonstrates:
- Multiple server blocks
- Virtual host configuration
- SSL/HTTPS setup
- Static file serving
- API endpoint configuration
- Directory listing
- Custom error pages
- Security headers
- Caching strategies

Testing Examples:
================

You can test various server features using these files:

1. Static File Serving:
   curl http://localhost:8080/examples/test.txt

2. HTML Content:
   curl http://localhost:8080/examples/sample.html

3. Directory Listing:
   curl http://localhost:8080/examples/

4. Large File Download:
   curl -O http://localhost:8080/examples/large-file.dat

5. Error Handling:
   curl http://localhost:8080/examples/nonexistent.html

Performance Testing:
===================

Use tools like Apache Bench (ab) or wrk to test performance:

ab -n 1000 -c 10 http://localhost:8080/examples/test.txt
wrk -t4 -c100 -d30s http://localhost:8080/examples/

Security Testing:
================

Test various security scenarios:

1. Directory traversal attempts:
   curl http://localhost:8080/examples/../../../etc/passwd

2. Hidden file access:
   curl http://localhost:8080/examples/.htaccess

3. Backup file access:
   curl http://localhost:8080/examples/config.php~

All these should be properly blocked by WebServ's security features.

For more information, visit the WebServ documentation.
