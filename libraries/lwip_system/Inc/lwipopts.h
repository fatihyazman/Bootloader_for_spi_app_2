/**
 * \file lwipopts.h - Configuration options for lwIP
 *
 * Copyright (c) 2010 Texas Instruments Incorporated
 */
/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/*****************************************************************************
**                           CONFIGURATIONS
*****************************************************************************/
/*****************************************************************************
**                    Platform specific locking
*****************************************************************************/
#define SYS_LIGHTWEIGHT_PROT            1
#define NO_SYS                          1

/*****************************************************************************
**                           DHCP Options
*****************************************************************************/
#define LWIP_DHCP                       1
#define DHCP_DOES_ARP_CHECK             0

/*****************************************************************************
**                           PBUF  Options
*****************************************************************************/
//#define PBUF_LINK_HLEN                  28      /* default is 14 */
//#define PBUF_POOL_BUFSIZE               (1024 * 2)
                                           /* default is LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)*/
#define ETH_PAD_SIZE                    0
#define LWIP_NETCONN                    0          /*default is 1*/

/*****************************************************************************
**                           Socket  Options
*****************************************************************************/
#define LWIP_SOCKET                     0           /* default is 1 */
#endif /* __LWIPOPTS_H__ */
