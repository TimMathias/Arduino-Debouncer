/*

  debouncer.cpp

  Debounces an input using hysteresis.
  Detects rising edges, falling edges and both edges.

*/

#ifdef __AVR__
#include <util/atomic.h>
#endif

bool Debouncer::Output() const
{
#ifdef ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
#endif
    return output_state;
#ifdef ATOMIC_BLOCK
  }
#endif
}

bool Debouncer::Edge() const
{
#ifdef ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
#endif
    return edge;
#ifdef ATOMIC_BLOCK
  }
#endif
}

bool Debouncer::Rise() const
{
#ifdef ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
#endif
    return rise;
#ifdef ATOMIC_BLOCK
  }
#endif
}

bool Debouncer::Fall() const
{
#ifdef ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
#endif
    return fall;
#ifdef ATOMIC_BLOCK
  }
#endif
}

// To be called from a polling loop where interrupts may or may not be enabled.
void Debouncer::Update()
{
  // Temporarily disable interrupts to ensure an accurate time stamp for the sample, and that status flags are updated synchronously.
#ifdef ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
#endif
    const bool input_state = digitalRead(INPUT_PIN);  // No interrupts will occur between here...
    const unsigned long current_ms = millis();        // ...and here, ensuring an accurate time stamp for the sample...
    Update_(input_state, current_ms);                 // ...and here, ensuring flags are updated synchronously.
#ifdef ATOMIC_BLOCK
  }
#endif
}

// For use as an Interrupt Service Routine where interrupts are disabled upon calling.
void Debouncer::UpdateISR()
{
  const bool input_state = digitalRead(INPUT_PIN);
  const unsigned long current_ms = millis();
  Update_(input_state, current_ms);
}

void Debouncer::Update_(const bool& input_state, const unsigned long& current_ms)
{
  // Hysteresis:
  //   If there is no change, reset the debounce timer.
  //   Else, compare the time difference with the debounce delay.
  if (input_state == output_state)
  {
    previous_ms = current_ms;
  }
  else if ((current_ms - previous_ms) >= DEBOUNCE_DELAY_ms)
  {
    // Successfully debounced, so reset the debounce timer and update the outputs.
    previous_ms = current_ms;
    rise = input_state && !output_state;
    fall = !input_state && output_state;
    edge = rise || fall;
    output_state = input_state;
    return;
  }
  edge = rise = fall = false;
}
