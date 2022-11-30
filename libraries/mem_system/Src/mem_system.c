/*
#include <mem_system.h>
 * System.c
 *
 *  Created on: May 17, 2022
 *      Author: fatih
 */

#include <stdint.h>
#include <stdio.h>
#include "mem_system.h"
#include "hal_stdtypes.h"

void* my_memcpy(void *dest, const void *src, uint32_t n)
{
	if(NULL_PTR == dest || NULL_PTR == src)
	{
		return NULL_PTR;
	}

	uint8_t *dest_u8 = (uint8_t*)dest;
	uint8_t *src_u8 = (uint8_t*)src;

	for (size_t i = 0; i < n; i++)
	{
		dest_u8[i] = src_u8[i];
	}

	return dest;
}
