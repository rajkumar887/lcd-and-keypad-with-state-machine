/*
 * u8g2_stm32f1.h
 *
 *  Created on: Jul 25, 2022
 *      Author: dmr1
 */

#ifndef INC_U8G2_STM32F1_H_
#define INC_U8G2_STM32F1_H_

uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);


#endif /* INC_U8G2_STM32F1_H_ */
