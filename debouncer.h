#pragma once
#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#ifdef __AVR__
#include <util/atomic.h>
#endif

class Debouncer
{
private:

  const int INPUT_PIN;
  volatile bool is_debounced;
  volatile bool output_state;
  volatile bool edge;
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
    this->is_debounced = false;
    this->output_state = false;
    this->edge = false;
    this->rise = false;
    this->fall = false;
  }

  bool IsDebounced() const
  {
    bool copy;
#ifdef ATOMIC_BLOCK
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif
      copy = is_debounced;
#ifdef ATOMIC_BLOCK
    }
#endif
    return copy;
  }

  bool Output() const
  {
    bool copy;
#ifdef ATOMIC_BLOCK
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif
      copy = output_state;
#ifdef ATOMIC_BLOCK
    }
#endif
    return copy;
  }

  bool Edge() const
  {
    bool copy;
#ifdef ATOMIC_BLOCK
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif
      copy = edge;
#ifdef ATOMIC_BLOCK
    }
#endif
    return copy;
  }

  bool Rise() const
  {
    bool copy;
#ifdef ATOMIC_BLOCK
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif
      copy = rise;
#ifdef ATOMIC_BLOCK
    }
#endif
    return copy;
  }

  bool Fall() const
  {
    bool copy;
#ifdef ATOMIC_BLOCK
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif
      copy = fall;
#ifdef ATOMIC_BLOCK
    }
#endif
    return copy;
  }

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
    is_debounced = edge = rise = fall = false;

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
      edge = rise || fall;
      output_state = input_state;
    }
  }
};
#endif /* DEBOUNCER_H */
