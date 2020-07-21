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
  static Debouncer button_debouncer(BUTTON_PIN, DEBOUNCE_DELAY_ms);
  static bool led_state = false;

  if (button_debouncer.IsDebounced())
  {
    // Update the LED to show the debounced signal.
    digitalWrite(LED_PIN, button_debouncer.Output());

    // Alternative feedback: Toggle the LED on the rising edge of the debounced signal.
    /**
    if (button_debouncer.Rise())
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
