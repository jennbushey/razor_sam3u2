/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Demonstrate various button API functions */
static void UserApp1SM_Idle(void)
{
  static bool bYellowBlink = FALSE;
 
  static LedRateType aeBlinkRate[] = {LED_1HZ, LED_2HZ, LED_4HZ, LED_8HZ};
  static u8 u8BlinkRateIndex = 0;
  static bool bLedBlink = FALSE;
  
  // works
  if( IsButtonPressed(BUTTON0) )
  {
    /* The button is currently pressed, so make sure the LED is on */
    LedOn(WHITE);
  }
  else
  {
    /* The button is not pressed, so make sure the LED is off */
    LedOff(WHITE);
  }
  
  
  /* Start the YELLOW LED blinking if BUTTON1 was pressed since last check;
  Turn the YELLOW LED off it was already blinking. */  
  /* if the LED is off, BUTTON1 starts it blinking at the current rate;
  If the LED is already on, turn it off. */
  if( WasButtonPressed(BUTTON1) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON1);

    /* If the LED is already blinking, toggle it off */
    if(bLedBlink)
    {
      //bYellowBlink = FALSE;
      bLedBlink = FALSE;
      LedOff(YELLOW);
      LedOff(PURPLE);
    }
    else
    {
     /* start blinking the LED at the current rate */
      //bYellowBlink = TRUE;
      bLedBlink=TRUE;
      LedBlink(YELLOW, aeBlinkRate[u8BlinkRateIndex]);
      LedOn(PURPLE);
    }
  }
 
    
  if( WasButtonPressed(BUTTON2) )
  {
    /* Be sure to acknowledge the button press */
    ButtonAcknowledge(BUTTON2);
    if(bLedBlink)
    {
      LedOff(BLUE);
      u8BlinkRateIndex++;
      if(u8BlinkRateIndex == 4)
      {
        u8BlinkRateIndex = 0;
      }
      /* Request the rate update */
      LedBlink(YELLOW, aeBlinkRate[u8BlinkRateIndex]);
    }
    else{
      LedOn(BLUE);
    }
  } /* END if(WasButtonPressed(BUTTON2))*/

  /* Turn on the CYAN LED if BUTTON3 has been held for 2 seconds. 
  The LED should turn off once the button is released. */
  //works
  if( IsButtonHeld(BUTTON3, 2000) )
  {
    LedOn(CYAN);
  }
  else
  {
    LedOff(CYAN);
  }


} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
