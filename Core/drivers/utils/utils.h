/*****************************************************************************
 * @file /CALMAR/Core/driver/utils/utils.h
 *
 * @brief Header file template
 * 
 * @author Michal Frankiewicz
 * @date Jun 25, 2020
 ****************************************************************************/
#pragma once 

#include <stdbool.h>
#include <stdint.h>

#include "middleware/soundNotifications/soundNotifications.h"

/*****************************************************************************
                       PUBLIC DEFINES / MACROS / ENUMS
*****************************************************************************/

#define RETURN_IF_FALSE(_TEST_,_RETURN_VALUE_)  if(!(_TEST_))               \
                                                {                           \
                                                    return _RETURN_VALUE_;  \
                                                }

#define RETURN_IF_TRUE(_TEST_,_RETURN_VALUE_)  if(_TEST_)                  \
                                               {                           \
                                                   return _RETURN_VALUE_;  \
                                               }
#define ASSERT(_CONDITION_) if(!(_CONDITION_)){while(1){SoundNotificationsPlayInBlockingMode(SN_ASSERT);}}

/*****************************************************************************
                         PUBLIC INTERFACE DECLARATION
*****************************************************************************/

/**@brief initializes utils
 */
void UtilsInit();


/**@brief calculates time elapsed from @param lastTimeCalled
 *        can measure up to ~60s after that resets to 0
 *        ! NEEDS TO BE CALLED AT LEAST EVERY 60s
 *
 * @param [in/out] lastTimeCalled - last time this function was called,
 *                                  this param is used to calculate elapsed time
 * @param [in] setCurrentTime - when true sets @param lastTimeCalled to current time
 * @return elapsed time in [s] with precision of [us]
 */
float GetTimeElapsed(uint32_t* lastTimeCalled, bool setCurrentTime);

/**@brief maps value from one interval to other
 *
 * @param [in] value - value to map
 * @param [in] fromMin - beginning of interval from which to map
 * @param [in] fromMax - end of interval from which to map
 * @param [in] toMin - beginning of interval to which to map
 * @param [in] toMax - end of interval to which to map
 * @return mapped value
 */
float UtilsMap(float value, float fromMin, float fromMax, float toMin, float toMax);
