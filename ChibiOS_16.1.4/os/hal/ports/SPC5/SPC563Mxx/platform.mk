# List of all the SPC563Mxx platform files.
PLATFORMSRC = ${CHIBIOS}/os/hal/ports/SPC5/SPC563Mxx/hal_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/DSPI_v1/spi_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/EDMA_v1/spc5_edma.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/EQADC_v1/adc_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/eMIOS200_v1/spc5_emios.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/eMIOS200_v1/icu_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/eMIOS200_v1/pwm_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/FlexCAN_v1/can_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/ESCI_v1/serial_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/SIU_v1/pal_lld.c \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/STM_v1/st_lld.c

# Required include directories
PLATFORMINC = ${CHIBIOS}/os/hal/ports/SPC5/SPC563Mxx \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/DSPI_v1 \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/EDMA_v1 \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/EQADC_v1 \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/eMIOS200_v1 \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/FlexCAN_v1 \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/ESCI_v1 \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/SIU_v1 \
              ${CHIBIOS}/os/hal/ports/SPC5/SPC5xx/STM_v1
