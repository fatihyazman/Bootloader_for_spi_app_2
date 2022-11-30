/*
 * ethernet.h
 *
 *  Created on: Apr 5, 2022
 *      Author: fatih
 */

#ifndef ETHERNET_H_
#define ETHERNET_H_

#include <stdint.h>
#include "lwip/ip_addr.h"
#include "lwiplib.h"
#include "emac.h"

#define ETHERNET_RECONNECT_TIME 10000//ms

#define ETHERNET_INSTANCE_COUNT MAX_EMAC_INSTANCE

typedef enum ethernet_result_enum
{
    ETHERNET_RESULT_SUCCESS, ETHERNET_RESULT_ERROR

} ethernet_result_enum_t;

typedef enum ethernet_state_enum
{
    ETHERNET_STATE_NO, ETHERNET_STATE_OK

} ethernet_state_enum_t;

typedef enum ethernet_channels_enum
{
    ETHERNET_CHANNEL_1
} ethernet_channels_enum_t;


typedef enum ethernet_ip_mode_enum
{
     ETHERNET_IP_MODE_STATIC,
     ETHERNET_IP_MODE_DHCP,
     ETHERNET_IP_MODE_AUTOIP
}ethernet_ip_mode_enum_t;

typedef struct ethernet_settings_struct
{
    ip_addr_t ip_address;
    ip_addr_t net_mask;
    ip_addr_t gateway;

    uint8_t mac_address[6];

    ethernet_ip_mode_enum_t ip_mode;

    uint32_t reconnect_tickstart;

    ethernet_state_enum_t state;
} ethernet_settings_struct_t;

void ethernet_timer_it(void);
uint32_t ethernet_get_tick(void);

ethernet_result_enum_t ethernet_init(ethernet_channels_enum_t channel, bool reconnect);

ethernet_result_enum_t ethernet_lwip_init(ethernet_channels_enum_t channel, bool reconnect);
ethernet_result_enum_t ethernet_lwip_init_async(ethernet_channels_enum_t channel);

ethernet_result_enum_t ethernet_network_init(ethernet_channels_enum_t channel);

ethernet_result_enum_t ethernet_loop(void);



void ethernet_set_ip_address(ip_addr_t ip_address, ethernet_channels_enum_t channel);
void ethernet_set_gateway(ip_addr_t gateway, ethernet_channels_enum_t channel);
void ethernet_set_net_mask(ip_addr_t net_mask, ethernet_channels_enum_t channel);
void ethernet_set_mac_address(ethernet_channels_enum_t channel, uint8_t *mac_address);
void ethernet_set_state(ethernet_channels_enum_t channel, ethernet_state_enum_t state);
void ethernet_set_ip_mode(ethernet_channels_enum_t channel, ethernet_ip_mode_enum_t ip_mode);

ip_addr_t ethernet_get_ip_address(ethernet_channels_enum_t channel);
ip_addr_t ethernet_get_gateway(ethernet_channels_enum_t channel);
ip_addr_t ethernet_get_net_mask(ethernet_channels_enum_t channel);
uint8_t * ethernet_get_mac_address(ethernet_channels_enum_t channel);
ethernet_ip_mode_enum_t ethernet_get_ip_mode(ethernet_channels_enum_t channel);

#endif /* ETHERNET_H_ */
