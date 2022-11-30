/*
 * main.h
 *
 *  Created on: Jul 20, 2022
 *      Author: M.Fatih YAZMAN
 */

#ifndef CORE_INC_MAIN_H_
#define CORE_INC_MAIN_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void user_main(void);
void tmc5160_readWriteArray(uint8_t channel, uint8_t *data, size_t length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CORE_INC_MAIN_H_ */
