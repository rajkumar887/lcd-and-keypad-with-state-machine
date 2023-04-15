

/*
**************************************************************************************************************

**************************************************************************************************************
*/
#ifndef __SPI_BITBANG_H__
#define __SPI_BITBANG_H__

#include "stdint.h"


unsigned char  Spi_Read_byte_cmx7341(unsigned char address);
unsigned short SPI_Read_HalfWord(unsigned char address_in);
uint16_t Spi_Read_word(unsigned char address_in);
void Spi_Read_BitBang_Stream(unsigned char reg_address_in, unsigned char *ptr_dataout, short n_out_data_bytes);

void Spi_Write_NOData_ChipCMX7341(unsigned char address_in);
void Spi_Write_byte_cmx7341(unsigned char add_in,unsigned char u8data);
void Spi_Write_HalfWord(unsigned char add_in,uint16_t hword_data);
void Spi_Write_BitBang_Stream(unsigned char address_in, unsigned short *ptr_data_in, int  n_in_data_bytes);
void Spi_Write_byte(unsigned char byte);


	

#endif
