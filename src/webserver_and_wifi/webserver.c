#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "cgi.h"


// WIFI Credentials - take care if pushing to github!
const char WIFI_SSID[] = "ShoutPenisForPassword_1_2.4GHz";
const char WIFI_PASSWORD[] = "Tyjtyc3009";

void wifi_init()
{
    if (cyw43_arch_init())
    {
        printf("[WiFi] Failed to initialize\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    printf("[WiFi] Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0)
    {
        printf("[WiFi] Failed to connect.\n");
        exit(1);
    }
    else
    {
        printf("[WiFi] Connected.\n");
    }

    
}

int main() {
    stdio_init_all();

    cyw43_arch_init();

    cyw43_arch_enable_sta_mode();

    // Connect to the WiFI network - loop until connected
    while(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0){
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected! \n");
    
    // Initialise web server
    httpd_init();
    printf("Http server initialised\n");

    // Configure SSI and CGI handler
    ssi_init(); 
    printf("SSI Handler initialised\n");
    cgi_init();
    printf("CGI Handler initialised\n");

    printf("IP: %s\n", ip4addr_ntoa(netif_ip_addr4(netif_default)));
    
    // Infinite loop
    while(1);
}