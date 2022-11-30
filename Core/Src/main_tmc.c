/*
 * main.c
 *
 *  Created on: Jul 20, 2022
 *      Author: fatih
 */

#include "sys_common.h"

#include "main_tmc.h"
#include "het.h"
#include "gio.h"
#include "sci.h"
#include "rti.h"

#include "TMC5160.h"

static void delay_ms(uint32_t time);

volatile uint32_t counter;

static ConfigurationTypeDef config;
static TMC5160TypeDef tmc5160;

void user_main(void)
{
	_enable_IRQ();
	rtiInit();
	rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
	rtiStartCounter(rtiCOUNTER_BLOCK0);
//	rtiEnableNotification(rtiNOTIFICATION_COMPARE1);
//	rtiStartCounter(rtiCOUNTER_BLOCK1);


	gioSetDirection(hetPORT1, 0xFFFFFFFF);
	gioSetBit(hetPORT1, 16, 0);

	tmc5160_init(&tmc5160, 0, &config, &tmc5160_defaultRegisterResetState[0]);

	uint32_t tickstart = counter;
	while (1)
	{

	}
}

void rtiNotification(uint32 notification)
{
	if (rtiNOTIFICATION_COMPARE0 == notification)
	{
		counter++;
	}
	else if (rtiNOTIFICATION_COMPARE1 == notification)
	{
	}
}

static void delay_ms(uint32_t time)
{
	uint32_t cycle = 16000 * time;
	while (cycle--)
	{
	}
}

void tmc5160_readWriteArray(uint8_t channel, uint8_t *data, size_t length)
{

}



