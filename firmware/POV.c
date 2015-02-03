//
// POV (Persistence of Vision)
// 
// (C) 2015, Daniel Quadros
//
// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <dqsoft.blogspot@gmail.com> wrote this file.  As long as you retain this 
//   notice you can do whatever you want with this stuff. If we meet some day, 
//   and you think this stuff is worth it, you can buy me a beer in return.
//      Daniel Quadros
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Conexões do hardware
#define SENSOR  _BV(PB0)
#define LED_VD  _BV(PB1)


// Variaveis
static const uint8_t imagem[16] =
{
    0x80, 0xC1, 0x82, 0xC3, 0x84, 0xC5, 0x86, 0xC7,
    0x88, 0xC9, 0x8A, 0xCB, 0x8C, 0xFD, 0x8E, 0x00
};

// Rotinas
static void initHw (void);

// Programa principal
int main(void)
{
    uint8_t  fOvf = 1;
    uint8_t  fSensor = 0;
    uint16_t tempo = 0;
    uint16_t prox = 0;
    uint16_t passo = 0;
    uint8_t  setor = 0;
    
    // Inicia o hardware
    initHw ();
    
    // Eterno equanto dure
    for (;;)
    {
        // Trata o sensor
        if (fSensor)
        {
            // já detectou o sensor, aguardando desligar
            fSensor = (PINB & SENSOR) == 0;
            if (!fSensor)
                PORTB &= ~LED_VD;       // apaga LED verde
        }
        else if ((PINB & SENSOR) == 0)
        {
            // Detectou sensor
            if (fOvf == 0)
            {
                // funcionamento normal
                tempo = TCNT1;          // registra o tempo da volta
                PORTA = imagem [0];     // LEDs para o primeiro setor
                passo = tempo >> 4;     // divide a volta em 16 setores
                prox = passo;
                setor = 1;
            }
            else
            {
                // ultrapassou o tempo máximo
                fOvf = 0;               // limpa o flag, vamos tentar de novo
            }
            TCNT1 = 0;          // reinicia a contagem de tempo
            fSensor = 1;        // lembra que detectou o sensor
            PORTB |= LED_VD;    // indica detecção
        }

        // Testa overflow do timer
        if (TIFR1 & _BV(TOV1))
        {
            fOvf = 1;               // ultrapassou o tempo máximo
            PORTA = 0;              // apaga os LEDs
            tempo = 0;              // não atualizar os LEDs
            TIFR1 |= _BV(TOV1);     // limpa o aviso do timer
        }
        
        // Atualiza os LEDs
        if (tempo != 0)
        {
            if (TCNT1 >= prox)
            {
                PORTA = imagem [setor++];   // passa para o setor seguinte
                prox += passo;
                if (setor == 16)
                    tempo = 0;              // acabaram os setores
            }
        }
        
    }
    
}

// Inicia o hardware
static void initHw (void)
{
    // Port A são os LEDs
    DDRA = 0xFF;    // tudo saida
    PORTA = 0;      // LEDs apagados
    
    // PORT B tem o sensor e o LED verde
    DDRB &= ~SENSOR;    // sensor é entrada
    DDRB |= LED_VD;     // LED é saida
    PORTB = SENSOR;     // com pullup
    
    // Timer 1
    // Modo normal, clock/1024
    TCCR1A = 0;
    TCCR1B = _BV(CS12) | _BV(CS10);
    TCCR1C = 0;
}



