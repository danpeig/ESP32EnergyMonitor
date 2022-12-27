#include "esphome.h"
#include "EmonLib.h"
#include "esp_task_wdt.h" //Watchdog control features
#define ESP32

// Watchdog timeout in seconds
#define WDT_TIMEOUT 5 

// The Sample window length is defined by the number of half wavelengths or crossings we choose to measure.
// 120 is quite conservative for two phases
#define CROSSINGS 120

// Polling interval
#define POLLING_INTERVAL 30000 //Every 30s will perform a read and publish the data. Do not go under 5s to prevent crashes.

// Pin configuration
#define V1 34
#define V2 34 //Set to V1 if using only one voltage meter
#define V3 34 //Set to V1 if using only one voltage meter

#define I1 35
#define I2 32
#define I3 33

// Calibration for voltage sensors CV and current sensors CI
#define CV1 510
#define CV2 510 //Set to CV1 if using only one voltage meter
#define CV3 510 //Set to CV1 if using only one voltage meter

#define CI1 96.72
#define CI2 94.93
#define CI3 1

class MyPowerSensor : public PollingComponent, public Sensor
{
public:
  MyPowerSensor() : PollingComponent(POLLING_INTERVAL) {}

  float get_setup_priority() const override { return esphome::setup_priority::LATE; }

  EnergyMonitor emon1; // Phase 1
  EnergyMonitor emon2; // Phase 2
  // EnergyMonitor emon3; //Phase 3

  // Phase 1 sensors
  Sensor *realpower_sensor1 = new Sensor();
  Sensor *apparentpower_sensor1 = new Sensor();
  Sensor *powerfactor_sensor1 = new Sensor();
  Sensor *supplyvoltage_sensor1 = new Sensor();
  Sensor *current_sensor1 = new Sensor();

  // Phase 2 sensors
  Sensor *realpower_sensor2 = new Sensor();
  Sensor *apparentpower_sensor2 = new Sensor();
  Sensor *powerfactor_sensor2 = new Sensor();
  Sensor *supplyvoltage_sensor2 = new Sensor();
  Sensor *current_sensor2 = new Sensor();

  // Phase 3 sensors
  /*
   Sensor *realpower_sensor3 = new Sensor();
   Sensor *apparentpower_sensor3 = new Sensor();
   Sensor *powerfactor_sensor3 = new Sensor();
   Sensor *supplyvoltage_sensor3 = new Sensor();
   Sensor *current_sensor3 = new Sensor();
   */

  // Total sensors
  Sensor *realpower_sensor_total = new Sensor();
  Sensor *apparentpower_sensor_total = new Sensor();
  Sensor *current_sensor_total = new Sensor();

  void setup() override
  {

    esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
    esp_task_wdt_add(NULL);               // add current thread to WDT watch

    /*
    Analog attenuation: When using 0-1V sensors the attenuation should be decreased to improve accuracy.

    ADC_0db: sets no attenuation. ADC can measure up to approximately 800 mV (1V input = ADC reading of 1088).
    ADC_2_5db: The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1100 mV. (1V input = ADC reading of 3722).
    ADC_6db: The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1350 mV. (1V input = ADC reading of 3033).
    ADC_11db (default): The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 2600 mV. (1V input = ADC reading of 1575).

    */
    analogSetPinAttenuation(V1, ADC_11db);
    analogSetPinAttenuation(V2, ADC_11db);
    analogSetPinAttenuation(V3, ADC_11db);

    analogSetPinAttenuation(I1, ADC_11db);
    analogSetPinAttenuation(I2, ADC_11db);
    analogSetPinAttenuation(I3, ADC_11db);

    // Phase 1 sensors
    emon1.voltage(V1, CV1, 1.732); // Voltage: input pin, calibration, phase_shift
    emon1.current(I1, CI1);        // Current: input pin, calibration.

    // Phase 2 sensors
    emon2.voltage(V2, CV2, 1.732); // Voltage: input pin, calibration, phase_shift
    emon2.current(I2, CI2);        // Current: input pin, calibration.

    /*
    //Phase 3 sensors
    emon3.voltage(V3, CV3, 1.732);  // Voltage: input pin, calibration, phase_shift
    emon3.current(I3, CI3);       // Current: input pin, calibration.
    */
  }

  void update() override
  {

    // Phase 1
    emon1.calcVI(CROSSINGS, 2000);
    float realPower1 = emon1.realPower;
    realpower_sensor1->publish_state(realPower1);
    float apparentPower1 = emon1.apparentPower;
    apparentpower_sensor1->publish_state(apparentPower1);
    float powerFactor1 = emon1.powerFactor;
    powerfactor_sensor1->publish_state(powerFactor1);
    float supplyVoltage1 = emon1.Vrms;
    supplyvoltage_sensor1->publish_state(supplyVoltage1);
    float current1 = emon1.Irms;
    current_sensor1->publish_state(current1);

    esp_task_wdt_reset(); // Things can take some time... this ensures the watchdog is aware

    // Phase 2
    emon2.calcVI(CROSSINGS, 2000);
    float realPower2 = emon2.realPower;
    realpower_sensor2->publish_state(realPower2);
    float apparentPower2 = emon2.apparentPower;
    apparentpower_sensor2->publish_state(apparentPower2);
    float powerFactor2 = emon2.powerFactor;
    powerfactor_sensor2->publish_state(powerFactor2);
    float supplyVoltage2 = emon2.Vrms;
    supplyvoltage_sensor2->publish_state(supplyVoltage2);
    float current2 = emon2.Irms;
    current_sensor2->publish_state(current2);

    esp_task_wdt_reset(); // Things can take some time... this ensures the watchdog is aware

    /*
     // Phase 3
     emon3.calcVI(CROSSINGS,2000);
     float realPower3 = emon3.realPower;
     realpower_sensor3->publish_state(realPower3);
     float apparentPower3 = emon3.apparentPower;
     apparentpower_sensor3->publish_state(apparentPower3);
     float powerFactor3 = emon3.powerFactor;
     powerfactor_sensor3->publish_state(powerFactor3);
     float supplyVoltage3 = emon3.Vrms;
     supplyvoltage_sensor3->publish_state(supplyVoltage3);
     float current3 = emon3.Irms;
     current_sensor3->publish_state(current3);
     */

    /*
    // Totals 1 phase - uncomment only this block if you are reading one phase
    float realPower_total = emon1.realPower;
    realpower_sensor_total->publish_state(realPower_total);
    float apparentPower_total = emon1.apparentPower;
    apparentpower_sensor_total->publish_state(apparentPower_total);
    float current_total = emon1.Irms;
    current_sensor_total->publish_state(current_total);
    */

    // Totals 2 phases - uncomment only this block if you are reading two phases
    float realPower_total = emon1.realPower + emon2.realPower;
    realpower_sensor_total->publish_state(realPower_total);
    float apparentPower_total = emon1.apparentPower + emon2.apparentPower;
    apparentpower_sensor_total->publish_state(apparentPower_total);
    float current_total = emon1.Irms + emon2.Irms;
    current_sensor_total->publish_state(current_total);

    /*
     // Totals 3 phases - uncomment only this block if you are reading three phases
     float realPower_total = emon1.realPower + emon2.realPower + emon3.realPower;
     realpower_sensor_total->publish_state(realPower_total);
     float apparentPower_total = emon1.apparentPower + emon2.apparentPower + emon3.apparentPower;
     apparentpower_sensor_total->publish_state(apparentPower_total);
     float current_total = emon1.Irms + emon2.Irms + emon3.Irms;
     current_sensor_total->publish_state(current_total);
     */
  }
};
