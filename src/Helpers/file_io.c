#include "file_io.h"

struct file_mime_mappings FILE_IO_mime_types[] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".jpg", "image/jpeg"},
    {".png", "image/png"},
    {".txt", "text/plain"},
    {".pdf", "application/pdf"}
    // ...
};

int is_subdir_of_root(char* path, const char* root) {
    struct stat s;
    if (stat(path, &s) == -1) return 0; // Error return
    if (!S_ISDIR(s.st_mode)) return 0;
    /* Check if root found */
    if (strcmp(path, root) == 0) {
        return 1; 
    }
    char *parent = dirname(path);
    if (parent == NULL) { /* Not child of Root Directory */
        return 0;
    }
    /* Recursive Case */
    return is_subdir_of_root(parent, root);
}

char* get_executable_path() {
    char exe_path[1048];
    ssize_t len = readlink("/proc/self/exe", exe_path, 512 - 1);
    if (len == -1) {
        return NULL;
    }

    exe_path[len] = '\0';
    char* path = strdup(dirname(exe_path)); // dirname can modify its argument
    return path;
}

void load_certificates(struct server_certs * ss) {
  const char* s_files[] = {"ca.pem", "cert.pem", "key.pem"};
  const char* c_files[] = {"client_ca.pem", "client_cert.pem", "client_key.pem"};
  get_certificates(&ss->s, s_files);
  get_certificates(&ss->c, c_files);
}

void get_certificates(struct fileio_tls_certs* certs, const char** filenames) {
  char** ctx_cert = (char**) malloc(3*sizeof(char*));
  for (int i = 0; i < 3; i++) {
      FILE* file = fopen(filenames[i], "rb");
      if (file == NULL) {
        fprintf(stderr, "Failed to open Cert file: %s\n", filenames[i]);
        ctx_cert[i] = NULL;
        continue;
      }

      fseek(file, 0, SEEK_END);
      long length = ftell(file);
      ctx_cert[i] = (char*) malloc(length+1);
      fseek(file, 0, SEEK_SET);
      fread(ctx_cert[i], 1, length, file);
      ctx_cert[i][length] = '\0'; 
      fclose(file);
  }
  certs->ca = ctx_cert[0]; certs->cert = ctx_cert[1]; certs->key = ctx_cert[2];
  free(ctx_cert);
}

char* determine_mime_type(const char* f_name) {
  // Find the file extension (last occurrence of '.' in the filename)
  const char *ext = strrchr(f_name, '.');
  if (ext != NULL) {
    // Search the mapping for the extension
    for (size_t i = 0; i < sizeof(FILE_IO_mime_types) / sizeof(FILE_IO_mime_types[0]); ++i) if (strcmp(ext, FILE_IO_mime_types[i].extension) == 0) return FILE_IO_mime_types[i].mime_type;
  }
  return "application/octet-stream";
}

void get_http_file_from_path(struct http_file_info* f_info, char* path ) {
  f_info->path = path;
  FILE *file = fopen(f_info->path, "rb");
  if (file != NULL) {
    f_info->f_ptr = file;
    fseek(file, 0, SEEK_END);
    f_info->file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    f_info->content_type = determine_mime_type(f_info->path);
    }
    else {
        f_info->f_ptr == NULL; f_info->file_size = -1;
    }
}

void get_http_file_from_uri(struct http_file_info* f_info, struct mg_http_message* msg) {
  strcat(f_info->path, ".");
  char *space_position = strchr(msg->uri.ptr, ' ');
  if (space_position != NULL) {
    if ((space_position - msg->uri.ptr + 1) > 512) /* Avoid Buffer Overflow*/ {
      f_info->f_ptr == NULL; f_info->file_size = -1; return;
    }
    strncpy(f_info->path+1, msg->uri.ptr, space_position - msg->uri.ptr);
    f_info->path[space_position - msg->uri.ptr + 1] = '\0'; // Null-terminate the string
  }
  FILE *file = fopen(f_info->path, "rb");
  if (file != NULL) {
    f_info->f_ptr = file;
    fseek(file, 0, SEEK_END);
    f_info->file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    f_info->content_type = determine_mime_type(f_info->path);
    }
  else {
    f_info->f_ptr == NULL; f_info->file_size = -1;
  }
}


