/* Calibrates the current sensor (SCT013)*/
/* ----------------------------------------------------*/
/* Instructions:
1. Set up the hardware, the potentiometer of the voltage sensor should be already adjusted.
2. Find an "almost pure" resistive load (no motors, no reactors, no electromagnets, no LEDs). Examples: heater, boiler, electric shower, electric oven, kettle...
3. Install a voltmeter and ammeter to use as reference.
4. Connect the voltage measurement and current measurement sensors.
5. Edit the sketch *calibrate-vi.ino* and set the correct GPIO pins for the sensors. 
6. Set the calibration coefficients CV1, CV2, CV3, CI1, CI2 and CI3 to 1000 in the same file.
7. Compile and update the code from Arduino IDE.
8. Watch the values in the serial terminal and wait for them to stabilize. Use 115200bps as baud rate.
9. Take a note of the measured current (I) and voltage (V) from the ESP32 and the current and voltage from the reference voltmeter (Vr) and ammeter (Ir).
10. Calculate the calibration factors: CVnew = Vr*CVold/V, CInew = Ir*CIold/I where CVold and CIold are the previous calibrations from the sketch (initially 1000).
11. Change the values under the "Calibration" section of the code to the calculated ones (CInew and CVnew).
12. Compile and upload the code again, watch the serial monitor until the data stabilizes and then check if the measurements are correct.
13. Repeat steps 8 to 12 if necessary.
*/

#include "EmonLib.h" // Include Emon Library
#define ESP32

// Pin configuration
#define V1 34
#define V2 34
#define V3 34

#define I1 35
#define I2 32
#define I3 33

// Calibration settings (allways start with 1000)
#define CV1 1000
#define CV2 1000
#define CV3 1000

#define CI1 1000
#define CI2 1000
#define CI3 1000

EnergyMonitor emon1; // Phase 1
EnergyMonitor emon2; // Phase 2
// EnergyMonitor emon3; //Phase 3

void setup()
{
  Serial.begin(115200);

  /*
  Analog attenuation:

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

  // Phase 1
  emon1.voltage(V1, CV1, 1.732); // Voltage: input pin, calibration, phase_shift
  emon1.current(I1, CI1);        // Current: input pin, calibration.

  // Phase 2
  emon2.voltage(V2, CV2, 1.732); // Voltage: input pin, calibration, phase_shift
  emon2.current(I2, CI2);        // Current: input pin, calibration.

  // Phase 3
  // emon3.voltage(V3, CV3, 1.732);  // Voltage: input pin, calibration, phase_shift
  // emon3.current(I3, CI3);       // Current: input pin, calibration.

}

void loop()
{

  Serial.println("------------");

  // Phase 1
  emon1.calcVI(120, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
  // emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

  float realPower = emon1.realPower;         // extract Real Power into variable
  float apparentPower = emon1.apparentPower; // extract Apparent Power into variable
  float powerFactor = emon1.powerFactor;     // extract Power Factor into Variable
  float supplyVoltage = emon1.Vrms;          // extract Vrms into Variable
  float Irms = emon1.Irms;                   // extract Irms into Variable

  Serial.print("V1: ");
  Serial.print(supplyVoltage);
  Serial.print(", I1: ");
  Serial.println(Irms);

  // Phase 2
  emon2.calcVI(120, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
  // emon2.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

  float realPower2 = emon2.realPower;         // extract Real Power into variable
  float apparentPower2 = emon2.apparentPower; // extract Apparent Power into variable
  float powerFactor2 = emon2.powerFactor;     // extract Power Factor into Variable
  float supplyVoltage2 = emon2.Vrms;          // extract Vrms into Variable
  float Irms2 = emon2.Irms;                   // extract Irms into Variable

  Serial.print("V2: ");
  Serial.print(supplyVoltage2);
  Serial.print(", I2: ");
  Serial.println(Irms2);

 /* 
  // Phase 3
  emon3.calcVI(120, 2000); // Calculate all. No.of half wavelengths (crossings), time-out
  // emon2.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

  float realPower3 = emon3.realPower;         // extract Real Power into variable
  float apparentPower3 = emon3.apparentPower; // extract Apparent Power into variable
  float powerFactor3 = emon3.powerFactor;     // extract Power Factor into Variable
  float supplyVoltage3 = emon3.Vrms;          // extract Vrms into Variable
  float Irms3 = emon3.Irms;                   // extract Irms into Variable

  Serial.print("V3: ");
  Serial.print(supplyVoltage3);
  Serial.print(", I3: ");
  Serial.println(Irms3);
  */
}
