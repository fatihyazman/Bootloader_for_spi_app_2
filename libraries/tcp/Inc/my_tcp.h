/**
 * \file            Tcp.h
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

#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include "lwip/tcp.h"
#include  "ipv4/lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum tcp_result
{
	TCP_RESULT_ERR_MEMORY,
	TCP_RESULT_ERROR,
	TCP_RESULT_NOT_CONNECTED,
	TCP_RESULT_OK,
	TCP_RESULT_CONNECTION_ERROR,
	TCP_RESULT_TIMEOUT
} tcp_result_t;

/**
 * \brief           Tcp üzerinden data geldiğinde çağırılan fonksiyon
 * \param[in]       tcp_channel: Tcp bağlantısının index'i
 * \param[in]       *p: Gelen data ile ilgili bilgilerin bulunduğu structure adresi
 * \return          İşlem sonucu
 */
typedef tcp_result_t (*my_tcp_received_fn)(uint8_t tcp_channel, uint8_t *data, uint32_t data_len, uint16_t port);
/**
 * \brief           Tcp bağlantısı sağlandığında çağırılan fonksiyon
 * \param[in]       tcp_channel: Tcp bağlantısının index'i
 * \return          İşlem sonucu
 */
typedef tcp_result_t (*my_tcp_connected_fn)(uint8_t tcp_channel);

/**
 * \brief           Tcp bağlantısı Sonlandığında çağırılan foksiyon
 * \param[in]       tcp_channel: Tcp bağlantısının index'i
 * \return          İşlem sonucu
 */
typedef tcp_result_t (*my_tcp_disconnected_fn)(uint8_t channel);
typedef tcp_result_t (*my_tcp_err_fn)(uint8_t channel, err_t err);
typedef tcp_result_t (*my_tcp_poll_fn)(uint8_t channel);
typedef tcp_result_t (*my_tcp_sent_fn)(uint8_t channel, u16_t len);

typedef enum TcpState
{
	TCP_STATE_UNUSED,
	TCP_STATE_CLOSING,
	TCP_STATE_CLOSED,
	TCP_STATE_INIT,
	TCP_STATE_CONNECTING,
	TCP_STATE_CONNECTED,
	TCP_STATE_STARTING,
	TCP_STATE_ACCEPTED
} TcpState_t;

typedef struct tcp_struct
{
	struct tcp_pcb *tcpPcb;

	ip_addr_t ip;
	uint16_t port;

	my_tcp_received_fn received;
	my_tcp_connected_fn connected;
	my_tcp_disconnected_fn disconnected;
	my_tcp_err_fn err;
	my_tcp_poll_fn poll;
	my_tcp_sent_fn sent;

	enum TcpState state;
	uint8_t channel;

	uint32_t connection_timeout; /* ms */

	/*Server*/
	uint8_t retries;


	uint8_t *receive_tcp_data;
	uint32_t size;

} tcp_struct_t;

/**
 * \brief           tcp zamanlama işlemlerinin yapılması için 1ms periodlar ile çağırılmalı.
 * \param[in]       void
 * \return          void
 */
void tcp_timer_it(void);
/**
 * \brief           Tcp bağlantısının yapıldığı fonksiyon.
 * \param[in]       *tcp: Bağlantı yapılacak Tcp özelliklerinin bulunduğu structure.
 * \return          İşlem sonucu
 */
tcp_result_t tcp_connect_my(tcp_struct_t *tcp);
/**
 * \brief           Tcp bağlantısının sonlandırıldığı fonksiyon.
 * \param[in]       *tcp: Bağlantısı sonlandırılacak Tcp özelliklerinin bulunduğu structure.
 * \return          İşlem sonucu
 */
tcp_result_t tcp_disconnected_my(tcp_struct_t *tcp);

/**
 * \brief           Tcp bağlantından data gönderir.
 * \param[in]       *tcp: data gönderilecek tcp bağlantısı.
 * \param[in]       *data: gönderilecek data.
 * \param[in]       dataLength: gönderilecek datanın uzunluğu.
 * \return          İşlem sonucu
 */
tcp_result_t tcp_write_my(tcp_struct_t *tcp, const uint8_t *const data, uint32_t data_length);


tcp_result_t tcp_server_init(tcp_struct_t *tcp);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TCP_H */
