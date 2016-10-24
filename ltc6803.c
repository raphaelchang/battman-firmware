#include "ltc6803.h"
#include "ch.h"
#include "hal.h"

#define PEC_POLY 7

/*
 *  * SPI configuration (562kHz, CPHA=1, CPOL=1, MSb first).
 *   */
static const SPIConfig ls_spicfg = {
    NULL,
    GPIOA,
    4,
    SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_CPOL | SPI_CR1_CPHA,
    SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
};

static uint8_t total_ic = 1;
static uint8_t pec8_calc(uint8_t len, uint8_t *data);
static void spi_sw_transfer(char *in_buf, const char *out_buf, int length);

void ltc6803_init(void)
{
    palSetPadMode(SCK_GPIO, SCK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(MISO_GPIO, MISO_PIN, PAL_MODE_INPUT_PULLUP | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(MOSI_GPIO, MOSI_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
}

//Function that calculates PEC byte
static uint8_t pec8_calc(uint8_t len, uint8_t *data)
{
    uint8_t  remainder = 0x41;//PEC_SEED;
    int byte;
    uint8_t bit;
    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < len; ++byte)
    {
	/*
	 * Bring the next byte into the remainder.
	 */
	remainder ^= data[byte];

	/*
	 * Perform modulo-2 division, a bit at a time.
	 */
	for (bit = 8; bit > 0; --bit)
	{
	    /*
	     * Try to divide the current data bit.
	     */
	    if (remainder & 128)
	    {
		remainder = (remainder << 1) ^ PEC_POLY;
	    }
	    else
	    {
		remainder = (remainder << 1);
	    }
	}
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);

}

void ltc6803_wrcfg(uint8_t config[][6])
{
    uint8_t BYTES_IN_REG = 6;
    uint8_t CMD_LEN = 2+(7*total_ic);
    uint8_t cmd[9];
    uint8_t rxbuf[9];
    uint16_t cfg_pec;
    uint8_t cmd_index;
    uint8_t current_ic;
    uint8_t current_byte;

    cmd[0] = 0x01;
    cmd[1] = 0xc7;

    cmd_index = 2;
    for (current_ic = total_ic; current_ic > 0; current_ic--)
    {

	for (current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
	{
	    cmd[cmd_index] = config[current_ic-1][current_byte];
	    cmd_index = cmd_index + 1;
	}

	cfg_pec = pec8_calc(BYTES_IN_REG, &config[current_ic-1][0]);    // calculating the PEC for each ICs configuration register data
	cmd[cmd_index] = (uint8_t)cfg_pec;
	cmd_index = cmd_index + 1;
    }

    spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
    spiStart(&SPID1, &ls_spicfg);       /* Setup transfer parameters.       */
    spiSelect(&SPID1);                  /* Slave Select assertion.          */
    /*spiExchange(&SPID1, CMD_LEN,*/
	    /*cmd, rxbuf);          [> Atomic transfer operations.      <]*/
    spi_sw_transfer(rxbuf, cmd, CMD_LEN);

    spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
    spiReleaseBus(&SPID1);              /* Ownership release.               */
}

void ltc6803_stcvad(void)
{
    uint8_t txbuf[2];
    uint8_t rxbuf[2];
    txbuf[0] = 0x10;
    txbuf[1] = 0xB0;
    spiAcquireBus(&SPID1);
    spiStart(&SPID1, &ls_spicfg);
    spiSelect(&SPID1);
    /*spiExchange(&SPID1, 2, txbuf, rxbuf);*/
    spi_sw_transfer(rxbuf, txbuf, 2);
    spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
    spiReleaseBus(&SPID1);              /* Ownership release.               */
}

void ltc6803_rdcv(uint16_t cells[][12])
{
    int data_counter = 0;
    int pec_error = 0;
    uint8_t data_pec = 0;
    uint8_t received_pec = 0;
    uint8_t txbuf[2];
    uint8_t rxbuf[2];
    uint8_t rx_data[19];
    txbuf[0] = 0x04;
    txbuf[1] = 0xDC;

    spiAcquireBus(&SPID1);
    spiStart(&SPID1, &ls_spicfg);
    spiSelect(&SPID1);
    /*spiExchange(&SPID1, 2, txbuf, rxbuf);*/
    spi_sw_transfer(rxbuf, txbuf, 2);
    txbuf[0] = 0xFF;
    for (int i = 0; i < total_ic; i++)
    {
	for (int j = 0; j < 19 ; j++)
	{
            /*spiExchange(&SPID1, 1, txbuf, rxbuf);*/
            spi_sw_transfer(rxbuf, txbuf, 1);
	    rx_data[data_counter++] = rxbuf[0];
	}
    }
    spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
    spiReleaseBus(&SPID1);              /* Ownership release.               */

    int cell_counter = 0;
    data_counter = 0;
    uint16_t temp, temp2;

    for (int j = 0; j < total_ic; j++)
    {

	received_pec = rx_data[18 +(19 * j)];
	data_pec = pec8_calc(18, &rx_data[(19 * j)]);
	if (received_pec != data_pec)
	{
	    pec_error = -1;
	}

	for (int k = 0; k < 12; k = k + 2)
	{

	    temp = rx_data[data_counter++];

	    temp2 = (uint16_t)(rx_data[data_counter] & 0x0F) << 8;

	    cells[j][k] = temp + temp2 - 512;
	    temp2 = (rx_data[data_counter++]) >> 4;

	    temp =  (rx_data[data_counter++]) << 4;

	    cells[j][k + 1] = temp + temp2 - 512;
	}

    }
}

static void spi_sw_transfer(char *in_buf, const char *out_buf, int length) {
    palSetPad(SCK_GPIO, SCK_PIN);
    chThdSleepMicroseconds(100);

    for (int i = 0;i < length;i++) {
        unsigned char send = out_buf ? out_buf[i] : 0;
        unsigned char recieve = 0;

        for (int bit=0;bit < 8;bit++) {
            palClearPad(SCK_GPIO, SCK_PIN);
            palWritePad(MOSI_GPIO, MOSI_PIN, send >> 7);
            send <<= 1;

            chThdSleepMicroseconds(100);

            recieve <<= 1;
            if (palReadPad(MISO_GPIO, MISO_PIN)) {
                recieve |= 0x1;
            }

            palSetPad(SCK_GPIO, SCK_PIN);
            chThdSleepMicroseconds(100);
        }

        if (in_buf) {
            in_buf[i] = recieve;
        }
    }
}

