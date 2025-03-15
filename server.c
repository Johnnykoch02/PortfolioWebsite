#include "mongoose.h"
#include "src/Helpers/routing.h"
#include "src/Helpers/file_io.h"
#include "src/Helpers/cache.h"
#include "src/Helpers/strings.h"
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h> // For DT_REG
#include <time.h>

char* BLOG_POSTS_DIR;

#pragma once
static const char *s_url = "https://0.0.0.0:443";
static const char *s_http_addr = "http://0.0.0.0:80";    // HTTP port
static const char *s_https_addr = "https://0.0.0.0:443";  // HTTPS port

char *s_tls_ca;
char *s_tls_cert;
char *s_tls_key;
char *c_tls_ca;
char *c_tls_cert;
char *c_tls_key;

char* server_ca_f = "ca.pem";
char* server_cert_f = "cert.pem";
char* server_key_f = "key.pem";
char* client_ca_f = "client_ca.pem";
char* client_cert_f = "client_cert.ca";
char* client_key_f = "client_key.pem";

char* root_directory;

struct static_thread_args {
    struct mg_connection* nc;
    struct mg_http_message* msg;
};

typedef struct char_ptr_key_value {
    char* key;
    char* value;
} StringKeyValue;

StringKeyValue* newKeyValue(const char* key, const char* value) {
    StringKeyValue* n_val = (StringKeyValue*) malloc(sizeof(StringKeyValue));
    size_t len_key = strlen(key) + 1;
    size_t len_val = strlen(value) + 1;
    n_val->key = (char*) malloc(len_key);
    n_val->value = (char*) malloc(len_val);
    strncpy(n_val->key, key, len_key);
    strncpy(n_val->value, value, len_val);
    return n_val;
}

size_t num_api_endpoints;
StringKeyValue** API_ENDPOINTS;
LRUCache* cache;

// Structure for blog post data
typedef struct blog_post {
    char* filename;
    char* content;
    time_t creation_date;
    size_t content_size;
} BlogPost;

// Global array of blog posts
BlogPost** blog_posts = NULL;
int blog_post_count = 0;

// Function to load all blog posts from the static/blog_posts directory
void load_blog_posts() {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stats;
    char path[1024];
    
    // Free existing posts if they exist
    if (blog_posts != NULL) {
        for (int i = 0; i < blog_post_count; i++) {
            if (blog_posts[i]->filename != NULL) free(blog_posts[i]->filename);
            if (blog_posts[i]->content != NULL) free(blog_posts[i]->content);
            free(blog_posts[i]);
        }
        free(blog_posts);
        blog_posts = NULL;
        blog_post_count = 0;
    }
    
    // Open the blog posts directory
    dir = opendir(BLOG_POSTS_DIR);
    if (dir == NULL) {
        printf("Error: Could not open blog posts directory\n");
        return;
    }
    
    // First, count the number of files
    while ((entry = readdir(dir)) != NULL) {
        char full_path[1024];
        sprintf(full_path, "static/blog_posts/%s", entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode) && strstr(entry->d_name, ".md") != NULL) {
            blog_post_count++;
        }
    }
    
    // Allocate memory for blog posts array
    blog_posts = (BlogPost**)malloc(sizeof(BlogPost*) * blog_post_count);
    
    // Reset directory pointer
    rewinddir(dir);
    
    // Load each blog post
    int index = 0;
    while ((entry = readdir(dir)) != NULL) {
        char full_path[1024];
        sprintf(full_path, "static/blog_posts/%s", entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode) && strstr(entry->d_name, ".md") != NULL) {
            // Get file stats for creation date
            if (stat(full_path, &file_stats) != 0) {
                continue;
            }
            
            // Create a new blog post
            BlogPost* post = (BlogPost*)malloc(sizeof(BlogPost));
            post->filename = strdup(entry->d_name);
            post->creation_date = file_stats.st_ctime;
            
            // Read file content
            FILE *file = fopen(full_path, "rb");
            if (file == NULL) {
                free(post->filename);
                free(post);
                continue;
            }
            
            // Get file size
            fseek(file, 0, SEEK_END);
            size_t file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            // Read content
            post->content = (char*)malloc(file_size + 1);
            fread(post->content, 1, file_size, file);
            post->content[file_size] = '\0';
            post->content_size = file_size;
            
            fclose(file);
            
            blog_posts[index++] = post;
        }
    }
    
    closedir(dir);
    printf("Loaded %d blog posts\n", blog_post_count);
}

// Route handler for individual blog posts
static void route_blog_post(struct mg_connection *nc, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
        // Extract the filename from the URI
        char filename[1024] = {0};
        if (sscanf((const char*)msg->uri.ptr, "/blog_post/%255s", filename) != 1) {
            serve_pnf(nc, 0, msg);
            return;
        }
        
        // Look for the post in our array
        for (int i = 0; i < blog_post_count; i++) {
            if (strcmp(blog_posts[i]->filename, filename) == 0) {
                // Send the markdown content
                mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: text/markdown\r\nContent-Length: %ld\r\n\r\n", 
                          blog_posts[i]->content_size);
                mg_send(nc, blog_posts[i]->content, blog_posts[i]->content_size);
                return;
            }
        }
        
        // Post not found
        serve_pnf(nc, 0, msg);
    }
}

static char* realloc_cat_free(char* a, char* b, size_t size_of_a) {
    char* dest = (char*) realloc(a, strlen(a) + strlen(b) + 1);
    strcat(dest, b);
    if (dest != a) free(a);
    return dest;
}

// Route handler for getting all blog posts as JSON
static void route_blog_posts(struct mg_connection *nc, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
        // Build JSON response
        SString* json = sstring_malloc(1024 * 8); // Malloc 8KB for JSON to start with
        sstring_cat(json, "[");
        
        for (int i = 0; i < blog_post_count; i++) {
            SString* post_json = sstring_malloc(4096);
            char date_str[64];
            struct tm *tm_info = localtime(&blog_posts[i]->creation_date);
            strftime(date_str, sizeof(date_str), "%Y-%m-%dT%H:%M:%S", tm_info);
            
            sprintf(post_json->str, "%s{\"filename\":\"%s\",\"date\":\"%s\",\"content\":", 
            (i > 0 ? "," : ""), 
            blog_posts[i]->filename, 
            date_str);

            sstring_cat(json, post_json->str);
            // Add content as JSON string (need to escape special chars)
            sstring_cat(json, "\"");
            
            // Simple escaping of quotes and backslashes in content
            size_t escaped_content_size = blog_posts[i]->content_size * 2;
            SString* escaped_content = sstring_malloc(escaped_content_size);
            char* p = escaped_content->str;
            for (size_t j = 0; j < blog_posts[i]->content_size; j++) {
                char c = blog_posts[i]->content[j];
                if (c == '\"' || c == '\\') {
                    *p++ = '\\';
                }
                if (c == '\n') {
                    *p++ = '\\';
                    *p++ = 'n';
                } else if (c == '\r') {
                    *p++ = '\\';
                    *p++ = 'r';
                } else {
                    *p++ = c;
                }
            }
            *p = '\0';

            sstring_cat(json, escaped_content->str);
            sstring_cat(json, "\"}");
            sstring_free(escaped_content);
            sstring_free(post_json);
        }

        
        sstring_cat(json, "]");
        size_t json_size = strlen(json->str);
        
        // Send JSON response
        mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %ld\r\n\r\n", 
                  json_size);
        mg_send(nc, json->str, json_size);
        printf("Sent blog posts: %s\n", json->str);
        sstring_free(json);
    }
}

static void route_blog(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(1048); route[0] = '\0';
      strcat(route, "static/Blog/index.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");
}

static void route_static(struct mg_connection* nc, struct mg_http_message* msg);

static void* threaded_static_routing(void* args) {
    struct static_thread_args* static_args = (struct static_thread_args*) args;
    route_static(static_args->nc, static_args->msg);
    free(static_args);
    return NULL;
}

static void redirect_to_home(struct mg_connection *nc) {
  const char *html_redirect = "<!DOCTYPE html>"
                                        "<html lang=\"en\">"
                                        "<head><meta charset=\"UTF-8\"></head>"
                                        "<body>"
                                        "<script>"
                                        "setTimeout(function() {"
                                        "  window.location.href = '/home';"
                                        "}, 500);" // 500-ms delay
                                        "</script>"
                                        "</body>"
                                        "</html>\r\n\r\n";
            mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
            mg_send(nc, html_redirect, strlen(html_redirect));
}

static void route_home(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(1048); route[0] = '\0';
      strcat(route, "static/Home/index.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");
}

static void route_about(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(1048); route[0] = '\0';
      strcat(route, "static/About/about-me.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");

}

static void route_projects(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(1048); route[0] = '\0';
      strcat(route, "static/Research-Projects/research-projects.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");
}

static void route_videos(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(1048); route[0] = '\0';
      strcat(route, "static/Videos-I-Like/videos-i-like.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");
}

static void route_resume(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
        char* path = malloc(1048); 
        char* path_base_address = path;
        strcpy(path, "static/data/content/personal/Jonathan_Koch_Resume.pdf");
        struct http_file_info f_info;
        get_http_file_from_path(&f_info,  path);
        if (f_info.f_ptr != NULL) {
            mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\nContent-Disposition: attachment; filename=\"Jonathan_Koch_Resume.pdf\"\r\n\r\n", f_info.content_type, f_info.file_size);
            char buffer[4096];
            size_t n;
            while ((n = fread(buffer, 1, sizeof(buffer), f_info.f_ptr )) > 0) {
                mg_send(nc, buffer, n);
            }
        fclose(f_info.f_ptr);
        f_info.path = NULL;
        }
        else {
            serve_pnf(nc, 0, msg);
        }
        free(path_base_address);
        mg_printf(nc, "\r\n\r\n");
        redirect_to_home(nc); /* After sending the file redirect home route.*/
    }
}

static void route_static(struct mg_connection *nc, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
        char* path = malloc(1048); 
        path[0] = '\0';
        struct http_file_info f_info;
        f_info.path = path;
        get_http_file_from_uri(&f_info, msg);
        if (f_info.f_ptr) {
            char* cached_data = get_from_cache(cache, f_info.path);
            if (cached_data) {
                mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", f_info.content_type, f_info.file_size);
                mg_send(nc, cached_data, f_info.file_size);
            } else  {
                mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", f_info.content_type, f_info.file_size);
                
                char* file_data = malloc(f_info.file_size);
                fread(file_data, 1, f_info.file_size, f_info.f_ptr);
                mg_send(nc, file_data, f_info.file_size);

                add_to_cache(cache, f_info.path, file_data, f_info.file_size);

                fclose(f_info.f_ptr);
            }    
        } 
        else {
            serve_pnf(nc, 0, msg);
        }
        free(path);  
    }  
    mg_printf(nc, "\r\n\r\n");
}

static void route_apis(struct mg_connection *nc, struct mg_http_message* msg) {
    //  TODO: Implement APIs route
    // if (mg_vcmp(&msg->method, "GET") == 0) {
    //     char* path = malloc(1048); 
    //     path[0] = '\0';
    //     struct http_file_info f_info;
    //     f_info.path = path;
    //     get_http_file_from_uri(&f_info, msg);
    //     if (f_info.f_ptr != NULL) {
    //         // if (! is_subdir_of_root(f_info.path, root_directory)) return; /* No (or at least I hope) illegal path traverals */
    //         mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", f_info.content_type, f_info.file_size);
    //         char buffer[512];
    //         size_t n;
    //         while ((n = fread(buffer, 1, sizeof(buffer), f_info.f_ptr )) > 0) {
    //             mg_send(nc, buffer, n);
    //         }
    //     fclose(f_info.f_ptr);
    //     }
    //     else {
    //         serve_pnf(nc, 0, msg);
    //     }
    //     free(path);  
    // }  
    // mg_printf(nc, "\r\n\r\n");
} 

static void handle_routes(struct mg_connection * nc, int ev, void* ev_data, void* fn_data) {
    /**ROUTING*/
    struct mg_http_message *msg = (struct mg_http_message *) ev_data;
    if (msg == NULL) return;

    struct mg_tls_opts opts = {
            // .server_ca = mg_str(s_tls_ca),
            .server_cert = mg_str(s_tls_cert),
            .server_key = mg_str(s_tls_key),
            // .client_ca = mg_str(c_tls_ca),
            .client_cert = mg_str(c_tls_cert),
            .client_key = mg_str(c_tls_key)
        };
    if (ev != MG_EV_POLL) mg_tls_ctx_init(nc->mgr, &opts);
    if (ev == MG_EV_ACCEPT) {
        printf("HTTP Secure");
        mg_tls_init(nc, mg_str(s_url));
    }
    else if (ev == MG_EV_HTTP_MSG) {
        printf("%s", msg->uri);
        if (msg->uri.len > 512) redirect_to_home(nc); /* don't allow weird requests */
        if (mg_http_match_uri(msg, "/home") || mg_http_match_uri(msg, "/home/#")) {
            route_home(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/about") || mg_http_match_uri(msg, "/about/#")) {
            route_about(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/videos-i-like") || mg_http_match_uri(msg, "/videos-i-like/#")) {
            route_videos(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/research-projects") || mg_http_match_uri(msg, "/research-projects/#")) {
            route_projects(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/blog") || mg_http_match_uri(msg, "/blog/#")) {
            route_blog(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/blog_posts")) {
            route_blog_posts(nc, msg);
        }
        else if (mg_http_match_uri(msg, "/blog_post/*")) {
            route_blog_post(nc, msg);
        }
        else if (mg_http_match_uri(msg, "/my-resume") || mg_http_match_uri(msg, "/my-resume/")) {
            route_resume(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/static/#")) {
            route_static(nc, msg); // Route normally
        }
        else {
            route_home(nc, ev, ev_data, msg);
        }
        nc->is_draining = 1;
    }
    if (ev == MG_EV_ACCEPT || ev == MG_EV_HTTP_MSG) {
        FILE *logFile = fopen("/home/server.log", "a");
        if (logFile != NULL) {
            // Get IP address of client
            uint8_t* ip_ptr = nc->rem.ip;
            fprintf(logFile, "Received connection from: %d.%d.%d.%d\n", ip_ptr[0], ip_ptr[1], ip_ptr[2], ip_ptr[3]);
            fclose(logFile);
        } else {
            perror("Could not open log file");
        }
    }
    (void) ev_data;
}

int main(int argc, char** args) {
    root_directory = args[1];
    BLOG_POSTS_DIR = malloc(1048);
    // Get the root directory from the file path to this executable
    char executable_path[1024] = {0};
    ssize_t len = readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);
    if (len != -1) {
        executable_path[len] = '\0';
        // Find the last slash to get the directory
        char* last_slash = strrchr(executable_path, '/');
        if (last_slash != NULL) {
            *last_slash = '\0'; // Truncate at the last slash to get directory
            printf("Executable directory: %s\n", executable_path);
        } else {
            printf("Could not determine executable directory, using current directory\n");
            strcpy(executable_path, ".");
        }
    } else {
        perror("readlink failed");
        strcpy(executable_path, ".");
    }
    
    // If root_directory is not provided as argument, use the executable directory
    if (argc < 2 || args[1] == NULL) {
        root_directory = executable_path;
        printf("Using executable directory as root: %s\n", root_directory);
    }
    BLOG_POSTS_DIR[0] = '\0';
    strcat(BLOG_POSTS_DIR, root_directory);
    strcat(BLOG_POSTS_DIR, "/static/static/blog_posts");
    /* HTTPS Certificates*/
    struct server_certs certs;
    load_certificates(&certs);
    s_tls_ca = certs.s.ca; s_tls_cert = certs.s.cert; s_tls_key = certs.s.key;
    c_tls_ca = certs.c.ca; c_tls_cert = certs.c.cert; c_tls_key = certs.c.key;

    /* Configure API Endpoints: TODO implement programatic API configuration dynamically */
    num_api_endpoints = 1;
    API_ENDPOINTS = (StringKeyValue**) malloc(sizeof(StringKeyValue*) * num_api_endpoints);

    cache = init_cache();

    // Load blog posts
    load_blog_posts();

    struct mg_mgr mgr;
    mg_log_set(MG_LL_DEBUG);
    mg_mgr_init(&mgr);  
    mg_http_listen(&mgr, s_http_addr, handle_routes, NULL);  // HTTP 
    mg_http_listen(&mgr, s_https_addr, handle_routes, (void *) 1);  // HTTPS 
    for (;;) mg_mgr_poll(&mgr, 500); 
    mg_mgr_free(&mgr);
    return 0;
}
