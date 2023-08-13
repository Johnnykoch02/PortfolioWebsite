#include "./mongoose.h"
#include "src/Helpers/routing.h"
#include "src/Helpers/file_io.h"

static const char *s_http_addr = "http://localhost:8000";    // HTTP port
static const char *s_https_addr = "https://localhost:8443";  // HTTPS port
#ifdef TLS_TWOWAY
static const char *s_tls_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBqjCCAU+gAwIBAgIUESoOPGqMhf9uarzblVFwzrQweMcwCgYIKoZIzj0EAwIw\n"
    "RDELMAkGA1UEBhMCSUUxDzANBgNVBAcMBkR1YmxpbjEQMA4GA1UECgwHQ2VzYW50\n"
    "YTESMBAGA1UEAwwJVGVzdCBSb290MCAXDTIwMDUwOTIxNTE0NFoYDzIwNTAwNTA5\n"
    "MjE1MTQ0WjBEMQswCQYDVQQGEwJJRTEPMA0GA1UEBwwGRHVibGluMRAwDgYDVQQK\n"
    "DAdDZXNhbnRhMRIwEAYDVQQDDAlUZXN0IFJvb3QwWTATBgcqhkjOPQIBBggqhkjO\n"
    "PQMBBwNCAAQsq9ECZiSW1xI+CVBP8VDuUehVA166sR2YsnJ5J6gbMQ1dUCH/QvLa\n"
    "dBdeU7JlQcH8hN5KEbmM9BnZxMor6ussox0wGzAMBgNVHRMEBTADAQH/MAsGA1Ud\n"
    "DwQEAwIBrjAKBggqhkjOPQQDAgNJADBGAiEAnHFsAIwGQQyRL81B04dH6d86Iq0l\n"
    "fL8OKzndegxOaB0CIQCPwSIwEGFdURDqCC0CY2dnMrUGY5ZXu3hHCojZGS7zvg==\n"
    "-----END CERTIFICATE-----\n";
#endif
static const char *s_tls_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBhzCCASygAwIBAgIUbnMoVd8TtWH1T09dANkK2LU6IUswCgYIKoZIzj0EAwIw\n"
    "RDELMAkGA1UEBhMCSUUxDzANBgNVBAcMBkR1YmxpbjEQMA4GA1UECgwHQ2VzYW50\n"
    "YTESMBAGA1UEAwwJVGVzdCBSb290MB4XDTIwMDUwOTIxNTE0OVoXDTMwMDUwOTIx\n"
    "NTE0OVowETEPMA0GA1UEAwwGc2VydmVyMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcD\n"
    "QgAEkuBGnInDN6l06zVVQ1VcrOvH5FDu9MC6FwJc2e201P8hEpq0Q/SJS2nkbSuW\n"
    "H/wBTTBaeXN2uhlBzMUWK790KKMvMC0wCQYDVR0TBAIwADALBgNVHQ8EBAMCA6gw\n"
    "EwYDVR0lBAwwCgYIKwYBBQUHAwEwCgYIKoZIzj0EAwIDSQAwRgIhAPo6xx7LjCdZ\n"
    "QY133XvLjAgVFrlucOZHONFVQuDXZsjwAiEAzHBNligA08c5U3SySYcnkhurGg50\n"
    "BllCI0eYQ9ggp/o=\n"
    "-----END CERTIFICATE-----\n";

static const char *s_tls_key =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQglNni0t9Dg9icgG8w\n"
    "kbfxWSS+TuNgbtNybIQXcm3NHpmhRANCAASS4EacicM3qXTrNVVDVVys68fkUO70\n"
    "wLoXAlzZ7bTU/yESmrRD9IlLaeRtK5Yf/AFNMFp5c3a6GUHMxRYrv3Qo\n"
    "-----END PRIVATE KEY-----\n";

static void redirect_to_home(struct mg_connection *nc) {
  const char *html_redirect = "<!DOCTYPE html>"
                                        "<html lang=\"en\">"
                                        "<head><meta charset=\"UTF-8\"></head>"
                                        "<body>"
                                        "<script>"
                                        "setTimeout(function() {"
                                        "  window.location.href = '/home';"
                                        "}, 5000);" // 5-second delay
                                        "</script>"
                                        "</body>"
                                        "</html>\r\n\r\n";
            mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
            mg_send(nc, html_redirect, strlen(html_redirect));
}

static void route_home(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(512); route[0] = '\0';
      strcat(route, "static/Home/index.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");
}

static void route_about(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(512); route[0] = '\0';
      strcat(route, "static/About/about-me.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");

}

static void route_projects(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
      char* route = malloc(512); route[0] = '\0';
      strcat(route, "static/Research-Projects/research-projects.html"); 
      serve_route(nc, ev, msg, route);
    }
    mg_printf(nc, "\r\n\r\n");
}

static void route_resume(struct mg_connection *nc, int ev, void *ev_data, struct mg_http_message* msg) {
    if (mg_vcmp(&msg->method, "GET") == 0) {
        char* path = malloc(512); 
        char* path_base_address = path;
        strcpy(path, "static/data/content/personal/Jonathan_Koch_Resume.pdf");
        struct http_file_info f_info;
        get_http_file_from_path(&f_info,  path);
        if (f_info.f_ptr != NULL) {
            mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\nContent-Disposition: attachment; filename=\"Jonathan_Koch_Resume.pdf\"\r\n\r\n", f_info.content_type, f_info.file_size);
            char buffer[1024];
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
        char* path = malloc(512); 
        path[0] = '\0';
        struct http_file_info f_info;
        f_info.path = path;
        get_http_file_from_uri(&f_info, msg);
        if (f_info.f_ptr != NULL) {
            mg_printf(nc, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", f_info.content_type, f_info.file_size);
            char buffer[1024];
            size_t n;
            while ((n = fread(buffer, 1, sizeof(buffer), f_info.f_ptr )) > 0) {
                mg_send(nc, buffer, n);
            }
        fclose(f_info.f_ptr);
        }
        else {
            serve_pnf(nc, 0, msg);
        }
        printf("FILE TYPE: %s", f_info.content_type);
        free(path);  
    }  
    mg_printf(nc, "\r\n\r\n");
} 


static void handle_routes(struct mg_connection * nc, int ev, void* ev_data, void* fn_data) {
    /**ROUTING*/
    struct mg_http_message *msg = (struct mg_http_message *) ev_data;
    if (msg == NULL) return;
    if (ev == MG_EV_HTTP_MSG) {
        printf("%s", msg->uri);
        if (mg_http_match_uri(msg, "/home") || mg_http_match_uri(msg, "/home/#")) {
            route_home(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/about") || mg_http_match_uri(msg, "/about/#")) {
            route_about(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/research-projects") || mg_http_match_uri(msg, "/research-projects/#")) {
            route_projects(nc, ev, ev_data, msg);
        }
        
        else if (mg_http_match_uri(msg, "/my-resume") || mg_http_match_uri(msg, "/my-resume/")) {
            route_resume(nc, ev, ev_data, msg);
        }
        else if (mg_http_match_uri(msg, "/static/#")) {
            route_static(nc, msg);
        }
        else {
            redirect_to_home(nc);
        }
    }
    (void) ev_data;
}

int main(int argc, char** args) {
  struct mg_mgr mgr;
  mg_log_set(MG_LL_DEBUG);
//     struct mg_tls_opts opts = {
//     #ifdef TLS_TWOWAY
//                              .client_ca = mg_str(s_tls_ca),
//     #endif
//                              .server_cert = mg_str(s_tls_cert),
//                              .server_key = mg_str(s_tls_key)};
//   mg_tls_ctx_init(&mgr, &opts);
  mg_mgr_init(&mgr);  
  mg_http_listen(&mgr, s_http_addr, handle_routes, NULL);  // Create HTTP listener
  mg_http_listen(&mgr, s_https_addr, handle_routes, (void *) 1);  // HTTPS listener
  for (;;) mg_mgr_poll(&mgr, 1000); 
  mg_mgr_free(&mgr);
  return 0;
  return 0;
}