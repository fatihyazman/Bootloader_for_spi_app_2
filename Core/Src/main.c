/*
 * main.c
 *
 *  Created on: Jul 20, 2022
 *      Author: fatih
 */

#include "sys_common.h"

#include "main.h"
#include "het.h"
#include "gio.h"
#include "rti.h"
#include "ethernet.h"

#include "my_tcp.h"
#include "sys_vim.h"
#include "esm.h"
#include "sys_core.h"

static void delay_ms(uint32_t time);

static tcp_result_t tcp_received(uint8_t channel, uint8_t *data, uint32_t data_len, uint16_t port);
static tcp_result_t tcp_disconnected(uint8_t channel);
static tcp_result_t tcp_connected(uint8_t tcp_channel);

extern uint8 emacAddress[6U];

volatile uint32_t counter;

volatile static tcp_struct_t tcp_connection;
void user_main(void)
{

	/*ERHERNET INIT BEGIN*/

	ethernet_set_ip_mode(ETHERNET_CHANNEL_1, ETHERNET_IP_MODE_DHCP);
	ethernet_set_mac_address(ETHERNET_CHANNEL_1, &emacAddress[0]);

	volatile ethernet_result_enum_t ethernet_result;
	do
	{

		ethernet_result = ethernet_init(ETHERNET_CHANNEL_1, true);
	} while (ETHERNET_RESULT_SUCCESS != ethernet_result);

	/*ERHERNET INIT END*/

	vimInit();
	esmInit();
	_enable_IRQ();
	_enable_interrupt_();
	rtiInit();

	rtiStartCounter(rtiCOUNTER_BLOCK0);
	rtiEnableNotification(rtiNOTIFICATION_COMPARE1);

	volatile const uint8_t ip_address[4] = { 192, 168, 31, 67 };
	uint16_t port = 5200;
	uint8_t message[] = "Hello, my linkedin account -> https://www.linkedin.com/in/fatihyazman/ \n\r";

	tcp_connection.channel = 0;
	tcp_connection.received = tcp_received;
	tcp_connection.disconnected = tcp_disconnected;
	tcp_connection.connected = tcp_connected;

	ip_addr_t destIPADDR;
	IP4_ADDR(&destIPADDR, 192, 168, 31, 67);

	tcp_connection.connection_timeout = 2000;
	tcp_connection.ip = destIPADDR;
	tcp_connection.port = port;
	tcp_connection.state = TCP_STATE_INIT;

	volatile tcp_result_t result;
	do
	{
		result = tcp_connect_my((tcp_struct_t*) &tcp_connection);

		delay_ms(2000);

	} while (TCP_RESULT_OK != result);

	uint32_t tickstart = counter;
	while (1)
	{
		if ((counter - tickstart) > 1000)
		{
			tcp_write_my((tcp_struct_t*) &tcp_connection, &message[0], strlen((const char*) (&message[0])));
			tickstart = counter;
		}
	}
}

void rtiNotification(uint32 notification)
{
	if (rtiNOTIFICATION_COMPARE0 == notification)
	{
	}
	else if (rtiNOTIFICATION_COMPARE1 == notification)
	{
		tcp_timer_it();
		counter++;
	}
}

static void delay_ms(uint32_t time)
{
	uint32_t cycle = 16000 * time;
	while (cycle--)
	{
	}
}

static tcp_result_t tcp_received(uint8_t channel, uint8_t *data, uint32_t data_len, uint16_t port)
{
	return TCP_RESULT_OK;
}

static tcp_result_t tcp_disconnected(uint8_t channel)
{
	return TCP_RESULT_OK;
}

static tcp_result_t tcp_connected(uint8_t tcp_channel)
{

	return TCP_RESULT_OK;
}
