/*
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU (Lesser) General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    linefollowing.c
 */

#include <linefollowing.h>
#include <amiroos.h>
#include <stdlib.h>
#include <math.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

#define RAND_TRESH 20000
#define MAX_CORRECTED_SPEED 1000000*100

#define TIMER_TRIGGEREVENT            (urtCoreGetEventMask() << 1)
#define PROXFLOOREVENT                (urtCoreGetEventMask() << 2)
#define PROXENVEVENT                  (urtCoreGetEventMask() << 3)

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/**
 * @brief   Name of linefollowing nodes.
 */
static const char _linefollowing_name[] = "LineFollowing";

/**
 * Line Following Setup
 */
float lf_rpmSpeed[2] = {0};

// The different classes (or members) of color discrimination
// BLACK is the line itselfe
// GREY is the boarder between the line and the surface
// WHITE is the common surface
typedef enum{
  BLACK,
  GREY,
  WHITE
}colorMember_t;

enum floorSensorIdx {
  WHEEL_LEFT,
  FRONT_LEFT,
  FRONT_RIGHT,
  WHEEL_RIGHT
};

enum wheelIdx {
  LEFT_WHEEL,
  RIGHT_WHEEL
};

const float rpmForward[2] = {0.08,0};
//Motor controller crashes with rpmSoftLeft and rpmHardLeft
const float rpmSoftLeft[2] = {0.06,0.3};
const float rpmHardLeft[2] = {0.03,0.5};
const float rpmSoftRight[2] = {rpmSoftLeft[0],(-1)*rpmSoftLeft[1]};
const float rpmHardRight[2] = {rpmHardLeft[0],(-1)*rpmHardLeft[1]};
const float rpmTurnLeft[2] = {0, 0.5};
const float rpmTurnRight[2] = {rpmTurnLeft[0],(-1)*rpmTurnLeft[1]};
const float rpmHalt[2] = {0, 0};
const float rpmStop[2] = {NAN, NAN}; // Setting this speed will stop the motor entirely

// Definition of the fuzzyfication function
//  | Membership
// 1|_B__   G    __W__
//  |    \  /\  /
//  |     \/  \/
//  |_____/\__/\______ Sensor values
// SEE MATLAB SCRIPT "fuzzyRule.m" for adjusting the values
// All values are "raw sensor values"
/* Use these values for white ground surface (e.g. paper) */

const uint16_t blackStartFalling = 9000; // Where the black curve starts falling
const uint16_t blackOff = 15000; // Where no more black is detected
const uint16_t whiteStartRising = 20000; // Where the white curve starts rising
const uint16_t whiteOn = 25000; // Where the white curve has reached the maximum value

const uint16_t greyMax = (whiteOn + blackStartFalling) / 2; // Where grey has its maximum
const uint16_t greyStartRising = blackStartFalling; // Where grey starts rising
const uint16_t greyOff = whiteOn; // Where grey is completely off again

// PID controller components ---------------
float ef_K_p = 1.0;
float ef_K_i = 0.0;
float ef_K_d = 0.0;
float ef_accumHist = 0;
float ef_oldError = 0;
int ef_whiteFlag = 0;
int ef_transBias = 0;
// ----------------------------------------

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

//Signal the led light service to update current colors
void lf_signalLightService(void* linefollowing) {
  urtDebugAssert(linefollowing != NULL);

  if (urtNrtRequestTryAcquire(&((linefollowing_node_t*)linefollowing)->light_data.request) == URT_STATUS_OK) {
    urtNrtRequestSubmit(&((linefollowing_node_t*)linefollowing)->light_data.request,
                        urtCoreGetService(((linefollowing_node_t*)linefollowing)->light_data.serviceid),
                        sizeof(((linefollowing_node_t*)linefollowing)->light_data.data),
                        0);
  } else {
    urtPrintf("Could not acquire urt while signaling light service!");
  }
  return;
}

//Signal the service with the new motor data
void lf_signalMotorService(linefollowing_node_t* linefollowing, float* rpm) {
  urtDebugAssert(linefollowing != NULL);

  linefollowing->motor_data.data.translation.axes[0] = rpm[0];
  linefollowing->motor_data.data.rotation.vector[2] = rpm[1];

  urt_status_t status = urtNrtRequestTryAcquire(&linefollowing->motor_data.request);
  if (status == URT_STATUS_OK) {
    urtNrtRequestSubmit(&linefollowing->motor_data.request,
                        linefollowing->motor_data.service,
                        sizeof(linefollowing->motor_data.data),
                        0);
  } else {
    urtPrintf("Could not acquire urt while signaling motor service! Reason:%i\n", status);
  }
  return;
}

void lf_getData(void* linefollowing, urt_osEventMask_t event) {
  urtDebugAssert(linefollowing != NULL);

  //local variables
  urt_status_t status;

  if (event & PROXFLOOREVENT) {
    // fetch NRT of the floor proximity data
    do {
      status = urtNrtSubscriberFetchNext(&((linefollowing_node_t*)linefollowing)->floor_data.nrt,
                                         &((linefollowing_node_t*)linefollowing)->floor_data.data,
                                         &((linefollowing_node_t*)linefollowing)->floor_data.data_size,
                                         NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);
    event &= ~PROXFLOOREVENT;
  }

  if (event & PROXENVEVENT) {
    // fetch NRT of the touch sensor data
    do {
      status = urtNrtSubscriberFetchNext(&((linefollowing_node_t*)linefollowing)->environment_data.nrt,
                                         &((linefollowing_node_t*)linefollowing)->environment_data.data,
                                         &((linefollowing_node_t*)linefollowing)->environment_data.data_size,
                                         NULL, NULL);
    } while (status != URT_STATUS_FETCH_NOMESSAGE);
    event &= ~PROXENVEVENT;
  }

  return;
}

// Copy the speed from the source to the target array
void copyRpmSpeed(const float* source, float* target) {
  target[LEFT_WHEEL] = source[LEFT_WHEEL];
  target[RIGHT_WHEEL] = source[RIGHT_WHEEL];
}

/**
 * @brief   Callback function for the trigger timer.
 *
 * @param[in] flags   Flags to emit for the trigger event.
 *                    Pointer is reinterpreted as integer value.
 */
static void _linefollowing_triggercb(virtual_timer_t* timer, void* params)
{
  // local constants
  linefollowing_node_t* const lfn = (linefollowing_node_t*)params;

  // signal node thread to read proximity data
  if (timer == &lfn->timer.vt) {
    urtEventSignal(lfn->node.thread, TIMER_TRIGGEREVENT);
  }

  return;
}

// Return the color, which has the highest fuzzy value
colorMember_t getMember(float* fuzzyValue) {
  colorMember_t member;
  if (fuzzyValue[BLACK] > fuzzyValue[GREY] && fuzzyValue[BLACK] > fuzzyValue[WHITE]) {
    member = BLACK;
  } else if (fuzzyValue[GREY] > fuzzyValue[WHITE]) {
    member = GREY;
  } else {
    member = WHITE;
  }

  return member;
}

// Get a crisp output for the steering commands
void defuzzyfication(colorMember_t* member, float *rpmFuzzyCtrl) {
  // all sensors are equal
  if (member[WHEEL_LEFT] == member[FRONT_LEFT] &&
      member[FRONT_LEFT] == member[FRONT_RIGHT] &&
      member[FRONT_RIGHT] == member[WHEEL_RIGHT]) {
    // something is wrong -> stop
    copyRpmSpeed(rpmHalt, rpmFuzzyCtrl);
    urtPrintf("HALT: all equal \n");
  // both front sensor detect a line
  } else if (member[FRONT_LEFT] == BLACK &&
      member[FRONT_RIGHT] == BLACK) {
    // straight
    copyRpmSpeed(rpmForward, rpmFuzzyCtrl);
    urtPrintf("FORWARD \n");
  // exact one front sensor detects a line
  } else if (member[FRONT_LEFT] == BLACK ||
             member[FRONT_RIGHT] == BLACK) {
    // soft correction
    if (member[FRONT_LEFT] == GREY) {
      // soft right
      copyRpmSpeed(rpmSoftRight, rpmFuzzyCtrl);
      urtPrintf("SOFT RIGHT \n");
    } else if (member[FRONT_LEFT] == WHITE) {
      // hard right
      copyRpmSpeed(rpmHardRight, rpmFuzzyCtrl);
      urtPrintf("HARD RIGHT \n");
    } else if (member[FRONT_RIGHT] == GREY) {
      // soft left
      copyRpmSpeed(rpmSoftLeft, rpmFuzzyCtrl);
      urtPrintf("SOFT LEFT \n");
    } else if (member[FRONT_RIGHT] == WHITE) {
      // hard left
      copyRpmSpeed(rpmHardLeft, rpmFuzzyCtrl);
      urtPrintf("HARD LEFT \n");
    }
  // both wheel sensors detect a line
  } else if (member[WHEEL_LEFT] == BLACK &&
             member[WHEEL_RIGHT] == BLACK) {
    // something is wrong -> stop
    copyRpmSpeed(rpmHalt, rpmFuzzyCtrl);
    urtPrintf("HALT: wheel line \n");
  // exactly one wheel sensor detects a line
  } else if (member[WHEEL_LEFT] == BLACK ||
             member[WHEEL_RIGHT] == BLACK) {
    if (member[WHEEL_LEFT] == BLACK) {
      // turn left
      copyRpmSpeed(rpmTurnLeft, rpmFuzzyCtrl);
      urtPrintf("TURN LEFT \n");
    } else if (member[WHEEL_RIGHT] == BLACK) {
      // turn right
      copyRpmSpeed(rpmTurnRight, rpmFuzzyCtrl);
      urtPrintf("TURN RIGHT \n");
    }
  // both front sensors may detect a line
  } else if (member[FRONT_LEFT] == GREY &&
             member[FRONT_RIGHT] == GREY) {
    if (member[WHEEL_LEFT] == GREY) {
      // turn left
      copyRpmSpeed(rpmTurnLeft, rpmFuzzyCtrl);
      urtPrintf("TURN LEFT \n");
    } else if (member[WHEEL_RIGHT] == GREY) {
      // turn right
      copyRpmSpeed(rpmTurnRight, rpmFuzzyCtrl);
      urtPrintf("TURN RIGHT \n");
    }
  // exactly one front sensor may detect a line
  } else if (member[FRONT_LEFT] == GREY ||
             member[FRONT_RIGHT] == GREY) {
    if (member[FRONT_LEFT] == GREY) {
      // turn left
      copyRpmSpeed(rpmTurnLeft, rpmFuzzyCtrl);
      urtPrintf("TURN LEFT \n");
    } else if (member[FRONT_RIGHT] == GREY) {
      // turn right
      copyRpmSpeed(rpmTurnRight, rpmFuzzyCtrl);
      urtPrintf("TURN RIGHT \n");
    }
  // both wheel sensors may detect a line
  } else if (member[WHEEL_LEFT] == GREY &&
             member[WHEEL_RIGHT] == GREY) {
    // something is wrong -> stop
    copyRpmSpeed(rpmHalt, rpmFuzzyCtrl);
    urtPrintf("HALT: wheel line \n");
  // exactly one wheel sensor may detect a line
  } else if (member[WHEEL_LEFT] == GREY ||
             member[WHEEL_RIGHT] == GREY) {
    if (member[WHEEL_LEFT] == GREY) {
      // turn left
      copyRpmSpeed(rpmTurnLeft, rpmFuzzyCtrl);
      urtPrintf("TURN LEFT \n");
    } else if (member[WHEEL_RIGHT] == GREY) {
      // turn right
      copyRpmSpeed(rpmTurnRight, rpmFuzzyCtrl);
      urtPrintf("TURN RIGHT \n");
    }
  // no sensor detects anything
  } else {
    // line is lost -> stop
    copyRpmSpeed(rpmHalt, rpmFuzzyCtrl);
    urtPrintf("HALT: line lost \n");
  }

  return;
}

// Fuzzyfication of the sensor values
void fuzzyfication(uint16_t sensorValue, float* fuzziedValue) {
  if (sensorValue < blackStartFalling ) {
    // Only black value
    fuzziedValue[BLACK] = 1.0f;
    fuzziedValue[GREY] = 0.0f;
    fuzziedValue[WHITE] = 0.0f;
  } else if (sensorValue > whiteOn ) {
    // Only white value
    fuzziedValue[BLACK] = 0.0f;
    fuzziedValue[GREY] = 0.0f;
    fuzziedValue[WHITE] = 1.0f;
  } else if ( sensorValue < greyMax) {
    // Some greyisch value between black and grey

    // Black is going down
    if ( sensorValue > blackOff) {
      fuzziedValue[BLACK] = 0.0f;
    } else {
      fuzziedValue[BLACK] = (((float) (sensorValue-blackOff)) / ((float) (blackStartFalling-blackOff)));
    }
    // Grey is going up
    if ( sensorValue < greyStartRising) {
      fuzziedValue[GREY] = 0.0f;
    } else {
      fuzziedValue[GREY] = (((float) (sensorValue-greyStartRising)) / ((float) (greyMax-greyStartRising)));
    }
    // White is absent
    fuzziedValue[WHITE] = 0.0f;

  } else if ( sensorValue >= greyMax) {
    // Some greyisch value between grey and white

    // Black is absent
    fuzziedValue[BLACK] = 0.0f;
    // Grey is going down
    if ( sensorValue < greyOff) {
      fuzziedValue[GREY] = (((float) (sensorValue-greyOff)) / ((float) (greyMax-greyOff)));
    } else {
      fuzziedValue[GREY] = 0.0f;
    }
    // White is going up
    if ( sensorValue < whiteStartRising) {
      fuzziedValue[WHITE] = 0.0f;
    } else {
      fuzziedValue[WHITE] = (((float) (sensorValue-whiteStartRising)) / ((float) (whiteOn-whiteStartRising)));
    }
  }

  return;
}

void lineFollowing(linefollowing_node_t* linefollowing)
{
  // FUZZYFICATION
  // First we need to get the fuzzy value for our 3 values {BLACK, GREY, WHITE}
  float leftWheelFuzzyMemberValues[3], leftFrontFuzzyMemberValues[3], rightFrontFuzzyMemberValues[3], rightWheelFuzzyMemberValues[3];
  fuzzyfication(linefollowing->floor_data.data.sensors.left_wheel, leftWheelFuzzyMemberValues);
  fuzzyfication(linefollowing->floor_data.data.sensors.left_front, leftFrontFuzzyMemberValues);
  fuzzyfication(linefollowing->floor_data.data.sensors.right_front, rightFrontFuzzyMemberValues);
  fuzzyfication(linefollowing->floor_data.data.sensors.right_wheel, rightWheelFuzzyMemberValues);

  // INFERENCE RULE DEFINITION
  // Get the member for each sensor
  colorMember_t member[4];
  member[WHEEL_LEFT] = getMember(leftWheelFuzzyMemberValues);
  member[FRONT_LEFT] = getMember(leftFrontFuzzyMemberValues);
  member[FRONT_RIGHT] = getMember(rightFrontFuzzyMemberValues);
  member[WHEEL_RIGHT] = getMember(rightWheelFuzzyMemberValues);

  // DEFUZZYFICATION
  defuzzyfication(member, lf_rpmSpeed);
  return;
}

float getError(void* linefollowing){
  int threshWhite = 30000;
  int BThresh = 3000;
  int WThresh = 17000;

  // Get actual sensor data of both front sensors
  int FL = ((linefollowing_node_t*)linefollowing)->floor_data.data.sensors.left_front;
  int FR = ((linefollowing_node_t*)linefollowing)->floor_data.data.sensors.right_front;
  // urtPrintf("FL: %i, FR: %i\n", FL, FR);
  
  int targetL = BThresh;
  int targetR = WThresh;
  int error = 0;
  switch (((linefollowing_node_t*)linefollowing)->strategy)
  {
  case EDGE_RIGHT:
      error = (FL + FR - targetL - targetR) / 100;
      break;
  case EDGE_LEFT:
      error = (targetL + targetR - FL - FR) / 100;
      break;
  default:
      break;
  }
  // Register white values
  if (FL+FR > threshWhite){
      ef_whiteFlag += 1;
  } else if (ef_whiteFlag > 0) {
      ef_whiteFlag -= 1;
  }
  float result = (float) error;
  return (float) result;
}

float getPidCorrectionSpeed(void* linefollowing){
    float error = getError(linefollowing) / 100.0;
    float correctionSpeed = ef_K_p * error;
    
    return correctionSpeed;
}

void updateCorrectionSpeed(linefollowing_node_t* linefollowing)
{
  float correctionSpeed = 0;
  correctionSpeed = getPidCorrectionSpeed(linefollowing);
  // urtPrintf("correction speed: %.2f\n", correctionSpeed);
  // chprintf((BaseSequentialStream*) &SD1, "Correction: %d, thresh: %d\n",correctionSpeed,  global->threshWhite);

  // Positive lateral speed rotates left, negative speed rotates right
  const float rpmCorrectionSpeed[2] = {0.1, correctionSpeed};  

  copyRpmSpeed(rpmCorrectionSpeed, lf_rpmSpeed);
  return; 
}

urt_osEventMask_t _linefollowing_Setup(urt_node_t* node, void* linefollowing)
{
  urtDebugAssert(linefollowing != NULL);
  (void)node;

  // set thread name
  chRegSetThreadName(_linefollowing_name);

#if (URT_CFG_PUBSUB_ENABLED == true)
  // subscribe to the floor proximity topic
  urt_topic_t* const floor_data_topic = urtCoreGetTopic(((linefollowing_node_t*)linefollowing)->floor_data.topicid);
  urtDebugAssert(floor_data_topic != NULL);
  urtNrtSubscriberSubscribe(&((linefollowing_node_t*)linefollowing)->floor_data.nrt, floor_data_topic, PROXFLOOREVENT);

  //Subscribe to the touch sensor topic
  urt_topic_t* const touch_sensor_topic = urtCoreGetTopic(((linefollowing_node_t*)linefollowing)->environment_data.topicid);
  urtDebugAssert(touch_sensor_topic != NULL);
  urtNrtSubscriberSubscribe(&((linefollowing_node_t*)linefollowing)->environment_data.nrt, touch_sensor_topic, PROXENVEVENT);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // Start timer
  aosTimerPeriodicInterval(&((linefollowing_node_t*)linefollowing)->timer, (1.0f / 100.0f) * MICROSECONDS_PER_SECOND + 0.5f,
                         _linefollowing_triggercb, ((linefollowing_node_t*)linefollowing));

  return PROXFLOOREVENT | PROXENVEVENT;
}

/**
 * @brief   Loop callback function for linefollowing nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] linefollowing    Pointer to the maze structure.
 *                    Must nor be NULL.
 *
 * @return  Event mask to listen for next.
 */
urt_osEventMask_t _linefollowing_Loop(urt_node_t* node, urt_osEventMask_t event, void* linefollowing)
{
  urtDebugAssert(linefollowing != NULL);
  (void)node;

  // local constants
  linefollowing_node_t* const lfn = (linefollowing_node_t*)linefollowing;
  
  if (event & (PROXENVEVENT | PROXFLOOREVENT)) {
    // get the proximity and ambient data of the ring and floor sensors
    lf_getData(lfn, event);

    // Check for state change
    // Top view of the AMiRo ring sensors and their indices :
    //     _____
    //    / 0F7
    //   |1     6|
    //   |2     5|
    //    \_3B4_/
    //
    if ((lfn->environment_data.data.data[2] > 40000)
     && (lfn->environment_data.data.data[5] > 40000)) {
      if (lfn->state == IDLE) {
        if (lfn->strategy == CENTER) {
          urtPrintf("LINEFOLLOWING \n");
          lfn->state = LINEFOLLOWING;
        } else if (lfn->strategy == EDGE_LEFT || lfn->strategy == EDGE_RIGHT) {
          urtPrintf("EDGEFOLLOWING \n");
          lfn->state = EDGEFOLLOWING;
        }
      } else {
        urtPrintf("IDLE \n");
        copyRpmSpeed(rpmHalt, lf_rpmSpeed);
        lf_signalMotorService(linefollowing, lf_rpmSpeed);
        lfn->state = IDLE;
        urtThreadSSleep(0.75);
        // Shutdown the motor
        copyRpmSpeed(rpmStop, lf_rpmSpeed);
        lf_signalMotorService(linefollowing, lf_rpmSpeed);
      }
      //Wait a little bit so that the environment sensors are untouched
      urtThreadSSleep(2);
    } 
  }

  if (event & TIMER_TRIGGEREVENT) {
    // Do state action
      switch (lfn->state) {
      case IDLE:
        urtThreadSSleep(1);
        break;
      case LINEFOLLOWING:
        lineFollowing(linefollowing);
        lf_signalMotorService(linefollowing, lf_rpmSpeed);
        break;
      case EDGEFOLLOWING:
        updateCorrectionSpeed(linefollowing);
        lf_signalMotorService(linefollowing, lf_rpmSpeed);
        break;
      default:
        break;
      }
    event &= ~TIMER_TRIGGEREVENT;
  }

  return PROXFLOOREVENT | PROXENVEVENT | TIMER_TRIGGEREVENT;
}

/**
 * @brief   Shutdown callback function for maze nodes.
 *
 * @param[in] node    Pointer to the node object.
 *                    Must not be NULL.
 * @param[in] reason  Reason of the termination.
 * @param[in] linefollowing  Pointer to the maze structure
 *                           Must not be NULL.
 */
void _linefollowing_Shutdown(urt_node_t* node, urt_status_t reason, void* linefollowing)
{
  urtDebugAssert(linefollowing != NULL);

  (void)node;
  (void)reason;

#if (URT_CFG_PUBSUB_ENABLED == true)
  // unsubscribe from topics
  urtNrtSubscriberUnsubscribe(&((linefollowing_node_t*)linefollowing)->floor_data.nrt);
  urtNrtSubscriberUnsubscribe(&((linefollowing_node_t*)linefollowing)->environment_data.nrt);
#endif /* (URT_CFG_PUBSUB_ENABLED == true) */

  return;
}

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/
/**
 * @addtogroup apps_linefollowing
 * @{
 */

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
int linefollowing_ShellCallback_strategy(BaseSequentialStream* stream, int argc, const char* argv[], linefollowing_node_t* lf) {
  
  urtDebugAssert(lf != NULL);
  (void)argc;
  (void)argv;

  bool print_help = (argc < 2) || (argc > 3);
  if (!print_help) {
    int set_value = atoi(argv[1]);
    if (set_value == 0) {
      lf->strategy = (lf_strategy_t) set_value;
      chprintf(stream, "LineFollowing: set LineFollowing off.\n");
    } else if (set_value == 1){
      lf->strategy = (lf_strategy_t) set_value;
      chprintf(stream, "LineFollowing: set strategy to LineFollowing.\n");
      chprintf(stream, "!You must touch the 4 side sensors to start/end the linefollowing!\n");
    } else if (set_value == 2){
      lf->strategy = (lf_strategy_t) set_value;
      chprintf(stream, "LineFollowing: set strategy to EdgeFollowing on left edge.\n");
      chprintf(stream, "!You must touch the 4 side sensors to start/end the linefollowing!\n");
    } else if (set_value == 3){
      lf->strategy = (lf_strategy_t) set_value;
      chprintf(stream, "LineFollowing: set strategy to EdgeFollowing on right edge.\n");
      chprintf(stream, "!You must touch the 4 side sensors to start/end the linefollowing!\n");
    } else {
      print_help = true;
    }
  }

  if (print_help) {
    chprintf(stream, "Usage: %s STRATEGY\n", argv[0]);
    chprintf(stream, "STRATEGY:\n");
    chprintf(stream, "  Strategy to be set. Must be one of the following:\n");
    chprintf(stream, "  0\n");
    chprintf(stream, "    Deactivate all LineFollowing.\n");
    chprintf(stream, "  1\n");
    chprintf(stream, "    Activate LineFollowing using the Fuzzy strategy.\n");
    chprintf(stream, "  2\n");
    chprintf(stream, "    Activate EdgeFollowing on left edge.\n");
    chprintf(stream, "  3\n");
    chprintf(stream, "    Activate EdgeFollowing on right edge.\n");
    return AOS_INVALIDARGUMENTS;
  }
  return AOS_OK;
}
#endif /* (AMIROOS_CFG_SHELL_ENABLE == true) */


void linefollowingInit(linefollowing_node_t* linefollowing,
                    urt_serviceid_t dmc_target_serviceid,
                    urt_serviceid_t led_light_serviceid,
                    urt_topicid_t proximity_topicid,
                    urt_topicid_t environment_topicid,
                    urt_osThreadPrio_t prio)
{
  urtDebugAssert(linefollowing != NULL);

  linefollowing->state = IDLE;
  linefollowing->strategy = NONE;

  linefollowing->floor_data.topicid = proximity_topicid;
  linefollowing->environment_data.topicid = environment_topicid;

  linefollowing->floor_data.data_size = sizeof(((linefollowing_node_t*)linefollowing)->floor_data.data);
  linefollowing->environment_data.data_size = sizeof(((linefollowing_node_t*)linefollowing)->environment_data.data);

  // initialize the node
  urtNodeInit(&linefollowing->node, (urt_osThread_t*)linefollowing->thread, sizeof(linefollowing->thread), prio,
              _linefollowing_Setup, linefollowing,
              _linefollowing_Loop, linefollowing,
              _linefollowing_Shutdown, linefollowing);

  // initialize service requests
#if (URT_CFG_RPC_ENABLED == true)
  urtNrtRequestInit(&linefollowing->motor_data.request, &linefollowing->motor_data.data);
  urtNrtRequestInit(&linefollowing->light_data.request, &linefollowing->light_data.data);
  linefollowing->motor_data.service = urtCoreGetService(dmc_target_serviceid);
  linefollowing->light_data.serviceid = led_light_serviceid;
#endif /* (URT_CFG_RPC_ENABLED == true) */

  // initialize subscriber
#if (URT_CFG_PUBSUB_ENABLED == true)
  urtNrtSubscriberInit(&linefollowing->floor_data.nrt);
  urtNrtSubscriberInit(&linefollowing->environment_data.nrt);
#endif /* URT_CFG_PUBSUB_ENABLED == true */

  // initialize trigger timer
  aosTimerInit(&linefollowing->timer);

  return;
}

/** @} */
