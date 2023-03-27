#include "AMS5915.h"

HAL_StatusTypeDef AMS5915_Init(AMS5915 *ams, I2C_HandleTypeDef *hi2c)
{
	if(!ams && !hi2c) return HAL_ERROR;
	ams->hi2c = hi2c;
	ams->hi2c->Devaddress = AMS5915_ADDR << 1;
	return HAL_I2C_IsDeviceReady(hi2c, ams->hi2c->Devaddress, 3, 500);
}

HAL_StatusTypeDef AMS5915_Read(AMS5915 *ams)
{
	if(!ams) return HAL_ERROR;
	return HAL_I2C_Master_Receive_IT(ams->hi2c, ams->hi2c->Devaddress & I2C_READ, ams->buf, sizeof(ams->buf));
}
