#include "spi_dev.h"
#include "ina229.h"

/*
╔════════════════╤═══════════════════╤════════════════════╤════════════════════╤════════════════════╗
║ ADC CONVERSION │   OUTPUT SAMPLE   │   OUTPUT SAMPLE    │  NOISE-FREE ENOB   │  NOISE-FREE ENOB   ║
║  TIME PERIOD   │AVERAGING [SAMPLES]│     PERIOD[ms]     │    (±163.84-mV)    │     (±40.96-mV)    ║
║     [µs]       │                   │                    │    (ADCRANGE=0)    │    (ADCRANGE = 1)  ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │0.05                │12.4                │10.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │0.084               │12.6                │10.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │0.15                │13.3                │11.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │0.28                │13.8                │11.8                ║
╟────────────────┤         1         ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │0.54                │14.2                │12.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │1.052               │14.5                │12.6                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │2.074               │15.3                │13.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │4.12                │16.0                │13.8                ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │0.2                 │13.1                │11.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │0.336               │13.9                │11.8                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │0.6                 │14.3                │12.2                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │1.12                │14.9                │12.8                ║
╟────────────────┤         4         ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │2.16                │15.1                │13.0                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │4.208               │15.8                │13.8                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │8.296               │16.1                │14.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │16.48               │16.5                │14.4                ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │0.8                 │13.9                │12.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │1.344               │14.7                │12.9                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │2.4                 │15.1                │13.0                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │4.48                │15.8                │13.7                ║
╟────────────────┤        16         ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │8.64                │16.3                │14.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │16.832              │16.5                │14.6                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │33.184              │17.1                │15.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │65.92               │17.7                │15.9                ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │3.2                 │15.0                │13.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │5.376               │15.9                │13.8                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │9.6                 │16.4                │14.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │17.92               │16.9                │14.5                ║
╟────────────────┤        64         ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │34.56               │17.7                │15.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │67.328              │17.7                │15.9                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │132.736             │18.1                │16.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │263.68              │18.7                │16.5                ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │6.4                 │15.5                │13.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │10.752              │16.3                │14.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │19.2                │16.9                │14.7                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │35.84               │17.1                │15.2                ║
╟────────────────┤        128        ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │69.12               │18.1                │15.9                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │134.656             │18.1                │16.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │265.472             │18.7                │16.9                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │527.36              │19.7                │17.1                ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │12.8                │15.5                │14.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │21.504              │16.7                │14.7                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │38.4                │17.4                │15.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │71.68               │17.7                │15.7                ║
╟────────────────┤        256        ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │138.24              │18.7                │16.1                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │269.312             │18.7                │16.7                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │530.944             │19.7                │17.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │1054.72             │19.7                │17.7                ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │25.6                │16.7                │14.3                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │43                  │17.4                │15.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │76.8                │17.7                │15.5                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │143.36              │18.7                │16.3                ║
╟────────────────┤        512        ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │276.48              │18.7                │16.5                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │538.624             │19.7                │17.4                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │1061.888            │19.7                │17.7                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │2109.44             │19.7                │18.7                ║
╟────────────────┼───────────────────┼────────────────────┼────────────────────┼────────────────────╢
║50              │                   │51.2                │17.1                │15.0                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║84              │                   │86.016              │17.7                │15.9                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║150             │                   │153.6               │18.1                │16.0                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║280             │                   │286.72              │18.7                │16.9                ║
╟────────────────┤       1024        ├────────────────────┼────────────────────┼────────────────────╢
║540             │                   │552.96              │19.7                │17.1                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║1052            │                   │1077.248            │19.7                │17.7                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║2074            │                   │2123.776            │19.7                │18.1                ║
╟────────────────┤                   ├────────────────────┼────────────────────┼────────────────────╢
║4120            │                   │4218.88             │20                  │18.7                ║
╚════════════════╧═══════════════════╧════════════════════╧════════════════════╧════════════════════╝

╔════════════════════╤═════════════════════════╤════════════════════╗
║   PARAMETER        │    FULL SCALE VALUE     │     RESOLUTION     ║
╟────────────────────┼─────────────────────────┼────────────────────╢
║                    │±163.84 mV (ADCRANGE = 0)│312.5 nV/LSB        ║
║    Shunt voltage   ├─────────────────────────┼────────────────────╢
║                    │ ±40.96 mV (ADCRANGE = 1)│78.125 nV/LSB       ║
╟────────────────────┼─────────────────────────┼────────────────────╢
║    Bus voltage     │0 V to 85 V              │195.3125 μV/LSB     ║
╟────────────────────┼─────────────────────────┼────────────────────╢
║    Temperature     │–40°C to +125°C          │7.8125 m°C/LSB      ║
╚════════════════════╧═════════════════════════╧════════════════════╝
*/

#define DIV_ROUND_CLOSEST(__DIVIDEND__, __DIVISOR__) (((__DIVIDEND__) + ((__DIVISOR__) / 2)) / (__DIVISOR__))

constexpr float ShuntVoltage163_84mV_LSB = 0.0000003125f;
constexpr float ShuntVoltage40_96mV_LSB =  0.000000078125f;
constexpr float BusVoltageLSB = 0.0001953125f;
constexpr float TemperatureLSB = 0.0078125f;

constexpr float shuntVoltageLSB(ADCRANGE adcRange) { return adcRange == ADCRANGE::_163_84mV ? ShuntVoltage163_84mV_LSB : ShuntVoltage40_96mV_LSB; }

spi_dev s;
struct ina229::Data ina229::data;
ADCRANGE ina229::adcRange_;
float ina229::shuntRes_;
float ina229::currentLsb;

ina229::ina229()
{
}

void ina229::Reset()
{
    //uint16_t cfg = 0;

    /*  Read programmed configuration  */
    //cfg = s.readRegister_16((uint8_t)Register::CONFIG);

    /* Set Reset bit bit */
    //cfg |= (uint8_t)RST::SystemReset;

    data.clear();
    data.config.reset = RST::SystemReset;
    s.writeRegister((uint8_t)Register::CONFIG, data.value16);
    do {
        data.value16 =  s.readRegister_16((uint8_t)Register::CONFIG);
    } while (data.config.reset == RST::SystemReset);
}

void ina229::begin()
{
    uint16_t cfg = 0;
    pinMode(A5, OUTPUT);
    s.begin(SPISettings(1000000, MSBFIRST, SPI_MODE1), GPIO_NUM_8);

    /* Reset all registers are to default values */
    Reset();
    delay(100);

    /* Set INA230 configuration */
    // cfg = INA229_MODE_CONTINOUS_VBUS_VSHUNT | INA229_VBUSCT_1052 | INA229_VSHCT_1052 | INA229_VTCT_1052 | INA229_AVG_16;
    // cfg = (cfg & ((uint16_t)~((uint8_t)Register::ADC_CONFIG_MODE_Msk))) | (pConfig->ShuntConvertTime << (uint8_t)Register::ADC_CONFIG_MODE_Pos);
    // cfg = (cfg & ((uint16_t)~((uint8_t)Register::ADC_CONFIG_VBUSCT_Msk))) | (pConfig->BusConvertTime << (uint8_t)Register::ADC_CONFIG_VBUSCT_Pos);
    // cfg = (cfg & ((uint16_t)~((uint8_t)Register::ADC_CONFIG_VSHCT_Msk))) | (pConfig->AveragingMode << (uint8_t)Register::ADC_CONFIG_VSHCTPos);
    // cfg = (cfg & ((uint16_t)~((uint8_t)Register::ADC_CONFIG_VTCT_Msk))) | (pConfig->AveragingMode << INA230_REG_CFG_AVG_Pos);
    // cfg = (cfg & ((uint16_t)~((uint8_t)Register::ADC_CONFIG_AVG_Msk))) | (pConfig->AveragingMode << INA230_REG_CFG_AVG_Pos);

    data.clear();
    data.config.adcRange = adcRange_ = ADCRANGE::_40_96mV;
    //data.config.adcRange = adcRange_ = ADCRANGE::_163_84mV;
    data.config.convDelay = 64;//_ms;
    s.writeRegister((uint8_t)Register::CONFIG, data.value16);
    delay(100);

    data.clear();
    data.adcConfig.avg = AVG::_1024;
    data.adcConfig.vBusConvTime = ConvTime::_1052us;
    data.adcConfig.vShtConvTime = ConvTime::_1052us;
    data.adcConfig.tempConvTime = ConvTime::_1052us;
    data.adcConfig.mode = MODE::ContinuousTUI;
    s.writeRegister((uint8_t)Register::ADC_CONFIG, data.value16);
    delay(100);

}

uint16_t ina229::ReadManufacturerID()
{
    return s.readRegister_16((uint8_t)Register::MANUFACTURER_ID);
}

uint16_t ina229::ReadDeviceID()
{
    return s.readRegister_16((uint8_t)Register::DEVICE_ID);
}

void ina229::SetCalibration(uint16_t Calibration)
{
    s.writeRegister((uint8_t)Register::SHUNT_CAL, Calibration);
}

/**
 * @brief  Read programmed calibration value
 * @param  Address INA229 address on communication Bus.
 * @retval Calibration Calibration value.
 */
uint16_t ina229::GetCalibration(uint16_t Address)
{
    return s.readRegister_16((uint8_t)Register::SHUNT_CAL);
}

int32_t ina229::GetCurrent()
{
    int32_t current = ((int32_t)(s.readRegister_24((uint8_t)Register::CURRENT) << 8)) >> 8;

    return current;
}

/**
 * @brief  Read the bus voltage
 * @param  Address INA229 address on communication Bus.
 * @retval BusVoltage Bus voltage value (in mV)
 */
uint32_t ina229::GetVBus()
{
    uint32_t val;
    uint32_t vbus;
    val = s.readRegister_24((uint8_t)Register::VBUS) >> 4;
    vbus = (val * 3125 >> 4);
    vbus = DIV_ROUND_CLOSEST(vbus, 1000);
    return (uint32_t)vbus;
}

/**
 * @brief  Read the shunt voltage
 * @param  Address INA229 address on communication Bus.
 * @retval VShunt Shunt voltage value (in mV)
 */
int32_t ina229::GetVShunt(uint8_t ADCRange)
{
    int32_t val;
    int32_t vshunt;
    val = s.readRegister_24((uint8_t)Register::VSHUNT);
    if (ADCRange == 0)
        vshunt = val * 625 >> 1;
    else
        vshunt = val * 625 >> 3;
    return vshunt;
}

void ina229::setShunt(float shuntRes)
{
    if (shuntRes_ == shuntRes)
        return;
    shuntRes_ = shuntRes;
    currentLsb = shuntVoltageLSB(adcRange_) / shuntRes;
}

void ina229::setShuntCal(float shuntCal)
{
    if (shuntCal == 1.0)
        return;
    data.clear();
    data.shuntCal.value = 0x1000 * shuntCal; // 13'107'200'000ULL * shuntVoltageLSB(adcRange_);
    s.writeRegister((uint8_t)Register::SHUNT_CAL, data.value16);
    delay(100);
}

float ina229::dieTemp()
{
    data.dietemp = s.readRegister_16((uint8_t)Register::DIETEMP);
    return data.dietemp * TemperatureLSB;
}

float ina229::current()
{
    data.value32 = (unsigned int)s.readRegister_24((uint8_t)Register::CURRENT);
    return data.current.value * currentLsb;
}

float ina229::power()
{
    data.value32 = s.readRegister_24((uint8_t)Register::POWER);
    return data.power.value * 3.2 * currentLsb;
}

float ina229::vBus()
{
    data.value32 = s.readRegister_24((uint8_t)Register::VBUS);
    return data.vbus.value * BusVoltageLSB;
}

float ina229::vShunt()
{
    data.value32 = s.readRegister_24((uint8_t)Register::VSHUNT);
    return data.vshunt.value * shuntVoltageLSB(adcRange_);
}