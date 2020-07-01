#include "i2c.h"

static uint8_t i2c_tx_buf[I2C_MAX_LEN];
static uint8_t i2c_rx_buf[I2C_MAX_LEN];

int32_t i2c_write(int* fD, uint8_t reg, uint8_t* buf, uint16_t len) {
	if (buf == NULL)
		return -1;

	if (len > (I2C_MAX_LEN - 1))
		return -1;

	i2c_tx_buf[0] = reg;
	if (buf && len)
		memcpy(&i2c_tx_buf[1], buf, len);
	mtk_os_hal_i2c_write(i2c_port_num, i2c_lsm6dso_addr, i2c_tx_buf, len + 1);
	return 0;
}

int32_t i2c_read(int* fD, uint8_t reg, uint8_t* buf, uint16_t len) {
	if (buf == NULL)
		return -1;

	if (len > (I2C_MAX_LEN))
		return -1;

	mtk_os_hal_i2c_write_read(i2c_port_num, i2c_lsm6dso_addr,
		&reg, i2c_rx_buf, 1, len);
	memcpy(buf, i2c_rx_buf, len);
	return 0;
}

void i2c_enum(void) {
	uint8_t i;
	uint8_t data;

	printf("[ISU%d] Enumerate I2C Bus, Start\n", i2c_port_num);
	for (i = 0; i < 0x80; i += 2) {
		printf("[ISU%d] Address:0x%02X, ", i2c_port_num, i);
		if (mtk_os_hal_i2c_read(i2c_port_num, i, &data, 1) == 0)
			printf("Found 0x%02X\n", i);
	}
	printf("[ISU%d] Enumerate I2C Bus, Finish\n\n", i2c_port_num);
}

int i2c_init(void) {
	/* Allocate I2C buffer */
	//i2c_tx_buf = pvPortMalloc(I2C_MAX_LEN);
	//i2c_rx_buf = pvPortMalloc(I2C_MAX_LEN);
	if (i2c_tx_buf == NULL || i2c_rx_buf == NULL) {
		printf("Failed to allocate I2C buffer!\n");
		return -1;
	}

	/* MT3620 I2C Init */
	mtk_os_hal_i2c_ctrl_init(i2c_port_num);
	mtk_os_hal_i2c_speed_init(i2c_port_num, i2c_speed);

	return 0;
}