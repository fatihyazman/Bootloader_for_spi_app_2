/*
 * ethernet.c
 *
 *  Created on: Apr 5, 2022
 *      Author: fatih
 */
#include "ethernet.h"
#include <string.h>
#include "lwip\inet.h"

uint32_t ethernet_reconnect_count;

static ethernet_settings_struct_t ethernet_settings[ETHERNET_INSTANCE_COUNT];

static void link_status_changed_it(ethernet_channels_enum_t channel);

static void link_status_changed_it(ethernet_channels_enum_t channel)
{
	if (lwip_link_status_get(channel) == 0)
	{

	}
}

static uint32_t ethernet_tick;
void ethernet_timer_it(void)
{
	ethernet_tick++;

}

uint32_t ethernet_get_tick(void)
{
	return ethernet_tick;
}

ethernet_result_enum_t ethernet_init(ethernet_channels_enum_t channel, bool reconnect)
{
	ethernet_result_enum_t status = ethernet_lwip_init(channel, reconnect);

	return status;
	//return ETHERNET_RESULT_SUCCESS;
}

ethernet_result_enum_t ethernet_lwip_init(ethernet_channels_enum_t channel, bool reconnect)
{
	/* Enable the interrupt generation in CPSR register */
	IntMasterIRQEnable();
	_enable_FIQ();
    unsigned int 	state;
    state = my_lwip_init(0, ethernet_get_mac_address(channel), 0, 0, 0, IPADDR_USE_DHCP);

	//state = my_lwip_init((uint8_t) channel, reconnect, ethernet_get_mac_address(channel), ethernet_get_ip_address(channel), ethernet_get_net_mask(channel), ethernet_get_gateway(channel), (uint8_t) ethernet_get_ip_mode(channel));
	// state = my_lwip_init(0, ethernet_get_mac_address(channel), ip_addr, ip_addr, ip_addr, IPADDR_USE_DHCP);
	if (state != 0)
	{
		ethernet_set_state(channel, ETHERNET_STATE_OK);

		if (reconnect == true)
		{
			ethernet_reconnect_count++;
		}

		ethernet_set_ip_address(lwip_get_ip_address((uint8_t) ETHERNET_CHANNEL_1), ETHERNET_CHANNEL_1);
		ethernet_set_gateway(lwip_get_gateway((uint8_t) ETHERNET_CHANNEL_1), ETHERNET_CHANNEL_1);
		ethernet_set_net_mask(lwip_get_net_mask((uint8_t) ETHERNET_CHANNEL_1), ETHERNET_CHANNEL_1);

		volatile ip_addr_t ipAddress = ethernet_get_ip_address(ETHERNET_CHANNEL_1);
		volatile ip_addr_t gateway = ethernet_get_gateway(ETHERNET_CHANNEL_1);
		volatile ip_addr_t netMask = ethernet_get_net_mask(ETHERNET_CHANNEL_1);

		return ETHERNET_RESULT_SUCCESS;
	}
	else
	{
		ethernet_set_state(channel, ETHERNET_STATE_NO);
	}

	return ETHERNET_RESULT_ERROR;
}
ethernet_result_enum_t ethernet_lwip_init_async(ethernet_channels_enum_t channel)
{

	return ETHERNET_RESULT_SUCCESS;
}

ethernet_result_enum_t ethernet_network_init(ethernet_channels_enum_t channel)
{

	return ETHERNET_RESULT_SUCCESS;
}

ethernet_result_enum_t ethernet_loop(void)
{
	static bool first_link_status = 1;
	for (uint8_t ethernet_channel = 0; ethernet_channel < ETHERNET_INSTANCE_COUNT; ethernet_channel++)
	{
		if (lwip_link_status_get(ethernet_channel) == 0)
		{
			if (first_link_status == 1)
			{
				link_status_changed_it((ethernet_channels_enum_t) ethernet_channel);

				first_link_status = 0;
			}
			if ((ethernet_get_tick() - ethernet_settings[ethernet_channel].reconnect_tickstart) > ETHERNET_RECONNECT_TIME)
			{
				ethernet_init((ethernet_channels_enum_t) ethernet_channel, true);

				ethernet_settings[ethernet_channel].reconnect_tickstart = ethernet_get_tick();
			}
		}
		else
		{
			if (first_link_status == 0)
			{
				link_status_changed_it((ethernet_channels_enum_t) ethernet_channel);

				first_link_status = 1;
			}
			ethernet_settings[ethernet_channel].reconnect_tickstart = ethernet_get_tick();
		}
	}

	return ETHERNET_RESULT_SUCCESS;
}

void ethernet_set_ip_address(ip_addr_t ip_address, ethernet_channels_enum_t channel)
{
	ethernet_settings[(uint8_t) channel].ip_address = ip_address;
}

void ethernet_set_gateway(ip_addr_t gateway, ethernet_channels_enum_t channel)
{
	ethernet_settings[(uint8_t) channel].gateway = gateway;
}

void ethernet_set_net_mask(ip_addr_t net_mask, ethernet_channels_enum_t channel)
{
	ethernet_settings[(uint8_t) channel].net_mask = net_mask;
}

void ethernet_set_mac_address(ethernet_channels_enum_t channel, uint8_t *mac_address)
{
	for (uint8_t i = 0; i < 6; i++)
	{
		ethernet_settings[(uint8_t) channel].mac_address[i] = mac_address[i];
	}
}
void ethernet_set_state(ethernet_channels_enum_t channel, ethernet_state_enum_t state)
{
	ethernet_settings[(uint8_t) channel].state = state;
}

void ethernet_set_ip_mode(ethernet_channels_enum_t channel, ethernet_ip_mode_enum_t ip_mode)
{
	ethernet_settings[(uint8_t) channel].ip_mode = ip_mode;
}

ip_addr_t ethernet_get_ip_address(ethernet_channels_enum_t channel)
{
	return ethernet_settings[(uint8_t) channel].ip_address;
}

ip_addr_t ethernet_get_gateway(ethernet_channels_enum_t channel)
{
	return ethernet_settings[(uint8_t) channel].gateway;
}

ip_addr_t ethernet_get_net_mask(ethernet_channels_enum_t channel)
{
	return ethernet_settings[(uint8_t) channel].net_mask;
}

uint8_t* ethernet_get_mac_address(ethernet_channels_enum_t channel)
{
	return &ethernet_settings[(uint8_t) channel].mac_address[0];
}

ethernet_ip_mode_enum_t ethernet_get_ip_mode(ethernet_channels_enum_t channel)
{
	return ethernet_settings[(uint8_t) channel].ip_mode;
}

ethernet_state_enum_t ethernet_get_state(ethernet_channels_enum_t channel)
{
	return ethernet_settings[(uint8_t) channel].state;
}
