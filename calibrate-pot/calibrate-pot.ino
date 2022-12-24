/* Calibrates the potentiometer of the ZMPT101B module */
/* ----------------------------------------------------*/
/* Instructions:
1. Set up the hardware
2. Plug the ZMPT101B to an AC voltage source. This voltage should be close to the voltage you will eventually read during the operation of this device. Do not connect to AC sources that generate square waves (no-break, inverters, etc...).
3. Edit *calibrate-pot.ino* and change the GPIO pin according to your hardware.
4. Compile and upload the code from Arduino IDE.
5. Open the Serial Plotter view, set baud rate to 115200bps.
6. Turn the potentiometer screw clockwise or counterclockwise until you see that the bottom and top of the curves transition from flat to curved. Do not exceed too much the transition zone (between flat top and curved top) because it decreases the sensibility of the sensor.
*/

// Pin configuration
#define ZMPT101B_PIN 34

void setup()
{
  Serial.begin(115200);

// Analog attenuation: When using 0-1V sensors the attenuation should be decreased to improve accuracy.
//    ADC_0db: sets no attenuation. ADC can measure up to approximately 800 mV (1V input = ADC reading of 1088).
//    ADC_2_5db: The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1100 mV. (1V input = ADC reading of 3722).
//    ADC_6db: The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1350 mV. (1V input = ADC reading of 3033).
//    ADC_11db (default): The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 2600 mV. (1V input = ADC reading of 1575).

analogSetAttenuation(ADC_11db);

  pinMode(ZMPT101B_PIN, INPUT);
}

void loop()
{
  Serial.println(analogRead(ZMPT101B_PIN));
  delay(17);
}