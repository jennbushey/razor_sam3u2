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
#include <stdio.h>

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

volatile uint32_t timer_ticks = 0;
volatile uint8_t timer_running = 0;
#define BUZZER_DURATION_MS 1000

// Define the frequency for the kitchen buzzer tone (in Hz)
#define TONE_KITCHEN_BUZZER   1500

// Define the duration for the kitchen buzzer tone (in milliseconds)
#define DURATION_KITCHEN_BUZZER   3000


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
    // Initialize LCD Screen
    u8 au8MessageWelcome[] = "Welcome! Timer Ready";
    LcdMessage(LINE1_START_ADDR, au8MessageWelcome);
    
    // Initialize Buzzer
    PWMAudioSetFrequency(BUZZER1, 500);
    
    // Initialize heartbeat LED
    HEARTBEAT_OFF();
    
    // Initialize LEDs
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
    LcdCommand(LCD_HOME_CMD);
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

// Define a delay function using a loop
void Delay(uint32_t milliseconds)
{
    volatile uint32_t counter = 0;
    for (uint32_t i = 0; i < milliseconds * 1000; i++)
    {
        counter++; // Increment the counter to consume CPU cycles
    }
}

void UpdateTime(u8 *au8Time, u8 u8Hour, u8 u8Minute, u8 u8Second)
{
    au8Time[0] = (u8Hour / 10) + '0';
    au8Time[1] = (u8Hour % 10) + '0';
    au8Time[3] = (u8Minute / 10) + '0';
    au8Time[4] = (u8Minute % 10) + '0';
    au8Time[6] = (u8Second / 10) + '0';
    au8Time[7] = (u8Second % 10) + '0';
} 

// Function to play the kitchen buzzer tone
void PlayKitchenBuzzer(void)
{
    PWMAudioSetFrequency(BUZZER1, TONE_KITCHEN_BUZZER);
    PWMAudioOn(BUZZER1);
    Delay(DURATION_KITCHEN_BUZZER);
    PWMAudioOff(BUZZER1);
    Delay(DURATION_KITCHEN_BUZZER);
    LedOff(GREEN);
}


static void UserApp1SM_Idle(void)
{
    static u8 au8Time[] = "00:00:00             ";
    static bool bIsCountingDown = FALSE;
    static u8 u8Hour = 0;
    static u8 u8Minute = 0;
    static u8 u8Second = 0;
    static u16 u16Counter = 0;
    static bool bLightIsOn = FALSE;  // Variable to hold the heartbeat
    
    // Keep track of the loop
    u16Counter++;
    
    // Turn buzzer off 
    PWMAudioOff(BUZZER1);
    
    // Button 0 for HOURS
    if (IsButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);
        LedOn(WHITE);
        LedOff(GREEN);
        if(u16Counter % 6 == 0) u8Hour++;
        if (u8Hour > 23)
        {
            u8Hour = 0;
        }
        UpdateTime(au8Time, u8Hour, u8Minute, u8Second);
        LcdMessage(LINE1_START_ADDR, au8Time);
        Delay(100);
    } else {
        LedOff(WHITE);
    }
    

    // Button 1 for MINUTES
    if (IsButtonPressed(BUTTON1))
    {
        ButtonAcknowledge(BUTTON1);
        LedOn(PURPLE);
        LedOff(GREEN);
        if(u16Counter % 6 == 0) u8Minute++;
        if (u8Minute > 59)
        {
            u8Minute = 0;
        }
        UpdateTime(au8Time, u8Hour, u8Minute, u8Second);
        LcdMessage(LINE1_START_ADDR, au8Time);
        Delay(100);
    } else {
        LedOff(PURPLE);
    }
    
    // Button 2 for SECONDS
    if (IsButtonPressed(BUTTON2))
    {
        ButtonAcknowledge(BUTTON2);
        if(u16Counter % 6 == 0) u8Second = u8Second + 10;
        LedOn(BLUE);
        LedOff(GREEN);
        if (u8Second > 59)
        {
            u8Second = 0;
        }
        UpdateTime(au8Time, u8Hour, u8Minute, u8Second);
        LcdMessage(LINE1_START_ADDR, au8Time);
        Delay(100);
    } else {
        LedOff(BLUE);
    }
    
    
    
    // Handle Button3 to start the countdown
    if(IsButtonPressed(BUTTON3))
    {
        ButtonAcknowledge(BUTTON3);
        bIsCountingDown = TRUE;
    }
    
    
    if(u16Counter == U16_COUNTER_PERIOD_MS){
        u16Counter = 0;
        if(bLightIsOn){
            HEARTBEAT_OFF();
            bLightIsOn = FALSE;
        }
        else {
            // HEARTBEAT_ON();
            bLightIsOn = TRUE;
            if(bIsCountingDown){
                HEARTBEAT_ON();
                
                // Update the countdown values
                if (u8Second > 0)
                    u8Second--;
                else if (u8Minute > 0)
                {
                    u8Minute--;
                    u8Second = 59;
                }
                else if (u8Hour > 0)
                {
                    u8Hour--;
                    u8Minute = 59;
                    u8Second = 59;
                }
                UpdateTime(au8Time, u8Hour, u8Minute, u8Second);
                LcdMessage(LINE1_START_ADDR, au8Time);
            }
        }
    }
    
    // Countdown completed, reset the flag
    if(bIsCountingDown == TRUE && u8Hour == 0 && u8Minute == 0 && u8Second == 0){
        bIsCountingDown = FALSE;
        LcdMessage(LINE1_START_ADDR, "   *****DONE!*****       ");
        LedOn(GREEN);
        HEARTBEAT_OFF();
        
        // Sound the buzzer 
        PlayKitchenBuzzer();
        
        }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
    
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
