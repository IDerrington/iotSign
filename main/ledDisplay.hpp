#ifndef _LEDDISPLAY_H_
#define _LEDDISPLAY_H_


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Used to to pass information to LED display task (WIP)
 * 
 */
typedef struct 
{
    char str;/* data */
    uint16_t type;
}ledDisplayQNFO_t;


/**
 * @brief Initialise the LED display task
 * 
 */
void initLEDDisplay(void);

/**
 * @brief Push a Message / Event to the LED task.
 * 
 * @return esp_err_t 
 */
esp_err_t PushLEDMsg(ledDisplayQNFO_t*);


#ifdef __cplusplus
}
#endif

#endif