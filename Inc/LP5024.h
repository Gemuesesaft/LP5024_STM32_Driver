/**
 ******************************************************************************
 * @file    LP5024.h
 * @version 1.0
 * @author  Till Heuer - EVE Audio GmbH
 * @brief   Headerfile for driver for Texas Instruments LP5024 LED driver IC.
 * @date 	Dec 7, 2023
 * @verbatim
 * Datasheet: https://www.ti.com/lit/ds/symlink/lp5024.pdf?ts=1706742536274
 * @endverbatim
 ******************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

	/** @addtogroup IC_Drivers
	 * @{
	 */

	/** @addtogroup LED_Driver
	 * @{
	 */

#ifndef CUSTOM_DRIVERS_INC_LP5024_H_
#define CUSTOM_DRIVERS_INC_LP5024_H_

#define STM32F1 // Select MCU

// Includes
/* For I2C and GPIO funcionality. */
#if defined(STM32WB)
#include "stm32wbxx_hal.h"
#elif defined(STM32F0)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L1)
#include "stm32l1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32L5)
#include "stm32l5xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32G4)
#include "stm32g4xx_hal.h"
#else
#error "Enter MCU"
#endif

#include <stdint.h> // For fixed width types.
#include <stdlib.h> // For descriptive return values.

#define LP5024_I2C_TIMEOUT (100)	  ///< Time before I2C timeout [ms].
#define LP5024_I2C_MAX_ATTEMPTS (3)	  ///< Number of attempts, before error.
#define LP5024_I2C_ATTEMPT_DELAY (10) ///< Time between attempts [ms].

// Error codes
#define LP5024_SUCCESS (0)		   ///< Error code for success.
#define LP5024_INPUTOUTOFRANGE (3) ///< Error code for wrong input.

/**
 * @brief Device address of LP5024 (7Bit Form).
 * Note that last two bits are configurable.
 * If A0 and A1 are connected to one of the following signals,
 * the last two bits are:
 * A0=GND, A1=GND -> 0b00;
 * A0=GND, A1=Vdd -> 0b01;
 * A0=Vdd, A1=GND -> 0b10;
 * A0=Vdd, A1=Vdd -> 0b11
 */
#define LP5024_ADDRESS (0x28)

// Register addresses
#define LP5024_REG_ENABLE (0x00) ///< Device enable register.

#define LP5024_REG_CONFIG (0x01) ///< Configuration register.

#define LP5024_REG_LED_CONF (0x02) ///< LED configuration register.

#define LP5024_REG_BRIGHT_TOT (0x03) ///< Total brightness register.

#define LP5024_REG_BRIGHT_BANK_A (0x04) ///< Bank A colour register.
#define LP5024_REG_BRIGHT_BANK_B (0x05) ///< Bank B colour register.
#define LP5024_REG_BRIGHT_BANK_C (0x06) ///< Bank C colour register.

#define LP5024_REG_BRIGHT_RGB_0 (0x07) ///< RGB LED 0 brightness register.
#define LP5024_REG_BRIGHT_RGB_1 (0x08) ///< RGB LED 1 brightness register.
#define LP5024_REG_BRIGHT_RGB_2 (0x09) ///< RGB LED 2 brightness register.
#define LP5024_REG_BRIGHT_RGB_3 (0x0A) ///< RGB LED 3 brightness register.
#define LP5024_REG_BRIGHT_RGB_4 (0x0B) ///< RGB LED 4 brightness register.
#define LP5024_REG_BRIGHT_RGB_5 (0x0C) ///< RGB LED 5 brightness register.
#define LP5024_REG_BRIGHT_RGB_6 (0x0D) ///< RGB LED 6 brightness register.
#define LP5024_REG_BRIGHT_RGB_7 (0x0E) ///< RGB LED 7 brightness register.

#define LP5024_REG_BRIGHT_LED_0 (0x0F)	///< LED 0 brightness register.
#define LP5024_REG_BRIGHT_LED_1 (0x10)	///< LED 1 brightness register.
#define LP5024_REG_BRIGHT_LED_2 (0x11)	///< LED 2 brightness register.
#define LP5024_REG_BRIGHT_LED_3 (0x12)	///< LED 3 brightness register.
#define LP5024_REG_BRIGHT_LED_4 (0x13)	///< LED 4 brightness register.
#define LP5024_REG_BRIGHT_LED_5 (0x14)	///< LED 5 brightness register.
#define LP5024_REG_BRIGHT_LED_6 (0x15)	///< LED 6 brightness register.
#define LP5024_REG_BRIGHT_LED_7 (0x16)	///< LED 7 brightness register.
#define LP5024_REG_BRIGHT_LED_8 (0x17)	///< LED 8 brightness register.
#define LP5024_REG_BRIGHT_LED_9 (0x18)	///< LED 9 brightness register.
#define LP5024_REG_BRIGHT_LED_10 (0x19) ///< LED 10 brightness register.
#define LP5024_REG_BRIGHT_LED_11 (0x1A) ///< LED 11 brightness register.
#define LP5024_REG_BRIGHT_LED_12 (0x1B) ///< LED 12 brightness register.
#define LP5024_REG_BRIGHT_LED_13 (0x1C) ///< LED 13 brightness register.
#define LP5024_REG_BRIGHT_LED_14 (0x1D) ///< LED 14 brightness register.
#define LP5024_REG_BRIGHT_LED_15 (0x1E) ///< LED 15 brightness register.
#define LP5024_REG_BRIGHT_LED_16 (0x1F) ///< LED 16 brightness register.
#define LP5024_REG_BRIGHT_LED_17 (0x20) ///< LED 17 brightness register.
#define LP5024_REG_BRIGHT_LED_18 (0x21) ///< LED 18 brightness register.
#define LP5024_REG_BRIGHT_LED_19 (0x22) ///< LED 19 brightness register.
#define LP5024_REG_BRIGHT_LED_20 (0x23) ///< LED 20 brightness register.
#define LP5024_REG_BRIGHT_LED_21 (0x24) ///< LED 21 brightness register.
#define LP5024_REG_BRIGHT_LED_22 (0x25) ///< LED 22 brightness register.
#define LP5024_REG_BRIGHT_LED_23 (0x26) ///< LED 23 brightness register.

#define LP5024_REG_RESET (0x27) ///< Reset register.

	/**
	 * @brief Enum for last two bits of device address.
	 */
	typedef enum
	{
		LP5024_A1_GND_A0_GND,
		LP5024_A1_GND_A0_VDD,
		LP5024_A1_VDD_A0_GND,
		LP5024_A1_VDD_A0_VDD
	} lp5024_A0_t;
	/**
	 * @brief Enum for dis-/enabling the IC.
	 *
	 */
	typedef enum
	{
		LP5024_DisableDevice,
		LP5024_EnableDevice
	} lp5024_Enable_t;
	/**
	 * @brief Enum for global LED control.
	 *
	 */
	typedef enum
	{
		LP5024_DisableGlobalOff,
		LP5024_EnableGlobalOff
	} lp5024_GlobalOff_t;
	/**
	 * @brief Enum for max. current.
	 *
	 */
	typedef enum
	{
		LP5024_MaxCurrent_26mA,
		LP5024_MaxCurrent_35mA
	} lp5024_MaxCurrent_t;
	/**
	 * @brief Enum for PWM dithering.
	 *
	 */
	typedef enum
	{
		LP5024_DisablePWMDithering,
		LP5024_EnablePWMDithering
	} lp5024_PWMDithering_t;
	/**
	 * @brief Enum for auto increment.
	 *
	 */
	typedef enum
	{
		LP5024_DisableAutoIncrement,
		LP5024_EnableAutoIncrement
	} lp5024_AutoIncrement_t;
	/**
	 * @brief Enum for auto power saving mode.
	 *
	 */
	typedef enum
	{
		LP5024_DisableAutoPowerSave,
		LP5024_EnableAutoPowerSave
	} lp5024_PowerSave_t;
	/**
	 * @brief Enum for log scale.
	 *
	 */
	typedef enum
	{
		LP5024_LinScale,
		LP5024_LogScale
	} lp5024_LogScale_t;
	/**
	 * @brief Enum for bank control.
	 *
	 */
	typedef enum
	{
		LP5024_IndividualControl,
		LP5024_BankControl
	} lp5024_BankControl_t;
	/**
	 * @brief Enum for banks of RGB LEDs.
	 *
	 */
	typedef enum
	{
		LP5024_Bank_A,
		LP5024_Bank_B,
		LP5024_Bank_C
	} lp5024_Banks_t;
	/**
	 * @brief Enum for RGB LED color order.
	 *
	 */
	typedef enum
	{
		LP5024_RGB,
		LP5024_RBG,
		LP5024_GRB,
		LP5024_GBR,
		LP5024_BGR,
		LP5024_BRG,
	} lp5024_ColorOrder_t;
	/**
	 * @brief Enum for RGB LEDs.
	 *
	 */
	typedef enum
	{
		LP5024_RGBLED_0,
		LP5024_RGBLED_1,
		LP5024_RGBLED_2,
		LP5024_RGBLED_3,
		LP5024_RGBLED_4,
		LP5024_RGBLED_5,
		LP5024_RGBLED_6,
		LP5024_RGBLED_7
	} lp5024_RGBLEDs_t;
	/**
	 * @brief Enum for LEDs.
	 *
	 */
	typedef enum
	{
		LP5024_LED_0,
		LP5024_LED_1,
		LP5024_LED_2,
		LP5024_LED_3,
		LP5024_LED_4,
		LP5024_LED_5,
		LP5024_LED_6,
		LP5024_LED_7,
		LP5024_LED_8,
		LP5024_LED_9,
		LP5024_LED_10,
		LP5024_LED_11,
		LP5024_LED_12,
		LP5024_LED_13,
		LP5024_LED_14,
		LP5024_LED_15,
		LP5024_LED_16,
		LP5024_LED_17,
		LP5024_LED_18,
		LP5024_LED_19,
		LP5024_LED_20,
		LP5024_LED_21,
		LP5024_LED_22,
		LP5024_LED_23
	} lp5024_LEDs_t;
	/**
	 * @brief Enum for LED state.
	 *
	 */
	typedef enum
	{
		LP5024_LED_Off,
		LP5024_LED_On
	} lp5024_LED_OnOff_t;

	/**
	 * @brief Struct for I2C handler and address pin status.
	 */
	typedef struct
	{
		I2C_HandleTypeDef *hi2c;
		lp5024_A0_t a0;
	} lp5024_Device_t;

	/**
	 * @brief 					Enables/Disables Chip.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	active			De-/Activates setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_Enable(lp5024_Device_t *device, lp5024_Enable_t active);
	/**
	 * @brief 					Sets LED global off.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	active 			De-/Activates setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetLEDGlobalOff(lp5024_Device_t *device, lp5024_LED_OnOff_t active);
	/**
	 * @brief			 		Sets maximum current.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	current 		Current setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetMaxCurrent(lp5024_Device_t *device, lp5024_MaxCurrent_t current);
	/**
	 * @brief 					Sets PWM dithering.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	active 			De-/Activates setting.
	 *
	 * @retval	uint8_t 		Error code.
	 */
	uint8_t LP5024_SetPWMDithering(lp5024_Device_t *device, lp5024_PWMDithering_t active);
	/**
	 * @brief 					Sets automatic increment mode.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	active 			De-/Activates setting.
	 *
	 * @retval	uint8_t 		Error code.
	 */
	uint8_t LP5024_SetAutoIncrement(lp5024_Device_t *device, lp5024_AutoIncrement_t active);
	/**
	 * @brief 					Sets automatic power save mode.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	active 			De-/Activates setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetAutoPowerSave(lp5024_Device_t *device, lp5024_PowerSave_t active);
	/**
	 * @brief 					Sets logarithmic dimming scale.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	active 			De-/Activates setting.
	 *
	 * @retval	uint8_t 		Error code.
	 */
	uint8_t LP5024_SetLogScale(lp5024_Device_t *device, lp5024_LogScale_t active);
	/**
	 * @brief 					Sets bank control.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	rgbLED			Selected RGB LED.
	 * @param 	active 			De-/Activates setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetBankControl(lp5024_Device_t *device, lp5024_RGBLEDs_t rgblED, lp5024_BankControl_t active);
	/**
	 * @brief 					Sets total brightness.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	brightness 		Brightness setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetTotalBrightness(lp5024_Device_t *device, uint8_t brightness);
	/**
	 * @brief 					Sets brightness of single RGB LED.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	rgbLED			Selected RGB LED.
	 * @param 	brightness 		Brightness setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetRGBLEDBrightness(lp5024_Device_t *device, lp5024_RGBLEDs_t rgbLED, uint8_t brightness);
	/**
	 * @brief 					Sets brightness of single LED.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	led				Selected LED.
	 * @param 	brightness 		Brightness setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetIndividualLEDBrightness(lp5024_Device_t *device, lp5024_LEDs_t led, uint8_t brightness);
	/**
	 * @brief 					Sets total colour in HSB Format.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	rgb				Order of colours.
	 * @param 	hue				Hue setting.
	 * @param 	saturation		Saturation setting.
	 * @param 	brightness 		Brightness setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetTotalColourHSB(lp5024_Device_t *device, uint8_t rgb, uint16_t hue, uint8_t saturation, uint8_t brightness);
	/**
	 * @brief 					Sets total colour in RGB Format.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	rgb				Order of colours.
	 * @param 	red				Red value.
	 * @param 	green			Green value.
	 * @param 	blue 			Blue value.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetTotalColourRGB(lp5024_Device_t *device, uint8_t rgb, uint8_t red, uint8_t green, uint8_t blue);
	/**
	 * @brief 					Sets LED colour in HSB Format.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	rgb				Order of colours.
	 * @param 	rgbLED			Selected LED.
	 * @param 	hue				Hue setting.
	 * @param 	saturation		Saturation setting.
	 * @param 	brightness 		Brightness setting.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetLEDColourHSB(lp5024_Device_t *device, uint8_t rgb, uint8_t rgbLED, uint16_t hue, uint8_t saturation, uint8_t brightness);
	/**
	 * @brief 					Sets LED colour in RGB Format.
	 *
	 * @param   device      	Struct with I2C handler and address pin status.
	 * @param 	rgb				Order of colours.
	 * @param 	rgbLED			Selected LED.
	 * @param 	red				Red value.
	 * @param 	green			Green value.
	 * @param 	blue 			Blue value.
	 *
	 * @retval 	uint8_t			Error code.
	 */
	uint8_t LP5024_SetLEDColourRGB(lp5024_Device_t *device, uint8_t rgb, uint8_t rgbLED, uint8_t red, uint8_t green, uint8_t blue);

	/**
	 * @}
	 */

	/**
	 * @}
	 */

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_DRIVERS_INC_LP5024_H_ */