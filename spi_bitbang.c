#include <spi_bitbang.h>
#include "main.h"
#include "stm32f1xx_hal.h"
#include "stdint.h"

uint16_t Spi_Read_word(unsigned char address_in)
{
	uint16_t data_out =0;
	unsigned int n=0;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);
	Spi_Write_byte(address_in);
	for(n=16; n != 0; n--)
	{
		//take SCLK low
		HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_RESET);
		data_out <<= 1; //shift a 0 into b0 ready to build RDATA byte
		//take SCLK high
		HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_SET);

		//append 1 to <byte> if RDATA=1
		if(HAL_GPIO_ReadPin(RDATA_GPIO_Port,RDATA_Pin))
			data_out |= 0x0001;
	}
	//return the byte read from C-BUS
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);
	return data_out;
}


void Spi_Write_byte(unsigned char byte)
{
	unsigned char n;
	for(n =8; n !=0; n--)
	{
		if(byte & 0x80)
			HAL_GPIO_WritePin(CDATA_GPIO_Port, CDATA_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(CDATA_GPIO_Port, CDATA_Pin, GPIO_PIN_RESET);

		byte <<=1;
		//SCLK
		HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_SET);
	}
}



unsigned char Spi_Read_byte()
{
	unsigned char data_out =0;
	unsigned int n=0;
	for(n=8; n != 0; n--)
	{
		//take SCLK low
		//HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_RESET);
		SCLK_GPIO_Port->BSRR = (uint32_t)SCLK_Pin << 16u;
		data_out <<= 1; //shift a 0 into b0 ready to build RDATA byte
		//take SCLK high
		//HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_SET);
		SCLK_GPIO_Port->BSRR = SCLK_Pin;

		//append 1 to <byte> if RDATA=1
		//if(HAL_GPIO_ReadPin(RDATA_GPIO_Port,RDATA_Pin))
		if(RDATA_GPIO_Port->IDR & RDATA_Pin)
			data_out |= 0x01;
	}
	//return the byte read from C-BUS
	return data_out;
}


unsigned char Spi_Read_byte_cmx7341(unsigned char address)
{
	unsigned char read_out =0;
	//HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);
	CS_GPIO_Port->BSRR = (uint32_t)CS_Pin << 16u;
	Spi_Write_byte(address);
	read_out = Spi_Read_byte();
	//HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);
	CS_GPIO_Port->BSRR = CS_Pin;
	//HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_RESET);
	SCLK_GPIO_Port->BSRR = (uint32_t)SCLK_Pin << 16u;
	return read_out;
}


unsigned short SPI_Read_HalfWord(unsigned char address_in)
{
	unsigned short r_halfword =0;
	unsigned char temp =0, temp1 = 0;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);
	Spi_Write_byte(address_in);
	temp = Spi_Read_byte();
	temp1 = Spi_Read_byte();
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);
	r_halfword = temp;
	r_halfword<<=8;
	r_halfword |= temp1;
	//HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin , GPIO_PIN_SET);
    return r_halfword;
}


void Spi_Read_BitBang_Stream(unsigned char reg_address_in, unsigned char *ptr_dataout, short n_out_data_bytes)
{
	short n_b =0;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);
	Spi_Write_byte(reg_address_in);
	for( n_b=0; n_b<n_out_data_bytes;n_b++)
	{
		*(ptr_dataout +n_b) = Spi_Read_byte();
	}
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);


}


void Spi_Write_NOData_ChipCMX7341(unsigned char address_in)
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);
	Spi_Write_byte(address_in);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);
}



void Spi_Write_byte_cmx7341(unsigned char reg_addr,unsigned char u8data)
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);
	Spi_Write_byte(reg_addr);
	Spi_Write_byte((unsigned char )(u8data));
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);

}


void Spi_Write_HalfWord(unsigned char reg_addr,uint16_t hword_data)
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);

	Spi_Write_byte(reg_addr);
	Spi_Write_byte((unsigned char )(hword_data>>8));  // lsb
	Spi_Write_byte((unsigned char )(hword_data)); // msb

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);


}

void Spi_Write_BitBang_Stream(unsigned char address_in, unsigned short *ptr_data_in, int n_in_data_bytes)
{
	    int n_b =0;
	    unsigned short int data_in =0;
		 HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_RESET);

		Spi_Write_byte(address_in);

		for( n_b=0; n_b<(n_in_data_bytes);)
		{
			data_in  = *(ptr_data_in + n_b);
			Spi_Write_byte((unsigned char )(data_in)>>8);
			Spi_Write_byte((unsigned char)(data_in));
			n_b++;
		}
		 HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin , GPIO_PIN_SET);

}

