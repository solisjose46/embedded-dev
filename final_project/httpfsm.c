/*
* httpfsm.c
*
* SER486 Final Project
* Fall 2022
* Author: Jose Solis Salazar
*
* FSM implemented to parse and respond to HTTP requests.
*
*/

#include "httpfsm.h"
#include "vpd.h"
#include "socket.h"
#include "temp.h"
#include "config.h"
#include "log.h"
#include "wdt.h"
#include "rtc.h"
#include "uart.h"

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

static unsigned char reset = 0;

extern config_struct config;

/* int update_tcrit_hi(int value)
 * 
 * Update critical high alarm value.
 *
 * arguments:
 *     int value
 * returns:
 *     0 if success else 1
 */
int update_tcrit_hi(int value){
    if(value > config.hi_warn){
        config.hi_alarm = value;
        config_set_modified();
        config_update();
        return 0;
    }
    return 1;
}

/* int update_twarn_hi(int value)
 * 
 * Update critical high alarm value.
 * 
 * arguments:
 *     int value
 * returns:
 *     0 if success else 1
 */
int update_twarn_hi(int value){
    if(value < config.hi_alarm && value > config.lo_warn){
        config.hi_warn = value;
        config_set_modified();
        config_update();
        return 0;
    }
    return 1;
}

/* int update_tcrit_lo(int value)
 * 
 * Update critical low alarm value.
 * 
 * arguments:
 *     int value
 * returns:
 *     0 if success else 1
 */
int update_tcrit_lo(int value){
    if(value < config.lo_warn){
        config.lo_alarm = value;
        config_set_modified();
        config_update();
        return 0;
    }

    return 1;

}

/* int update_twarn_lo(int value)
 * 
 * Update warning low alarm value.
 * 
 * arguments:
 *     int value
 * returns:
 *     0 if success else 1
 */
int update_twarn_lo(int value){
    if(value > config.lo_alarm && value < config.hi_warn){
        config.lo_warn = value;
        config_set_modified();
        config_update();
        return 0;
    }
    return 1;
}

/* static char * get_temp_state()
 * 
 * Private method for getting temp state based on current temp.
 *
 * arguments:
 *     None
 *
 * returns:
 *     (char *) temp state
 */
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

/* static void http_response_get()
 *
 * Private method for responding to HTTP get request.
 * Response is json of current device state.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 */
static void http_response_get(){
    /* Write HTTP response code, headers and prepare json body */
    socket_writestr(CLIENT_SOCKET, "HTTP/1.1 200 OK\n");

    socket_writestr(CLIENT_SOCKET, "Content-Type: application/vnd.api+json\r\n");
    socket_writestr(CLIENT_SOCKET, "Connection: close\r\n");
    socket_writestr(CLIENT_SOCKET, "\r\n");

    socket_writestr(CLIENT_SOCKET, "{");

    /* Write VPD data */
    socket_writequotedstring(CLIENT_SOCKET, "vpd"); socket_writestr(CLIENT_SOCKET, ":{");
    socket_writequotedstring(CLIENT_SOCKET,"model"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.model); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"manufacturer"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.manufacturer); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"serial_number"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.serial_number); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"manufacture_date"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedate(CLIENT_SOCKET, vpd.manufacture_date); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"mac_address"); socket_writestr(CLIENT_SOCKET, ":"); socket_write_macaddress(CLIENT_SOCKET, vpd.mac_address); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"country_code"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, vpd.country_of_origin);
    socket_writestr(CLIENT_SOCKET, "},");

    /* Write alarm temps */
    socket_writequotedstring(CLIENT_SOCKET,"tcrit_hi"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.hi_alarm); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"twarn_hi"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.hi_warn); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"tcrit_lo"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.lo_alarm); socket_writestr(CLIENT_SOCKET, ",");
    socket_writequotedstring(CLIENT_SOCKET,"twarn_lo"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, config.lo_warn); socket_writestr(CLIENT_SOCKET, ",");

    socket_writequotedstring(CLIENT_SOCKET,"temperature"); socket_writestr(CLIENT_SOCKET, ":"); socket_writedec32(CLIENT_SOCKET, temp_get()); socket_writestr(CLIENT_SOCKET, ",");

    /* Write temp state */
    socket_writequotedstring(CLIENT_SOCKET,"state"); socket_writestr(CLIENT_SOCKET, ":"); socket_writequotedstring(CLIENT_SOCKET, get_temp_state()); socket_writestr(CLIENT_SOCKET, ",");

    /* Write log */
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
    socket_writestr(CLIENT_SOCKET, "\r\n");
    socket_disconnect(CLIENT_SOCKET);
}

/* static void http_response_ok()
 *
 * Private method for replying with HTTP reponse 200.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 */
static void http_response_ok(){
    socket_writestr(CLIENT_SOCKET, "HTTP/1.1 200 OK\n");
    socket_writestr(CLIENT_SOCKET, "Connection: close\r\n");
    socket_writestr(CLIENT_SOCKET, "\r\n");
    socket_disconnect(CLIENT_SOCKET);
}

/* static void send_error_response()
 *
 * Private method for replying with HTTP reposonse 400.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 */
static void send_error_response(){
    socket_writestr(CLIENT_SOCKET, "HTTP/1.1 400 Bad Request\n");
    socket_writestr(CLIENT_SOCKET, "Connection: close\r\n");
    socket_writestr(CLIENT_SOCKET, "\r\n");
    socket_disconnect(CLIENT_SOCKET);
}

/* static unsigned char process_uri()
 *
 * Private method for determining is uri is valid.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     0 if invalid 1 if valid
 */
static unsigned char process_uri(){
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

    /* Bad request */
    if(count>= MAX_CHARS_BEFORE_RESOURCE){
        return 0;
    }

    /* If top level uri not device then bad request */
    if(!socket_recv_compare(CLIENT_SOCKET, "device")){
        return 0;
    }


    return 1;
}

/* static unsigned char process_get()
 *
 * Private method for validating get request.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     0 if invalid 1 if valid
 */
static unsigned char process_get(){
    return process_uri();
}

/* static unsigned char process_put()
 *
 * Private method for validating put request.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     0 if invalid 1 if valid
 */
static unsigned char process_put(){
    if(process_uri() == 0){
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

/* static unsigned char process_delete()
 *
 * Private method for validating delete request.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     0 if invalid 1 if valid
 */
static unsigned char process_delete(){
    if(process_uri() == 0){
        return 0;
    }

    /* log only valid DELETE endpoint */
    if(!socket_recv_compare(CLIENT_SOCKET, "/log")){
        return 0;
    }

    unsigned char buf[2];
    socket_peek(CLIENT_SOCKET, buf);
    if(buf[0] != ' '){
        return 0;
    }

    log_clear();


    return 1;
}

/* static void http_response_put()
 *
 * Private method for replying to HTTP PUT request.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 */
static void http_response_put(){
    if(reset > 0){
        http_response_ok();
        wdt_force_restart();
    }
    else{
        switch(temp_value_to_update){
	    case TCRIT_HI:
                if(update_tcrit_hi(update_temp_value) == 0){
		    http_response_ok();
                }
                else{
                    send_error_response();
                }
                break;
            case TWARN_HI:
                if(update_twarn_hi(update_temp_value) == 0){
                    http_response_ok();
		}
                else{
                    send_error_response();
                }
                break;
            case TCRIT_LO:
                if(update_tcrit_lo(update_temp_value) == 0){
                    http_response_ok();
		}
                else{
                    send_error_response();
                }
                break;
            case TWARN_LO:
                if(update_twarn_lo(update_temp_value) == 0){
                    http_response_ok();
		}
                else{
                    send_error_response();
                }
                break;
        }
    }
}

/* void httpfsm_update()
 *
 * Mehtod for updating HTTP FSM.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 */
void httpfsm_update(){
    static REQUEST_TYPE request_type;
    switch(state){
        case INITIAL_STATE:
	    /* If request not GET, PUT, DELETE then FLUSH */
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
	    /* flush and reset state */
            while(socket_recv_available(CLIENT_SOCKET)){
                socket_flush_line(CLIENT_SOCKET);
            }
            send_error_response();
            state = INITIAL_STATE;
            break;
	/* Validate GET, PUT, DELETE else FLUSH */
        case GET:
            if(process_get() == 0){
                state = FLUSH;
            }
            else{
                state = HTTP_VERSION;
            }
            break;
        case PUT:
            if(process_put() == 0){
                state = FLUSH;
            }
            else{
                state = HTTP_VERSION;
            }
            break;
        case DELETE:
            if(process_delete() == 0){
                state = FLUSH;
            }
            else{
                state = HTTP_VERSION;
            }
            break;
        case HTTP_VERSION:
	    /* Validate HTTP version */
            if(!socket_recv_compare(CLIENT_SOCKET, " HTTP/1.1\r\n")){
                state = FLUSH;
            }
            else {
                state = HEADERS;
            }
            break;
        case HEADERS:
	    /* Validate Headers */
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
                http_response_ok();
            }

            state = INITIAL_STATE;
            break;
    }

}

/* void httpfsm_init()
 *
 * Initializes state to INITIAL_STATE.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 */
void httpfsm_init(){
    state = INITIAL_STATE;
    reset = 0;
}

/* void httpfsm_reset()
 *
 * Resets the fsm by setting state to FLUSH.
 * 
 * arguments:
 *     None
 *
 * returns:
 *     Nothing
 */
void httpfsm_reset(){
    state = FLUSH;
}
