/*
 * rotary.h
 *
 *  Created on: Aug 2, 2022
 *      Author: dmr1
 */

#ifndef INC_ROTARY_H_
#define INC_ROTARY_H_

struct Channel_parameters
{
	double frequency;
	double txfrequency;
	double rxfrequency;
	uint8_t channel_number;
	uint8_t mode;
	uint8_t power;
	uint8_t squelch;
	uint8_t channel_type;

};

void unify_ChannelParameters(void);

void rotary_information(uint8_t status);

uint8_t unify_RotaryGetKey(void);

#endif /* INC_ROTARY_H_ */
