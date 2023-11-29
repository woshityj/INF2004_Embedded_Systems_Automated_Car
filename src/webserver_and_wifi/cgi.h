#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"

/*!
*	@brief      CGI Handler that is ran when a specific request
*               from the client is detected, in this case it is the
*               /led.cgi that toggles the pico leds on/off
*/
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if an request for LED has been made (/led.cgi?led=x)
    if (strcmp(pcParam[0] , "led") == 0){
        // Look at the argument to check if LED is to be turned on (x=1) or off (x=0)
        if(strcmp(pcValue[0], "0") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        else if(strcmp(pcValue[0], "1") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    
    // Send the index page back to the user
    return "/index.shtml";
}

// tCGI Struct
// Struct containing all of the CGI requests and their respective handlers functions
static const tCGI cgi_handlers[] = {
    {
        // Html request for "/led.cgi" triggers cgi_handler
        "/led.cgi", cgi_led_handler
    },
};

/*!
*	@brief      This function initializes all CGI Handlers
*               based on the struct containing all the CGI requests
*/
void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, 1);
}