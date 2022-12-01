#include "httpfsm.h"
#include "vpd.h"
#include "socket.h"
#include "temp.h"
#include "config.h"
#include "log.h"
#include "wdt.h"
#include "rtc.h"
#include "uart.h"

// TODO: rename private methods and add comment blocks

/* Possible FSM states */
typedef enum {
    INITIAL_STATE,
    FLUSH,
    HEADERS,
    GET,
    PUT,
    DELETE,
    HTTP_VERSION,
    PROCESS_MESSAGE
} HTTP_FSM_STATE;

typedef enum {
    TCRIT_HI,
    TWARN_HI,
    TCRIT_LO,
    TWARN_LO
} UPDATE_TEMP;

/* Socket for client requests */
#define CLIENT_SOCKET 0
/* For readability */
#define REQUEST_TYPE HTTP_FSM_STATE

/* state of the fsm */
static HTTP_FSM_STATE state;

/* Temp value to update */
static UPDATE_TEMP temp_value_to_update;

/* The value received in the put request for temp update */
static int update_temp_value;

/* variable to determine whether or not to reset device after http request has been verified. */
static unsigned char reset = 0;

extern config_struct config;

int update_tcrit_hi(int value){
    if(value > config.hi_warn){
        config.hi_alarm = value;
        config_set_modified();
        config_update();
        return 0;
    }
    return 1;
}

int update_twarn_hi(int value){
    if(value < config.hi_alarm && value > config.lo_warn){
        config.hi_warn = value;
        config_set_modified();
        config_update();
        return 0;
    }
    return 1;
}

int update_tcrit_lo(int value){
    if(value < config.lo_warn){
        config.lo_alarm = value;
        config_set_modified();
        config_update();
        return 0;
    }

    return 1;

}

int update_twarn_lo(int value){
    if(value > config.lo_alarm && value < config.hi_warn){
        config.lo_warn = value;
        config_set_modified();
        config_update();
        return 0;
    }
    return 1;
}


// Private method for getting temp state based on current temp.
static char * get_temp_state(){
    int temp = temp_get();
    if(temp >= config.hi_alarm){
        return "CRIT_HI";
    }
    else if(temp >= config.hi_warn){
        return "WARN_HI";
    }
    else if(temp <= config.lo_alarm){
        return  "CRIT_LO";
    }
    else if(temp <= config.lo_warn){
        return  "WARN_LO";
    }
    else {
        return  "NORMAL";
    }
}

// Private method for responding to HTTP get request. Response is json of current device state.

static void http_response_get(){
    // write http response code
    socket_writestr(CLIENT_SOCKET, "HTTP/1.1 200 OK\n");

    //write headers
    socket_writestr(CLIENT_SOCKET, "Content-Type: application/vnd.api+json\r\n");
    socket_writestr(CLIENT_SOCKET, "Connection: close\r\n");
    socket_writestr(CLIENT_SOCKET, "\r\n");

    // write json body
    socket_writestr(CLIENT_SOCKET, "{");

    // write the vpd data
    socket_writequotedstring(CLIENT_SOCKET, "vpd"); socket_writestr(CLIENT_SOCKET, ":{");
    socket_writequotedstring(CLIENT_SOCKET,"model"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.model); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"manufacturer"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.manufacturer); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"serial_number"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.serial_number); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"manufacture_date"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedate(CLIENT_SOCKET, vpd.manufacture_date); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"mac_address"); socket_writestr(CLIENT_SOCKET, ":"); socket_write_macaddress(CLIENT_SOCKET, vpd.mac_address); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"country_code"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.country_of_origin);
    socket_writestr(CLIENT_SOCKET, "},");

    // write the temp alarm values
    socket_writequotedstring(CLIENT_SOCKET,"tcrit_hi"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.hi_alarm); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"twarn_hi"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.hi_warn); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"tcrit_lo"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.lo_alarm); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"twarn_lo"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.lo_warn); socket_writestr(CLIENT_SOCKET, ",");

    // write current temp
    socket_writequotedstring(CLIENT_SOCKET,"temperature"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, temp_get()); socket_writestr(CLIENT_SOCKET, ",");

    //write state
    socket_writequotedstring(CLIENT_SOCKET,"state"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, get_temp_state()); socket_writestr(CLIENT_SOCKET, ",");

    // write log
    socket_writequotedstring(CLIENT_SOCKET,"log"); socket_writestr(CLIENT_SOCKET, ":[");

    for(int i = 0; i < log_get_num_entries(); i++){
        unsigned long timestamp;
        unsigned char event ;
        log_get_record(i, &timestamp, &event);
        socket_writestr(CLIENT_SOCKET, "{");
        socket_writequotedstring(CLIENT_SOCKET,"timestamp"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, rtc_num2datestr(timestamp)); socket_writestr(CLIENT_SOCKET, ",");

        socket_writequotedstring(CLIENT_SOCKET,"event"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, event);
        socket_writestr(CLIENT_SOCKET, "}");

        if(i != log_get_num_entries() - 1){
            socket_writestr(CLIENT_SOCKET, ",");
        }
    }

    socket_writestr(CLIENT_SOCKET, "]}");

    //write final CRLF
    socket_writestr(CLIENT_SOCKET, "\r\n");

    //disconnect the socket
    socket_disconnect(CLIENT_SOCKET);
}

// Private method for replying with HTTP reponse 200.
static void http_response_delete(){
    socket_writestr(CLIENT_SOCKET, "HTTP/1.1 200 OK\n");
    socket_writestr(CLIENT_SOCKET, "Connection: close\r\n");
    socket_writestr(CLIENT_SOCKET, "\r\n");
    socket_disconnect(CLIENT_SOCKET);
}

// Private method for replying with HTTP reposonse 400.
static void httpfsm_send_error_response(){
    socket_writestr(CLIENT_SOCKET, "HTTP/1.1 400 Bad Request\n");
    socket_writestr(CLIENT_SOCKET, "Connection: close\r\n");
    socket_writestr(CLIENT_SOCKET, "\r\n");
    socket_disconnect(CLIENT_SOCKET);
}



// Private method for determining is uri is valid.
// 0 if invalid 1 if valid
static unsigned char httpfsm_process_uri(){
    unsigned char MAX_CHARS_BEFORE_RESOURCE = 30; 
    unsigned char count = 0;
    unsigned char buf[1];
    while(count < MAX_CHARS_BEFORE_RESOURCE){
        socket_recv(CLIENT_SOCKET, buf, 1);
        if(buf[0] == '/'){
            break;
        }
        count++;
    }
    if(count>= MAX_CHARS_BEFORE_RESOURCE){
        // too many characters before resource return 0 indicating a bad request
        return 0;
    }

    if(!socket_recv_compare(CLIENT_SOCKET, "device")){
        // device is the only valid top level resource. If it is anything else this is a bad request
        return 0;
    }


    return 1;
}

// Private method for validating get request.
// 0 if invalid 1 if valid.
static unsigned char httpfsm_process_get(){
    return httpfsm_process_uri();
}

// Private method for validating put request.
// 0 if invalid 1 if valid.
static unsigned char httpfsm_process_put(){
    if(httpfsm_process_uri() == 0){
        return 0;
    }

    if(socket_recv_compare(CLIENT_SOCKET, "/config?")){
        if(socket_recv_compare(CLIENT_SOCKET, "tcrit_hi=")){
            uart_writestr("Updating tcrit_hi with value: ");
            temp_value_to_update = TCRIT_HI;
        }
        else if (socket_recv_compare(CLIENT_SOCKET, "twarn_hi=")){
            uart_writestr("Updating twarn_hi with value: ");
            temp_value_to_update = TWARN_HI;
        }
        else if (socket_recv_compare(CLIENT_SOCKET, "tcrit_lo=")){
            uart_writestr("Updating tcrit_lo with value: ");
            temp_value_to_update = TCRIT_LO;
        }
        else if (socket_recv_compare(CLIENT_SOCKET, "twarn_lo=")){
            uart_writestr("Updating twarn_lo with value: ");
            temp_value_to_update = TWARN_LO;
        }
        else{
            return 0;
        }

        if(!socket_recv_int(CLIENT_SOCKET, &update_temp_value)){
            return 0;
        }

        uart_writedec32((long)update_temp_value);
        uart_writestr("\r\n");

        return 1;
    }
    else if(socket_recv_compare(CLIENT_SOCKET, "?reset=")){
        if(socket_recv_compare(CLIENT_SOCKET, "\"false\"")){
            reset = 0;
        }
        else if(socket_recv_compare(CLIENT_SOCKET, "\"true\"")){
            reset = 1;
        }
        else{
            return 0;
        }
        return 1;
    }

    return 0;
}

/**
httpfsm_process_delete()

returns
    0 if the request is bad
    1 otherwise
*/
static unsigned char httpfsm_process_delete(){
    if(httpfsm_process_uri() == 0){
        return 0;
    }

    if(!socket_recv_compare(CLIENT_SOCKET, "/log")){
        // the only valid resources for a delete command is log.
        // If this is not the case it is a bad request
        return 0;
    }

    unsigned char buf[2];
    socket_peek(CLIENT_SOCKET, buf);
    if(buf[0] != ' '){
        // there must be a space after /log otherwise an invalid uri was passed in for a delete request.
        return 0;
    }

    log_clear();


    return 1;
}

// Private method for replying to HTTP PUT request.
static void http_response_put(){
    if(reset > 0){
        http_response_delete();
        wdt_force_restart();
    }
    else{
        switch(temp_value_to_update){
        case TCRIT_HI:
            if(update_tcrit_hi(update_temp_value) == 0){
                http_response_delete();
            }
            else{
                httpfsm_send_error_response();
            }
            break;
        case TWARN_HI:
            if(update_twarn_hi(update_temp_value) == 0){
                http_response_delete();
            }
            else{
                httpfsm_send_error_response();
            }
            break;
        case TCRIT_LO:
            if(update_tcrit_lo(update_temp_value) == 0){
                http_response_delete();
            }
            else{
                httpfsm_send_error_response();
            }
            break;
        case TWARN_LO:
            if(update_twarn_lo(update_temp_value) == 0){
                http_response_delete();
            }
            else{
                httpfsm_send_error_response();
            }
            break;
        }
    }
}

// Mehtod for updating HTTP FSM.
void httpfsm_update(){
    static REQUEST_TYPE request_type;
    switch(state){
        case INITIAL_STATE:
            if(socket_recv_compare(CLIENT_SOCKET, "GET")){
                request_type = GET;
                state = GET;
            }
            else if(socket_recv_compare(CLIENT_SOCKET, "PUT")){
                request_type = PUT;
                state = PUT;
            }
            else if(socket_recv_compare(CLIENT_SOCKET, "DELETE")){
                request_type = DELETE;
                state = DELETE;
            }
            else{
                state = FLUSH;
            }
            break;
        case FLUSH:
            while(socket_recv_available(CLIENT_SOCKET)){
                socket_flush_line(CLIENT_SOCKET);
            }
            httpfsm_send_error_response();
            state = INITIAL_STATE;
            break;
        case GET:
            if(httpfsm_process_get() == 0){
                state = FLUSH;
            }
            else{
                state = HTTP_VERSION;
            }
            break;
        case PUT:
            if(httpfsm_process_put() == 0){
                state = FLUSH;
            }
            else{
                state = HTTP_VERSION;
            }
            break;
        case DELETE:
            if(httpfsm_process_delete() == 0){
                state = FLUSH;
            }
            else{
                state = HTTP_VERSION;
            }
            break;
        case HTTP_VERSION:
            if(!socket_recv_compare(CLIENT_SOCKET, " HTTP/1.1\r\n")){
                state = FLUSH;
            }
            else {
                state = HEADERS;
            }
            break;
        case HEADERS:
            while(1){
                if(socket_is_blank_line(CLIENT_SOCKET)){
                    break;
                }
                socket_flush_line(CLIENT_SOCKET);
            }
            state = PROCESS_MESSAGE;
            break;
        case PROCESS_MESSAGE:
            if(request_type == GET){
                http_response_get();
            }
            else if (request_type == PUT){
                http_response_put();
            }
            else if (request_type == DELETE){
                http_response_delete();
            }

            state = INITIAL_STATE;
            break;
    }

}

// Initializes state to INITIAL_STATE.
void httpfsm_init(){
    state = INITIAL_STATE;
    reset = 0;
}

// Resets the fsm by setting state to FLUSH.
void httpfsm_reset(){
    state = FLUSH;
}
