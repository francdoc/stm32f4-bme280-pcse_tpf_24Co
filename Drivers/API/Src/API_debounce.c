#include "API_debounce.h"

#define BUTTONled LED1

static debounceState_t currentButtonState;
static delay_t delayDebounce;
static bool_t wasPressed;

static const uint32_t DELAYdebounce = 40; // Debounce delay time in milliseconds.

// Private function prototypes
void buttonPressed();
void buttonReleased();
uint32_t getBtnState();
static void DebounceFsmErrorHandler();

/**
 * @brief  When called this function turns ON LED to indicate that the button is being pressed.
 * @retval None.
 */
void buttonPressed() {
	BSP_LED_On(BUTTONled);
}

/**
 * @brief  When called this function turns OFF LED to indicate that the button was released.
 * @retval None.
 */
void buttonReleased() {
	BSP_LED_Off(BUTTONled);
}

/**
 * @brief  Returns the state of user button.
 * @retval uint32_t: 0 if user button is not pressed, 1 if user button is
 * pressed.
 */
uint32_t getBtnState() {
	wasPressed = BSP_PB_GetState(BUTTON_USER);
	return wasPressed;
}

/**
 * @brief  Initializes button debouncing FSM with initial state = BUTTON_UP.
 * @retval None.
 */
void debounceFSM_init() {
	uartInit();
	BSP_LED_Init(BUTTONled);
	wasPressed = false;
	currentButtonState = BUTTON_UP;
    delayInit(&delayDebounce, DELAYdebounce);
}

/**
 * @brief  Function that tracks if the button was pressed or not.
 * @retval Returns true if button was pressed, false otherwise.
 */
bool_t readKey(){
	if (wasPressed == true) {
		wasPressed = false;
		return true;
	}
	return false;
}

/**
 * @brief  Updates the state of the button debounce finite state machine (FSM).
 *
 * This function must be called repeatedly to manage the transitions and states
 * of the button debounce FSM. It handles four states: BUTTON_UP, BUTTON_FALLING,
 * BUTTON_DOWN, and BUTTON_RAISING, ensuring that button presses and releases
 * are debounced properly.
 *
 * The debounceFSM_update function uses a delay mechanism to confirm state transitions.
 * When a button press or release is detected, it waits for a specified debounce time
 * to confirm the action, thus avoiding false triggers due to noise or quick transitions.
 *
 * @retval None
 */
void debounceFSM_update() {
  switch (currentButtonState) {
  case BUTTON_UP:
    if (getBtnState()) {
      currentButtonState = BUTTON_FALLING;
    }
    break;
  case BUTTON_FALLING:
	uartSendString((uint8_t*)"Flanco DESCENDENTE detectado\r\n");
    if (delayRead(&delayDebounce)) {
      if (getBtnState()) {
        buttonPressed();
        wasPressed = true;
        currentButtonState = BUTTON_DOWN;
      } else {
        currentButtonState = BUTTON_UP;
      }
    }
    break;
  case BUTTON_DOWN:
    if (!getBtnState()) {
      uartSendString((uint8_t*)"Flanco ASCENDENTE detectado\r\n");
      currentButtonState = BUTTON_RAISING;
    }
    break;
  case BUTTON_RAISING:
    if (delayRead(&delayDebounce)) {
      if (!getBtnState()) {
        buttonReleased();
        currentButtonState = BUTTON_UP;
      } else {
        currentButtonState = BUTTON_DOWN;
      }
    }
    break;
  default:
	  DebounceFsmErrorHandler();
    break;
  }
}

/**
 * @brief Handles invalid case in button debouncing FSM.
 */
static void DebounceFsmErrorHandler()
{
    BSP_LED_On(LED3);
    while (1)
    {
        __NOP();
    }
}
