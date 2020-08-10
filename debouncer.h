#pragma once
#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <util/atomic.h>

class Debouncer
{
private:
  const int input_pin;
  bool output_state;
  bool rise;
  bool fall;
  const unsigned long debounce_delay_ms;
  unsigned long last_ms;

public:

  Debouncer(const int input_pin, const unsigned long debounce_delay_ms = 50)
      :
      input_pin(input_pin),
      debounce_delay_ms(debounce_delay_ms)
  {
    this->output_state = false;
  }

  bool Output() const { return output_state; }
  bool Rise() const { return rise; }
  bool Fall() const { return fall; }

  bool IsDebounced()
  {
    bool input_state;
    unsigned long current_ms;

    // Temporarily disable interrupts to ensure an accurate time stamp for the sample.
#ifdef ATOMIC_BLOCK
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
    {
#endif
      input_state = digitalRead(input_pin);  // No interrupt will occur between here...
      current_ms = millis();                 // ...and here, ensuring an accurate time stamp for the sample.
#ifdef ATOMIC_BLOCK
    }
#endif

    rise = fall = false;

    // Hysteresis:
    //   If there is no change, reset the debounce timer.
    //   Else, compare the time difference with the debounce delay.
    if (input_state == output_state)
    {
      last_ms = current_ms;
    }
    else if ((current_ms - last_ms) >= debounce_delay_ms)
    {
      // Successfully debounced, so update the outputs.
      rise = input_state && !output_state;
      fall = !input_state && output_state;
      output_state = input_state;
      return true;
    }
    return false;
  }
};
#endif /* DEBOUNCER_H */
