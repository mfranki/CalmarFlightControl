/*****************************************************************************
 * @file /CalmarFlightController/Core/middleware/altitude/altitude.h
 *
 * @brief Header file 
 * 
 * @author Michal Frankiewicz
 * @date Jun 23, 2021
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/



/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes altitude filter
 *
 * @return true if successful
 */
bool AltitudeInit();

/**@brief freertos task
 */
void AltitudeTask();

/**@brief returns distance in height between current position and home position
 *
 * @return distance [m]
 */
float AltitudeGetAltitudeFromHome();

/**@brief sets current height as position
 */
void AltitudeSetHome();
