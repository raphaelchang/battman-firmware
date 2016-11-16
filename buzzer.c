#include "buzzer.h"
#include "hal.h"
#include "hw_conf.h"
#include "stm32f30x_conf.h"

#define SYSTEM_CORE_CLOCK 72000000

static float midi[120] = {16.3515978313, 17.3239144361, 18.3540479948, 19.4454364826, 20.6017223071, 21.8267644646, 23.1246514195, 24.4997147489, 25.9565435987, 27.5, 29.1352350949, 30.8677063285, 32.7031956626, 34.6478288721, 36.7080959897, 38.8908729653, 41.2034446141, 43.6535289291, 46.249302839, 48.9994294977, 51.9130871975, 55.0, 58.2704701898, 61.735412657, 65.4063913251, 69.2956577442, 73.4161919794, 77.7817459305, 82.4068892282, 87.3070578583, 92.4986056779, 97.9988589954, 103.826174395, 110.0, 116.54094038, 123.470825314, 130.81278265, 138.591315488, 146.832383959, 155.563491861, 164.813778456, 174.614115717, 184.997211356, 195.997717991, 207.65234879, 220.0, 233.081880759, 246.941650628, 261.625565301, 277.182630977, 293.664767917, 311.126983722, 329.627556913, 349.228231433, 369.994422712, 391.995435982, 415.30469758, 440.0, 466.163761518, 493.883301256, 523.251130601, 554.365261954, 587.329535835, 622.253967444, 659.255113826, 698.456462866, 739.988845423, 783.990871963, 830.60939516, 880.0, 932.327523036, 987.766602512, 1046.5022612, 1108.73052391, 1174.65907167, 1244.50793489, 1318.51022765, 1396.91292573, 1479.97769085, 1567.98174393, 1661.21879032, 1760.0, 1864.65504607, 1975.53320502, 2093.0045224, 2217.46104781, 2349.31814334, 2489.01586978, 2637.0204553, 2793.82585146, 2959.95538169, 3135.96348785, 3322.43758064, 3520.0, 3729.31009214, 3951.06641005, 4186.00904481, 4434.92209563, 4698.63628668, 4978.03173955, 5274.04091061, 5587.65170293, 5919.91076339, 6271.92697571, 6644.87516128, 7040.0, 7458.62018429, 7902.1328201, 8372.01808962, 8869.84419126, 9397.27257336, 9956.06347911, 10548.0818212, 11175.3034059, 11839.8215268, 12543.8539514, 13289.7503226, 14080.0, 14917.2403686, 15804.2656402};

void buzzer_init(void)
{
    TIM_DeInit(TIM15);
    TIM15->CNT = 0;
    rccEnableAPB2(RCC_APB2Periph_TIM15, false);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 0;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = SYSTEM_CORE_CLOCK / 4000;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM15, &timerInitStructure);
    TIM_Cmd(TIM15, ENABLE);

    TIM_OCInitTypeDef outputChannelInit = {0,};
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = 0;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC2Init(TIM15, &outputChannelInit);
    TIM_OC2PreloadConfig(TIM15, TIM_OCPreload_Enable);
    TIM15->CCR2 = 0;
    TIM_CtrlPWMOutputs(TIM15, ENABLE);
}

void buzzer_play_note(note_t note, uint16_t duration)
{
    buzzer_set_frequency(midi[note]);
    chThdSleepMilliseconds(duration);
}

void buzzer_play_rest(uint16_t duration)
{
    buzzer_set_frequency(0);
    chThdSleepMilliseconds(duration);
}

void buzzer_set_frequency(float freq)
{
    if (freq == 0)
    {
        TIM15->CCR2 = 0;
    }
    else
    {
        if (freq < 100)
        {
            TIM15->PSC = 31;
            TIM15->ARR = (int32_t)(SYSTEM_CORE_CLOCK / (freq * 32));
        } else if (freq < 200)
        {
            TIM15->PSC = 15;
            TIM15->ARR = (int32_t)(SYSTEM_CORE_CLOCK / (freq * 16));
        } else if (freq < 400)
        {
            TIM15->PSC = 7;
            TIM15->ARR = (int32_t)(SYSTEM_CORE_CLOCK / (freq * 8));
        } else if (freq < 700)
        {
            TIM15->PSC = 3;
            TIM15->ARR = (int32_t)(SYSTEM_CORE_CLOCK / (freq * 4));
        } else if (freq < 1300)
        {
            TIM15->PSC = 1;
            TIM15->ARR = (int32_t)(SYSTEM_CORE_CLOCK / (freq * 2));
        } else
        {
            TIM15->PSC = 0;
            TIM15->ARR = (int32_t)(SYSTEM_CORE_CLOCK / freq);
        }
        TIM15->CCR2 = TIM15->ARR / 2;
    }
}
