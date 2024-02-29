/**
 ******************************************************************************
 * @file    LP5024.h
 * @version 2.0
 * @author  Till Heuer - EVE Audio GmbH
 * @brief   Driver for Texas Instruments LP5024 LED driver IC.
 * @date 	Dec 7, 2023
 ******************************************************************************
 */

/** @addtogroup IC_Drivers
 * @{
 */

/** @addtogroup LED_Driver
 * @{
 */

#include "LP5024.h"

uint8_t LP5024_ReadI2C(lp5024_Device_t *device, uint8_t regAdress, uint8_t *data)
{
	return HAL_I2C_Mem_Read(device->hi2c, (LP5024_ADDRESS + device->a0) << 1, regAdress, I2C_MEMADD_SIZE_8BIT, data, 1, LP5024_I2C_TIMEOUT);
}

uint8_t LP5024_WriteI2C(lp5024_Device_t *device, uint8_t regAdress, uint8_t *data)
{
	return HAL_I2C_Mem_Write(device->hi2c, (LP5024_ADDRESS + device->a0) << 1, regAdress, I2C_MEMADD_SIZE_8BIT, data, 1, LP5024_I2C_TIMEOUT);
}

void HSVtoRGB(uint8_t *red, uint8_t *green, uint8_t *blue, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
	float c, x, m;
	m = brightness; // Cast brightness to float.
	m /= 100;		// Normalise brightness.
	c = saturation; // Cast saturation to float.
	c /= 100;		// Normalise saturation.
	c *= m;			// Calculates c (saturation * brightness).
	m -= c;			// Calculates m (brightness - c).
	x = hue;		// Cast hue to float.
	/* Calculates x (c * (1 - |(hue/60) mod 2 -1|). */
	x = c * (1 - abs(((int)(x / 60) % 2) - 1));
	if (hue < 60)
	{
		*red = (uint8_t)((c + m) * 255);
		*green = (uint8_t)((x + m) * 255);
		*blue = (uint8_t)(m * 255);
	}
	else if (hue < 120)
	{
		*red = (uint8_t)((x + m) * 255);
		*green = (uint8_t)((c + m) * 255);
		*blue = (uint8_t)(m * 255);
	}
	else if (hue < 180)
	{
		*red = (uint8_t)(m * 255);
		*green = (uint8_t)((c + m) * 255);
		*blue = (uint8_t)((x + m) * 255);
	}
	else if (hue < 240)
	{
		*red = (uint8_t)(m * 255);
		*green = (uint8_t)((x + m) * 255);
		*blue = (uint8_t)((c + m) * 255);
	}
	else if (hue < 300)
	{
		*red = (uint8_t)((x + m) * 255);
		*green = (uint8_t)(m * 255);
		*blue = (uint8_t)((c + m) * 255);
	}
	else
	{
		*red = (uint8_t)((c + m) * 255);
		*green = (uint8_t)(m * 255);
		*blue = (uint8_t)((x + m) * 255);
	}
}

uint8_t LP5024_Enable(lp5024_Device_t *device, lp5024_Enable_t active)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (active > 1)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Shifts bit in right position. */
	active = active << 6;
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_ENABLE, &active);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetLEDGlobalOff(lp5024_Device_t *device, lp5024_LED_OnOff_t active)
{
	/* Holds data for i2c communication. */
	uint8_t data = 0;
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (active > 1)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Reads current setting of register. */
		status = LP5024_ReadI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Combines current value of register with value that has to be changed. */
	data = (data & (~0b1)) | active;
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetMaxCurrent(lp5024_Device_t *device, lp5024_MaxCurrent_t current)
{
	/* Holds data for i2c communication. */
	uint8_t data = 0;
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (current > 1)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Reads current setting of register. */
		status = LP5024_ReadI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Combines current value of register with value that has to be changed. */
	data = (data & (~(0b1 << 1))) | (current << 1);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetPWMDithering(lp5024_Device_t *device, lp5024_PWMDithering_t active)
{
	/* Holds data for i2c communication. */
	uint8_t data = 0;
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (active > 1)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Reads current setting of register. */
		status = LP5024_ReadI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Combines current value of register with value that has to be changed. */
	data = (data & (~(0b1 << 2))) | (active << 2);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetAutoIncrement(lp5024_Device_t *device, lp5024_AutoIncrement_t active)
{
	/* Holds data for i2c communication. */
	uint8_t data = 0;
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (active > 1)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Reads current setting of register. */
		status = LP5024_ReadI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Combines current value of register with value that has to be changed. */
	data = (data & (~(0b1 << 3))) | (active << 3);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetAutoPowerSave(lp5024_Device_t *device, lp5024_PowerSave_t active)
{
	/* Holds data for i2c communication. */
	uint8_t data = 0;
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (active > 1)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Reads current setting of register. */
		status = LP5024_ReadI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Combines current value of register with value that has to be changed. */
	data = (data & (~(0b1 << 4))) | (active << 4);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetLogScale(lp5024_Device_t *device, lp5024_LogScale_t active)
{
	/* Holds data for i2c communication. */
	uint8_t data = 0;
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (active > 1)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Reads current setting of register. */
		status = LP5024_ReadI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Combines current value of register with value that has to be changed. */
	data = (data & (~(0b1 << 5))) | (active << 5);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_CONFIG, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetBankControl(lp5024_Device_t *device, lp5024_RGBLEDs_t rgbLED, lp5024_BankControl_t active)
{
	/* Holds data for i2c communication. */
	uint8_t data = 0;
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (active > 1 || rgbLED > 7)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Reads current setting of register. */
		status = LP5024_ReadI2C(device, LP5024_REG_LED_CONF, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Combines current value of register with value that has to be changed. */
	data = (data & (~(0b1 << rgbLED))) | (active << rgbLED);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_LED_CONF, &data);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetTotalBrightness(lp5024_Device_t *device, uint8_t brightness)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_TOT, &brightness);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetRGBLEDBrightness(lp5024_Device_t *device, lp5024_RGBLEDs_t rgbLED, uint8_t brightness)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (rgbLED > 7)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_RGB_0 + rgbLED, &brightness);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetIndividualLEDBrightness(lp5024_Device_t *device, lp5024_LEDs_t led, uint8_t brightness)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	if (led > 23)
	{ /* Checks for input errors. */
		return LP5024_INPUTOUTOFRANGE;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_0 + led, &brightness);
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetTotalColourHSB(lp5024_Device_t *device, uint8_t rgb, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	/* Checks for input errors. */
	if (hue >= 360 || saturation > 100 || brightness > 100)
	{
		return LP5024_INPUTOUTOFRANGE;
	}
	HSVtoRGB(&red, &green, &blue, hue, saturation, brightness);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		if (rgb < 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_A, &red);
		}
		else if (rgb < 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_A, &green);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_A, &blue);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb == 0 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &green);
		}
		else if (rgb == 1 || rgb == 3)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &blue);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &red);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb == 0 || rgb == 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &blue);
		}
		else if (rgb == 3 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &red);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &green);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetTotalColourRGB(lp5024_Device_t *device, uint8_t rgb, uint8_t red, uint8_t green, uint8_t blue)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb < 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_A, &red);
		}
		else if (rgb < 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_A, &green);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_A, &blue);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb == 0 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &green);
		}
		else if (rgb == 1 || rgb == 3)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &blue);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_B, &red);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		/* Sends changed register settings to chip. */
		if (rgb == 0 || rgb == 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_C, &blue);
		}
		else if (rgb == 3 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_C, &red);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_BANK_C, &green);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetLEDColourHSB(lp5024_Device_t *device, uint8_t rgb, uint8_t rgbLED, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	/* Checks for input errors. */
	if (hue >= 360 || saturation > 100 || brightness > 100)
	{
		return LP5024_INPUTOUTOFRANGE;
	}
	HSVtoRGB(&red, &green, &blue, hue, saturation, brightness);
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb < 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_0 + (rgbLED * 3), &red);
		}
		else if (rgb < 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_0 + (rgbLED * 3), &green);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_0 + (rgbLED * 3), &blue);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb == 0 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_1 + (rgbLED * 3), &green);
		}
		else if (rgb == 1 || rgb == 3)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_1 + (rgbLED * 3), &blue);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_1 + (rgbLED * 3), &red);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb == 0 || rgb == 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_2 + (rgbLED * 3), &blue);
		}
		else if (rgb == 3 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_2 + (rgbLED * 3), &red);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_2 + (rgbLED * 3), &green);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

uint8_t LP5024_SetLEDColourRGB(lp5024_Device_t *device, uint8_t rgb, uint8_t rgbLED, uint8_t red, uint8_t green, uint8_t blue)
{
	/* Holds i2c status for error catching. */
	uint8_t status = 0;
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb < 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_0 + (rgbLED * 3), &red);
		}
		else if (rgb < 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_0 + (rgbLED * 3), &green);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_0 + (rgbLED * 3), &blue);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb == 0 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_1 + (rgbLED * 3), &green);
		}
		else if (rgb == 1 || rgb == 3)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_1 + (rgbLED * 3), &blue);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_1 + (rgbLED * 3), &red);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	/* Catches case when all attempts failed and returns last error code. */
	if (status > HAL_OK)
	{
		return status;
	}
	/* Repeats i2c call, in case of busy i2c unit. */
	for (uint8_t attempt = 0; attempt <= LP5024_I2C_MAX_ATTEMPTS; attempt++)
	{
		if (rgb == 0 || rgb == 2)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_2 + (rgbLED * 3), &blue);
		}
		else if (rgb == 3 || rgb == 4)
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_2 + (rgbLED * 3), &red);
		}
		else
		{ /* Sends changed register settings to chip. */
			status = LP5024_WriteI2C(device, LP5024_REG_BRIGHT_LED_2 + (rgbLED * 3), &green);
		}
		if (status == HAL_OK)
		{ /* Breaks out of loop if successful. */
			break;
		}
		else if (status == HAL_ERROR)
		{ /* Returns error if i2c unit fails. */
			return HAL_ERROR;
		}
		/* Delays next i2c call if first attempt failed. */
		HAL_Delay(LP5024_I2C_ATTEMPT_DELAY);
	}
	return status;
}

/**
 * @}
 */

/**
 * @}
 */