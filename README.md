# Debouncer for Arduino
Simple debouncer for Arduino.

## Usage

```c++
#include <debouncer.h>

const int BUTTON_PIN = 2;  // Pushbutton pin number.
const int LED_PIN = 13;    // LED pin number.
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
  static bool led_state = false;
  
  button1.Update();

  if (button1.IsDebounced())
  {
    // Update the LED to show the debounced signal.
    digitalWrite(LED_PIN, button1.Output());

    // Alternative feedback: Toggle the LED on the rising edge of the debounced signal.
    /**
    if (button1.Rise())
    {
      led_state = !led_state;
      digitalWrite(LED_PIN, led_state);
    }
    /**/
  }
}
```
## Licence
MIT
