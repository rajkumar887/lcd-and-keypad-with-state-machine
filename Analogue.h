/*
 * Analogue.h
 *
 *  Created on: 19-Aug-2022
 *      Author: dmr
 */

#ifndef INC_ANALOGUE_H_
#define INC_ANALOGUE_H_


void Unify_Start_Analogue(void);
void Unify_Analogue_Tx(void);
void Unify_Analogue_Rx(void);
void Unify_AgcConfig(void);
void Unify_Squelch(void);
void Unify_Analogue_ProgBlock0(void);
void Unify_Analogue_ProgBlock2(void);
void Unify_Analogue_ProgBlock6(void);
void Unify_config_cmx994(void);
void Unify_NextProgWrite(uint16_t data);
void Unify_ProgWrite(uint16_t addr, uint16_t data);
void Unify_AudioPAon(void);
void Unify_AudioPAoff(void);
void Check_FM_Mode_Rx_Status(void);

#endif /* INC_ANALOGUE_H_ */
