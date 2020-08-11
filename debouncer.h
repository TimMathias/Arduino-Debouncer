#pragma once
#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <util/atomic.h>

class Debouncer
{
private:

  const int INPUT_PIN;
  volatile bool is_debounced;
  volatile bool output_state;
  volatile bool rise;
  volatile bool fall;
  const unsigned long DEBOUNCE_DELAY_ms;
  volatile unsigned long last_ms;

public:

  Debouncer(const int INPUT_PIN, const unsigned long DEBOUNCE_DELAY_ms = 50)
      :
      INPUT_PIN(INPUT_PIN),
      DEBOUNCE_DELAY_ms(DEBOUNCE_DELAY_ms)
  {
    this->output_state = false;
  }

  bool IsDebounced() const { return is_debounced; }
  bool Output() const { return output_state; }
  bool Rise() const { return rise; }
  bool Fall() const { return fall; }

  // For use as an Interrupt Service Routine where interrupts are disabled upon calling.
  void UpdateISR()
  {
    bool input_state = digitalRead(INPUT_PIN);
    unsigned long current_ms = millis();
    Update_(input_state, current_ms);
  }

  // To be called from a polling loop where interrupts may or may not be enabled.
  void Update()
  {
    // Temporarily disable interrupts to ensure an accurate time stamp for the sample, and that status flags are updated synchronously.
#ifdef ATOMIC_BLOCK
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif
      bool input_state = digitalRead(INPUT_PIN);  // No interrupts will occur between here...
      unsigned long current_ms = millis();        // ...and here, ensuring an accurate time stamp for the sample.
      Update_(input_state, current_ms);
#ifdef ATOMIC_BLOCK
    }
#endif
  }

private:

  inline void Update_(const int& input_state, const unsigned long& current_ms)
  {
    is_debounced = rise = fall = false;

    // Hysteresis:
    //   If there is no change, reset the debounce timer.
    //   Else, compare the time difference with the debounce delay.
    if (input_state == output_state)
    {
      last_ms = current_ms;
    }
    else if ((current_ms - last_ms) >= DEBOUNCE_DELAY_ms)
    {
      // Successfully debounced, so update the outputs.
      is_debounced = true;
      rise = input_state && !output_state;
      fall = !input_state && output_state;
      output_state = input_state;
    }
  }
};
#endif /* DEBOUNCER_H */
