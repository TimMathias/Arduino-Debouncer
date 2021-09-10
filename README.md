# Debouncer for Arduino
Simple debouncer for Arduino that detects positive and negative edges using hysteresis or integration.

## Usage
### Polling
#### Level Detector
```c++
#include <debouncer.h>

const byte BUTTON_PIN = 2;  // Pushbutton pin number.
const byte LED_PIN = 13;    // LED pin number.
const unsigned long DEBOUNCE_DELAY_ms = 50;  // The debounce time in milliseconds.

void setup()
{
  // Set pin modes.
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  static Debouncer button1(BUTTON_PIN, DEBOUNCE_DELAY_ms);
  static bool previous_state = false;

  button1.Update();
  bool current_state = button1.Output();

  if (current_state != previous_state)
  {
    // Update the LED to show the debounced signal.
    digitalWrite(LED_PIN, current_state);
    previous_state = current_state;
  }
}
```
#### Rising Edge Detector
```c++
void loop()
{
  static Debouncer button1(BUTTON_PIN, DEBOUNCE_DELAY_ms);
  static bool led_state = false;
  
  button1.Update();

  // Toggle the LED on the rising edge of the debounced signal.
  if (button1.Rise())
  {
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state);
  }
}
```
### Timer with Interrupt Service Routine (ISR)
#### Falling Edge Detector
```c++
#include <debouncer.h>

const byte BUTTON_PIN = 2;  // Pushbutton pin number.
const byte LED_PIN = 13;    // LED pin number.
const unsigned long DEBOUNCE_DELAY_ms = 50;  // The debounce time in milliseconds.

void setup()
{
  // Set pin modes.
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  cli(); // Disable interrupts.

  //
  // Set Timer0 interrupt
  //
  // Clock frequency     = 16 MHz
  // Interrupt frequency = 1 kHz
  // Prescaler           = 64
  //
  // Timeout = 16 MHz / (1 kHz * 64) - 1 = 249
  //
  TCCR0A = 0;                           // Clear TCCR0A register.
  TCCR0B = 0;                           // Clear TCCR0B register.
  TCNT0  = 0;                           // Clear counter value.
  OCR0A  = 249;                         // Set timeout value.
  TCCR0A |= (1 << WGM01);               // CTC mode.
  TCCR0B |= (1 << CS01) | (1 << CS00);  // Set prescaler.
  TIMSK0 |= (1 << OCIE0A);              // Enable compare interrupt.
  
  sei(); // Enable interrupts.
}

ISR(TIMER0_COMPA_vect)
{
  static Debouncer button1(BUTTON_PIN, DEBOUNCE_DELAY_ms);
  static bool led_state = false;
  
  button1.UpdateISR();
  
  // Toggle the LED on the falling edge of the debounced signal.
  if (button1.Fall())
  {
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state);
  }
}
```
#### Either Edge Detector
```c++
ISR(TIMER0_COMPA_vect)
{
  static Debouncer button1(BUTTON_PIN, DEBOUNCE_DELAY_ms);
  static bool led_state = false;
  
  button1.UpdateISR();
  
  // Toggle the LED on either edge of the debounced signal.
  if (button1.Edge())
  {
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state);
  }
}
```
## Licence
MIT
