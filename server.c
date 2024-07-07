#include "mongoose.h"
#include "src/Helpers/routing.h"
#include "src/Helpers/file_io.h"
#include "src/Helpers/cache.h"
#include <pthread.h>
#include <string.h>

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
        
        else if (mg_http_match_uri(msg, "/my-resume") || mg_http_match_uri(msg, "/my-resume/")) {
            route_resume(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/static/#")) {
            // struct static_thread_args * args = malloc(sizeof(struct static_thread_args));
            // if (!args) 
                route_static(nc, msg); // Route normally
            // pthread_t tid;
            // args->msg = msg;
            // args->nc = nc;
            // pthread_create(&tid, NULL, threaded_static_routing, (void*) args);
            // pthread_detach(tid);
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
    /* HTTPS Certificates*/
    struct server_certs certs;
    load_certificates(&certs);
    s_tls_ca = certs.s.ca; s_tls_cert = certs.s.cert; s_tls_key = certs.s.key;
    c_tls_ca = certs.c.ca; c_tls_cert = certs.c.cert; c_tls_key = certs.c.key;

    /* Configure API Endpoints: TODO implement programatic API configuration dynamically */
    num_api_endpoints = 1;
    API_ENDPOINTS = (StringKeyValue**) malloc(sizeof(StringKeyValue*) * num_api_endpoints);

    cache = init_cache();

    struct mg_mgr mgr;
    mg_log_set(MG_LL_DEBUG);
    mg_mgr_init(&mgr);  
    mg_http_listen(&mgr, s_http_addr, handle_routes, NULL);  // HTTP 
    mg_http_listen(&mgr, s_https_addr, handle_routes, (void *) 1);  // HTTPS 
    for (;;) mg_mgr_poll(&mgr, 500); 
    mg_mgr_free(&mgr);
    return 0;
}
