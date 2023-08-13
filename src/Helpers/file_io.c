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

char* get_executable_path() {
    char exe_path[512];
    ssize_t len = readlink("/proc/self/exe", exe_path, 512 - 1);
    if (len == -1) {
        return NULL;
    }

    exe_path[len] = '\0';
    char* path = strdup(dirname(exe_path)); // dirname can modify its argument
    return path;
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


