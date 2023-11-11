#ifndef __FILE_IO_H__
#define __FILE_IO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include "../../mongoose.h"

struct file_mime_mappings {
    const char *extension;
    const char *mime_type;
};

struct http_file_info {
    char* path; 
    FILE* f_ptr;
    long file_size;
    char* content_type;
};

struct fileio_tls_certs {
    char *ca;
    char *cert;
    char *key;
};

struct server_certs {
    struct fileio_tls_certs s;
    struct fileio_tls_certs c;
};

extern struct file_mime_mappings FILE_IO_mime_types[];



char* get_executable_path();
void load_certificates(struct server_certs * ss);
void get_certificates(struct fileio_tls_certs* certs, const char ** filenames);
char* determine_mime_type(const char* f_name);
void get_http_file_from_path(struct http_file_info* f_info, char* path);
void get_http_file_from_uri(struct http_file_info* f_info, struct mg_http_message* msg);

#endif