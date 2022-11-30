/**
 * \file            Tcp.c
 * \brief           LwIp üzerine geliştirilmiş bir Tcp katmanıdır.
 */

/*
 * Copyright (c) 2021 SapperX
 *
 * This file is part of Tcp.
 *
 * Author:          M.Fatih YAZMAN <fatih.yazman@sapperx.com>
 * Version:         1.0.0.0
 * Created on:      10/27/2021
 */

#include "my_tcp.h"
#include <stdlib.h>
#include "mem_system.h"
#include "hal_stdtypes.h"

/* VARIABLES BEGIN */

static uintmax_t tcp_timer_counter;

/* VARIABLES END */

/* STATIC FUNCTIONS PROTOTYPE BEGIN */
static uintmax_t tcp_timer_get(void);

static err_t tcp_pcb_closed(struct tcp_pcb *tcpPcb);

static err_t tcp_conencted(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_received_my(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_poll_my(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_sent_my(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_err_my(void *arg, err_t err);

/* STATIC FUNCTIONS PROTOTYPE END */

/* STATIC FUNCTIONS BEGIN */

static err_t tcp_pcb_closed(struct tcp_pcb *tcpPcb)
{
	tcp_arg(tcpPcb, NULL);
	tcp_sent(tcpPcb, NULL);
	tcp_recv(tcpPcb, NULL);
	tcp_err(tcpPcb, NULL);
	tcp_poll(tcpPcb, NULL, 0);

	tcp_abort(tcpPcb);
	return tcp_close(tcpPcb);
}

static err_t tcp_conencted(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	LWIP_UNUSED_ARG(err);

	if (NULL_PTR != arg)
	{
		tcp_struct_t *tcp = ((tcp_struct_t*) arg);
		MEMCPY(tcp->tcpPcb, newpcb, sizeof(struct tcp_pcb));

		tcp->state = TCP_STATE_CONNECTED;

		if (NULL_PTR != tcp->connected)
		{
			tcp->connected(tcp->channel);
		}

		return ERR_OK;
	}
	else
	{
		tcp_pcb_closed(newpcb);

		return ERR_ABRT;
	}
}

static err_t tcp_received_my(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	tcp_struct_t *tcp = ((tcp_struct_t*) arg);

	if (NULL_PTR == p) // p NULL ise bağlantının sonlandığı anlamına gelir.
	{
		tcp_disconnected_my(tcp);
		return ERR_OK;
	}
	else if (err != ERR_OK) //err bir hata döndürmüş ama p null değilse hata döndür ve p yi memory den at
	{
		/* p'yi at */
		if (NULL_PTR != p)
		{
			pbuf_free(p);
		}
		return err;
	}
	else if (tcp->state == TCP_STATE_CONNECTED || tcp->state == TCP_STATE_ACCEPTED)
	{

		if ((uint8_t*) NULL_PTR == tcp->receive_tcp_data)
		{
			tcp->receive_tcp_data = (uint8_t*) malloc(p->len);
			tcp->size = p->len;

			my_memcpy(&tcp->receive_tcp_data[0], p->payload, p->len);
		}
		else
		{
			tcp->receive_tcp_data = realloc(tcp->receive_tcp_data, tcp->size + p->len);

			my_memcpy(&tcp->receive_tcp_data[tcp->size], p->payload, p->len);
			tcp->size += p->len;
		}

		if ((uint8_t*) NULL_PTR != tcp->receive_tcp_data)
		{
			if (p->flags > 0)
			{

				if (NULL_PTR != tcp->received)
					tcp->received(tcp->channel, tcp->receive_tcp_data, tcp->size, tcp->port);

				tcp->size = 0;
			}
		}

		if (p->flags > 0)
		{
			if ((uint8_t*) NULL_PTR != tcp->receive_tcp_data)
			{
				free(tcp->receive_tcp_data);
				tcp->receive_tcp_data = (uint8_t*) NULL_PTR;
			}
		}

		pbuf_free(p);

		//tcp->received(tcp->channel, p->payload, p->len, tcp->port);
		/* Acknowledge the received data */
		tcp_recved(tcp->tcpPcb, p->tot_len);

		return ERR_OK;
	}
	else if (tcp->state == TCP_STATE_CLOSING)
	{
		tcp_recved(tpcb, p->tot_len);
		pbuf_free(p);

		tcp->state = TCP_STATE_CLOSED;
		if (NULL_PTR != tcp->disconnected)
			tcp->disconnected(tcp->channel);

		return ERR_OK;
	}
	else
	{
		tcp_recved(tpcb, p->tot_len);
		pbuf_free(p);
		return ERR_OK;
	}
}
err_t tcp_poll_my(void *arg, struct tcp_pcb *tpcb)
{
	tcp_struct_t *tcp = ((tcp_struct_t*) arg);
	if (NULL_PTR != tcp->poll)
		tcp->poll(tcp->channel);

	return ERR_OK;
}
err_t tcp_sent_my(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	tcp_struct_t *tcp = ((tcp_struct_t*) arg);

	if (NULL_PTR != tcp->sent)
		tcp->sent(tcp->channel, len);

	return ERR_OK;
}

static void tcp_err_my(void *arg, err_t err)
{
	tcp_struct_t *tcp = ((tcp_struct_t*) arg);
	if (NULL_PTR != tcp->err)
		tcp->err(tcp->channel, err);
}

static uintmax_t tcp_timer_get(void)
{
	return tcp_timer_counter;
}

/* STATIC FUNCTIONS END */

void tcp_timer_it(void)
{
	tcp_timer_counter++;
}
/**
 * @brief  This function is the implementation of tcp_accept LwIP callback
 * @param  arg: not used
 * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
 * @param  err: not used
 * @retval err_t: error status
 */
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	err_t ret_err;

	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	/* set priority for the newly accepted tcp connection newpcb */
	tcp_setprio(newpcb, TCP_PRIO_MIN);

	tcp_struct_t *tcp = ((tcp_struct_t*) arg);
	if (NULL_PTR != tcp)
	{
		tcp->state = TCP_STATE_ACCEPTED;
		if (NULL_PTR != newpcb)
		{
			//free(tcp->tcpPcb);
			tcp->tcpPcb = newpcb;
		}
		tcp->retries = 0;

		/* pass newly allocated es structure as argument to newpcb */
		tcp_arg(tcp->tcpPcb, tcp);

		/* initialize lwip tcp_recv callback function for newpcb  */
		tcp_recv(tcp->tcpPcb, tcp_received_my);

		/* initialize lwip tcp_err callback function for newpcb  */
		tcp_err(tcp->tcpPcb, tcp_err_my);

		/* initialize lwip tcp_poll callback function for newpcb */
		tcp_poll(tcp->tcpPcb, tcp_poll_my, 0);

		ret_err = ERR_OK;
	}
	else
	{
		/*  close tcp connection */
		tcp_pcb_closed(newpcb);
		/* return memory error */
		ret_err = ERR_MEM;
	}
	return ret_err;
}

tcp_result_t tcp_server_init(tcp_struct_t *tcp)
{
	tcp_result_t return_value = TCP_RESULT_OK;

	if (NULL_PTR != tcp->tcpPcb)
	{
		tcp_pcb_closed(tcp->tcpPcb);
	}
	else
	{
		tcp->tcpPcb = tcp_new();
	}

	if (NULL_PTR != tcp->tcpPcb)
	{
		err_t err = tcp_bind(tcp->tcpPcb, &tcp->ip, tcp->port);

		if (err == ERR_OK)
		{
			/* 3. start tcp listening for _pcb */
			struct tcp_pcb *new_tcp = tcp_listen(tcp->tcpPcb);
			if (NULL_PTR != new_tcp)
			{
				free(tcp->tcpPcb);
				tcp->tcpPcb = new_tcp;

				tcp_arg(tcp->tcpPcb, tcp);

				/* 4. initialize LwIP tcp_accept callback function */
				tcp_accept(tcp->tcpPcb, tcp_server_accept);

			}
			else
			{
				return_value = TCP_RESULT_ERROR;
			}
		}
		else
		{
			/* deallocate the pcb */
			free(tcp->tcpPcb);
		}
	}
	else
	{
		return_value = TCP_RESULT_ERR_MEMORY;
	}
	if (TCP_RESULT_OK != return_value)
	{
		free(tcp->tcpPcb);
	}
	return return_value;
}
tcp_result_t tcp_connect_my(tcp_struct_t *tcp)
{
	tcp_result_t return_value = TCP_RESULT_OK;

	if (NULL_PTR != tcp->tcpPcb)
	{
		tcp_pcb_closed(tcp->tcpPcb);
		free(tcp->tcpPcb);
	}

	tcp->tcpPcb = tcp_new();

	if (NULL_PTR != tcp->tcpPcb)
	{
		tcp->state = TCP_STATE_CONNECTING;

		tcp_arg(tcp->tcpPcb, tcp);

		/* initialize lwip tcp_recv callback function for newpcb  */
		tcp_recv(tcp->tcpPcb, tcp_received_my);
		/* initialize lwip tcp_poll callback function for newpcb */
		tcp_poll(tcp->tcpPcb, tcp_poll_my, 0);
		/* initialize LwIP tcp_sent callback function */
		tcp_sent(tcp->tcpPcb, tcp_sent_my);

		tcp_err(tcp->tcpPcb, tcp_err_my);

		err_t err = tcp_connect(tcp->tcpPcb, &tcp->ip, tcp->port, tcp_conencted);
		if (err == ERR_OK)
		{
			uintmax_t tickstart = tcp_timer_get();
			while ((tcp_timer_get() - tickstart) < tcp->connection_timeout)
			{
				if (tcp->state == TCP_STATE_CONNECTED)
				{
					break;
				}
			}
			if (tcp->state == TCP_STATE_CONNECTED)
			{
				return_value = TCP_RESULT_OK;
			}
			else
			{
				return_value = TCP_RESULT_TIMEOUT;
			}
		}
		else
		{
			return_value = TCP_RESULT_CONNECTION_ERROR;
		}

	}
	else
	{
		return_value = TCP_RESULT_ERR_MEMORY;
	}

	if (TCP_RESULT_OK != return_value)
	{
		free(tcp->tcpPcb);
	}
	if (tcp->state == TCP_STATE_CONNECTING)
	{
		tcp->state = TCP_STATE_UNUSED;
	}
	return return_value;
}

tcp_result_t tcp_disconnected_my(tcp_struct_t *tcp)
{
	tcp->state = TCP_STATE_CLOSING;

	err_t err = tcp_pcb_closed(tcp->tcpPcb);

	tcp->state = TCP_STATE_CLOSED;
	if (NULL_PTR != tcp->disconnected)
		tcp->disconnected(tcp->channel);

	return TCP_RESULT_ERROR;
}

tcp_result_t tcp_write_my(tcp_struct_t *tcp, const uint8_t *const data, uint32_t data_length)
{
	if (tcp->state != TCP_STATE_CONNECTED)
		return TCP_RESULT_NOT_CONNECTED;

	err_t result = tcp_write(tcp->tcpPcb, data, data_length, TCP_WRITE_FLAG_COPY);
	if (result == ERR_OK)
	{
		if (tcp_output(tcp->tcpPcb) == ERR_OK)
		{
			return TCP_RESULT_OK;
		}
		else
		{
			return TCP_RESULT_ERROR;
		}
	}
	else
	{
		return TCP_RESULT_ERROR;
	}
}

