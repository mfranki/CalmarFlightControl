/*****************************************************************************
 * @file /CALMAR/Core/driver/uart/uart.c
 *
 * @brief 
 * 
 * @author Michal Frankiewicz
 * @date May 5, 2020
 *
 ****************************************************************************/

#include "drivers/uart/uart.h"
#include "drivers/utils/utils.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/*****************************************************************************
                          PRIVATE DEFINES / MACROS
*****************************************************************************/

#define PRECISION_SIGNIFICANT_DIGITS (6U)   ///< needs to be  compatible with PRECISION_MULTIPLICATOR
#define PRECISION_MULTIPLICATOR (1000000U)  ///< = 10^PRECISION_SIGNIFICANT_DIGITS


/*****************************************************************************
                     PRIVATE STRUCTS / ENUMS / VARIABLES
*****************************************************************************/

static UART_HandleTypeDef *uartHandle;

static const char messageTooLongErrMsg[] = "Message too long\r\n";

/*****************************************************************************
                         PRIVATE FUNCTION DECLARATION
*****************************************************************************/

/**@brief disects double into two integers
 *        1-st int - digits before comma
 *        2-nd int log10(PRECISION_MULTIPLICATOR) amount of digits after comma
 * @param [in] d
 * @param [out] wholes
 * @param [out] parts
 * @param [out] bellow_0 - true when d<0
 */
void DoubleToTwoInts(double d, uint32_t* wholes, uint32_t* parts, bool *bellow_0);

/**@brief flips given array back to front
 *
 * @param [in/out] array - array pointer
 * @param [in] elementCount - number of array elements
 */
void MirrorCharArray(char array[], uint32_t elementCount);

/**@brief converts int32_t to string
 *
 * @param [in] integer
 * @param [out] buffer
 * @return count of characters written in buffer, 0 if error
 */
uint32_t IntToCharArray(int32_t integer, char buffer[]);

/**@brief converts uint32_t to string
 *
 * @param [in] integer
 * @param [out] buffer
 * @param [in] fillCharacters - pads with '0' in front up to this amount, setting to 0 equals no padding
 * @return count of characters written in buffer, 0 if error
 */
uint32_t UintToCharArray(uint32_t integer, char buffer[], uint32_t fillCharacters);

/**@brief converts uint32_t to string in hex format
 *
 * @param [in] integer
 * @param [out] buffer
 * @return count of characters written in buffer, 0 if error
 */
uint32_t HexToCharArray(uint32_t integer, char buffer[]);

/**@brief prints format to buffer substituting flags for data in args va_list
 *
 *        flags:
 *        %f - double from @param args
 *        %i - int32_t from @param args
 *        %u - uint32_t from @param args
 *        %x - uint32_t  (prints in hex format) from @param args
 *        %% - %
 *
 * @param [out] buffer
 * @param [in[ format
 * @param [in] args
 * @return count of characters written to buffer, 0 if error
 */
uint32_t Vsprintf(char* buffer, char format[], va_list args);

/*****************************************************************************
                           INTERFACE IMPLEMENTATION
*****************************************************************************/

bool UartInit(UART_HandleTypeDef *uh)
{
    uartHandle = uh;
    return true;
}

bool UartWrite(char *format, ...)
{
    if(strlen(format) > UART_MAX_MESSAGE_SIZE)
    {
        if(HAL_UART_Transmit(uartHandle, (uint8_t*)messageTooLongErrMsg, strlen(messageTooLongErrMsg), 1000) != HAL_OK)
        {
            return false;
        }
        return false;
    }

    va_list aptr;
    va_start(aptr, format);
    char buffer[UART_MAX_MESSAGE_SIZE];
    uint32_t msgSize = Vsprintf(buffer, format, aptr);
    ASSERT(msgSize <= UART_MAX_MESSAGE_SIZE)
    va_end(aptr);

    if(HAL_UART_Transmit(uartHandle, (uint8_t*)buffer, msgSize, 1000) != HAL_OK)
    {
        return false;
    }
    return true;
}


/******************************************************************************
                        PRIVATE FUNCTION IMPLEMENTATION
******************************************************************************/

void DoubleToTwoInts(double d, uint32_t* wholes, uint32_t* parts, bool *bellow_0)
{
    if(wholes == NULL || parts == NULL)
    {
        return;
    }
    *bellow_0 = d<0;

    *wholes = (uint32_t)abs((int32_t)d);
    *parts = abs((d-(((double)!bellow_0*2-1)*(double)(*wholes)))*PRECISION_MULTIPLICATOR);
}

void MirrorCharArray(char array[], uint32_t elementCount)
{
    if(array == NULL || elementCount < 2)
    {
        return;
    }

    for(uint32_t i=0; i<elementCount/2; i++)
    {
        char buffer = array[i];
        array[i] = array[elementCount-1-i];
        array[elementCount-1-i] = buffer;
    }
}

uint32_t IntToCharArray(int32_t integer, char buffer[])
{
    if(buffer == NULL)
    {
        return 0;
    }

    if(integer < 0)
    {
        buffer[0] = '-';
        return 1 + UintToCharArray(abs(integer), &buffer[1], 0);
    }

    return UintToCharArray(abs(integer), buffer, 0);
}

uint32_t UintToCharArray(uint32_t integer, char buffer[], uint32_t fillCharacters)
{
    if(buffer == NULL)
    {
        return 0;
    }

    uint32_t character = 0;
    do{
        buffer[character] = '0' + integer-(integer/10)*10;
        character++;
        integer /= 10;
    }while(integer != 0);

    while(character < fillCharacters)
    {
        buffer[character] = '0';
        character++;
    }

    MirrorCharArray(buffer, character);

    return character;
}

uint32_t HexToCharArray(uint32_t integer, char buffer[])
{
    if(buffer == NULL)
    {
       return 0;
    }

    uint32_t character = 0;
    do{
       uint8_t digit = integer-((integer>>4)<<4);
       if(digit < 10)
       {
           buffer[character] = '0' + digit;
       } else {
           buffer[character] = 'A' + digit-10;
       }
       character++;
       integer = integer>>4;
    }while(integer != 0);

    MirrorCharArray(buffer, character);

    return character;
}

uint32_t Vsprintf(char buffer[], char format[], va_list args)
{
    if(format == NULL || buffer == NULL)
    {
        return 0;
    }
    char prevSign = 0;
    uint32_t character = 0;
    bool prevDoublePercent = false;

    for(uint32_t i=0; format[i] != 0; prevSign=format[i],i++)
    {
        if(prevDoublePercent)
        {
            prevSign = 0;
            prevDoublePercent = false;
        }

        if(format[i] == '%')
        {
            if(prevSign == '%')
            {
                prevDoublePercent = true;
                buffer[character] = '%';
                character++;
            }
            continue;
        }

        if(prevSign == '%')
        {
            uint32_t wholes = 0;
            uint32_t parts = 0;
            int32_t I = 0;
            uint32_t U = 0;
            double D = 0;
            bool bellow_0 = false;

            switch(format[i])
            {
            case 'u':
                U = va_arg(args,uint32_t);
                character += UintToCharArray(U,&buffer[character],0);
                break;
            case 'i':
                I = va_arg(args,int32_t);
                character += IntToCharArray(I,&buffer[character]);
                break;
            case 'f':
                D = va_arg(args,double);
                DoubleToTwoInts(D, &wholes, &parts, &bellow_0);
                if(bellow_0)
                {
                        buffer[character] = '-';
                        character++;
                }
                character += UintToCharArray(wholes,&buffer[character],1);
                buffer[character] = '.';
                character++;
                character += UintToCharArray(parts,&buffer[character],PRECISION_SIGNIFICANT_DIGITS);
                break;
            case 'x':
                U = va_arg(args,uint32_t);
                character += HexToCharArray(U,&buffer[character]);
                break;
            }
            continue;
        }

        buffer[character] = format[i];
        character++;
    }

    buffer[character] = 0;
    character++;
    buffer[character] = 0;
    character++;
    return character;
}
