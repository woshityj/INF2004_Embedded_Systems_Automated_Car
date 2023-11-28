#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define TCP_PORT 4242
#define DEBUG_printf printf
#define BUF_SIZE 2048
#define POLL_TIME_S 20

// WIFI Credentials 
const char WIFI_SSID[] = "yujie";
const char WIFI_PASSWORD[] = "tyjtyc84";

/*!
*	@brief      Struct to store info such as TCP server and client Socket,
*               flag indicating completion of communication,
*               message and reply buffer, length of message and reply
*/

typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    struct tcp_pcb *client_pcb;
    bool complete;
    char buffer_sent[BUF_SIZE];
    char buffer_recv[BUF_SIZE];
    int sent_len;
    int recv_len;
} TCP_SERVER_T;


/*!
*	@brief      Function that initializes a new TCP server state,
*               allocating memory for the state structure and returns a pointer
*               If state cannot be allocated function returns NULL
*/
static TCP_SERVER_T* tcp_server_init(void) {
    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    return state;
}

/*!
*	@brief      Function that closes TCP server connection
*               When called, checks if there is a client connected
*               If there is it sets all callbacks, 
*               client pcb and server pcb to NULL and closes the server
*/
static err_t tcp_server_close(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    err_t err = ERR_OK;
    if (state->client_pcb != NULL) {
        tcp_arg(state->client_pcb, NULL);
        tcp_poll(state->client_pcb, NULL, 0);
        tcp_sent(state->client_pcb, NULL);
        tcp_recv(state->client_pcb, NULL);
        tcp_err(state->client_pcb, NULL);
        err = tcp_close(state->client_pcb);
        if (err != ERR_OK) {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(state->client_pcb);
            err = ERR_ABRT;
        }
        state->client_pcb = NULL;
    }
    if (state->server_pcb) {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
    return err;
}

/*!
*	@brief          Function to close TCP client connection
*                   Checks for connected client, if there is
*                   Sets callbacks to NULL, disconnects client
*                   and resets client pcb, sent_len and recv_len
*/
static err_t tcp_client_close(void *arg)
{
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    err_t err = ERR_OK;
    if (state->client_pcb != NULL)
    {
        tcp_arg(state->client_pcb, NULL);
        tcp_poll(state->client_pcb, NULL, 0);
        tcp_sent(state->client_pcb, NULL);
        tcp_recv(state->client_pcb, NULL);
        tcp_err(state->client_pcb, NULL);
        err = tcp_close(state->client_pcb);
        if (err != ERR_OK) {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(state->client_pcb);
            err = ERR_ABRT;
        }

        state->sent_len = 0;
        state->recv_len = 0;
        state->client_pcb = NULL;
    }

    // Clear contents of received and sent buffers
    strcpy(state->buffer_recv, "");

    printf("[WiFi] Closed TCP Client Connection\n");

    return err;
}
/*!
*	@brief          Function to check if communication is successful
*                   Called when TCP has finished communication
*                   Sets complete flag to indicate end of operation
*/
static err_t tcp_server_result(void *arg, int status) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (status == 0) {
        DEBUG_printf("test success\n");
    } else {
        DEBUG_printf("test failed %d\n", status);
    }

    state->complete = true;
    return ERR_OK;
}


/*!
*	@brief          Function is called after server sends message to client
*                   CSets sent_len to length of message sent
*                   and recv_len to zero to indicate server is expecting 
*                   a reply from client
*/
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    DEBUG_printf("tcp_server_sent %u\n", len);
    state->sent_len += len;

    state->recv_len = 0;

    printf("Waiting for buffer from client\n");

    return ERR_OK;
}
/*!
*   @brief          to send message to client connected
*                   Copies message receive from client over to 
*                   reply buffer and retransmit the message back
*                   to the client using tcp_write
*                   Server calls tcp_client_close to disconnect
*                   client after 
*/
err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb, char message[BUF_SIZE])
{
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;

    strcpy(state->buffer_sent, message);
    
    state->sent_len = 0;
    DEBUG_printf("Writing %ld bytes to client\n", BUF_SIZE);
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    err_t err = tcp_write(tpcb, state->buffer_sent, BUF_SIZE, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        DEBUG_printf("Failed to write data %d\n", err);
        return tcp_server_result(arg, -1);
    }
    printf("Test buffer_send: %s\n", state->buffer_sent);

    if (state->client_pcb != NULL)
    {   
        tcp_client_close(arg);
    }
    return ERR_OK;
}

/*!
*   @brief          Function to receive message from client connected
*                   Called when the server receives data from client
*                   Once it receives the entire buffer of data
*                   it calls tcp_server_send_data to send a reply to client
*/
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (!p) 
    {
        return tcp_server_result(arg, -1);
    }
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    if (p->tot_len > 0) {
        DEBUG_printf("tcp_server_recv %d/%d err %d\n", p->tot_len, state->recv_len, err);

        // Receive the buffer
        const uint16_t buffer_left = BUF_SIZE - state->recv_len;
        state->recv_len += pbuf_copy_partial(p, state->buffer_recv + state->recv_len,
                                             p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }
    pbuf_free(p);

    // Have we have received the whole buffer
    printf("Test buffer_recv: %s\n", state->buffer_recv);
    
    DEBUG_printf("tcp_server_recv buffer ok\n");

    return tcp_server_send_data(arg, state->client_pcb, state->buffer_recv);
}


/*!
*   @brief          Function to check Client connection
*                   Called after connection has been idle
*                   for a set time interval, to kill the connection
*                   calls tcp_server_result to change flag to complete 
*                   to indicate end of operation 
*/
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
    DEBUG_printf("tcp_server_poll_fn\n");
    return tcp_server_result(arg, -1); // no response is an error?
}

/*!
*   @brief          Function to indicate fatal error in connection
*                   Called when a fatal error in connection has occurred
*                   Calls tcp_server_result to set flag to complete
*                   to indicate end of operation 
*/
static void tcp_server_err(void *arg, err_t err) {
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err_fn %d\n", err);
        tcp_server_result(arg, err);
    }
}

/*!
*   @brief          Function to set callbacks to other functions
*                   Called when a client connects to the server
*                   Stores the client PCB variable and sets up
*                   callback functions
*/
static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (err != ERR_OK || client_pcb == NULL) {
        DEBUG_printf("Failure in accept\n");
        tcp_server_result(arg, err);
        return ERR_VAL;
    }
    DEBUG_printf("Client connected\n");

    state->client_pcb = client_pcb;
    tcp_arg(client_pcb, state);
    tcp_sent(client_pcb, tcp_server_sent);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

/*!
*   @brief          Function to Open TCP server
*                   Creates a new PCB based on IP
*                   Binding it to the specified port
*                   and sets up callback functions to accept 
*                   incoming connections
*/
static bool tcp_server_open(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    DEBUG_printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, NULL, TCP_PORT);
    if (err) {
        DEBUG_printf("failed to bind to port %u\n", TCP_PORT);
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb) {
        DEBUG_printf("failed to listen\n");
        if (pcb) {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcp_server_accept);

    return true;
}

/*!
*   @brief          Function to Run TCP server
*                   Calls tcp_server_init to initialise a server state
*                   Opens a server by calling tcp_server_open by providing state 
*                   as arg 
*                   Continues looping until server has finished communication
*/
void run_tcp_server(void)
{
    TCP_SERVER_T *state = tcp_server_init();
    if (!state)
    {
        return;
    }
    
    if (!tcp_server_open(state))
    {
        tcp_server_result(state, -1);
        return;
    }

    while(!state->complete)
    {
        sleep_ms(1000);
    }

    free(state);
}
/*!
*   @brief          Function to initialize wifi module
*                   Connects to specified SSID and calls
*                   run_tcp_server to start tcp server 
*                   once connected to the SSID
*/
void wifi_init()
{
    if (cyw43_arch_init())
    {
        printf("[WiFi] Failed to initialize.\n");
        return;
    }

    cyw43_arch_enable_sta_mode();

    printf("[WiFi] Connecting to WiFi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0)
    {
        printf("[WiFi] Failed to connect.\n");
        printf("[WiFi] Attempting to reconnect...");
    }

    printf("[WiFi] Connected.\n");

    run_tcp_server();
    cyw43_arch_deinit();

    return;
}

int main()
{
    stdio_init_all();

    wifi_init();

    return 1;
}