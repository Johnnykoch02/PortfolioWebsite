#include "routing.h"
#include "file_io.h"

void serve_route(struct mg_connection *nc, int ev, struct mg_http_message* msg, char* s_route) {
    FILE * route_file = fopen(s_route, "r");
    if (route_file != NULL) {
        if (fseek(route_file, 0, SEEK_END) == 0) {
            long length = ftell(route_file);
            fseek(route_file, 0, SEEK_SET);
            char *content = (char*) malloc(length+1);
            if (content != NULL) {
                fread(content, 1, length, route_file);
                content[length] = '\0'; // Null-terminate the content
                #ifdef __DEBUG__
                printf("Length: %ld\nContent: %s\n", length, content); // Debug print
                #endif
                mg_http_reply(nc, 200, "Content-Type: text/html\r\n", content);
                free(content);
            } else {
                mg_http_reply(nc, 500, "Content-Type: text/plain\r\n", "Failed to allocate memory");
            }
        } else {
            mg_http_reply(nc, 500, "Content-Type: text/plain\r\n", "Failed to seek in file");
        }
        fclose(route_file);
    } else {
        mg_http_reply(nc, 404, "Content-Type: text/plain\r\n\r\n", "File not found");
    }
}

void serve_pnf(struct mg_connection *nc, int ev, struct mg_http_message* msg) {
    mg_http_reply(nc, 404, "Content-Type: text/html\r\n\r\n", "<html><body><h1>404 - Content Not Found.</h1></body></html>");
}

void serve_fbd(struct mg_connection *nc, int ev, struct mg_http_message* msg) {
    mg_http_reply(nc, 401, "Content-Type: text/html\r\n\r\n", "<html><body><h1>401 - Forbidden</h1></body></html>");
}

void link_route(char* s_route) {
    char *cpy = malloc(128); cpy[0] = '\0';
    strcat(cpy, "../");
    strcat(cpy, s_route);
    strcpy(s_route, cpy);
    free(cpy);
}