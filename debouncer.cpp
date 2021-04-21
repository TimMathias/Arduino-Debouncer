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

bool Debouncer::Edge() const
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

bool Debouncer::Rise() const
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

bool Debouncer::Fall() const
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

// To be called from a polling loop where interrupts may or may not be enabled.
void Debouncer::Update()
{
  // Temporarily disable interrupts to ensure an accurate time stamp for the sample, and that status flags are updated synchronously.
#ifdef ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
#endif
    bool input_state = digitalRead(INPUT_PIN);  // No interrupts will occur between here...
    unsigned long current_ms = millis();        // ...and here, ensuring an accurate time stamp for the sample...
    Update_(input_state, current_ms);           // ...and here, ensuring flags are updated synchronously.
#ifdef ATOMIC_BLOCK
  }
#endif
}

// For use as an Interrupt Service Routine where interrupts are disabled upon calling.
void Debouncer::UpdateISR()
{
  bool input_state = digitalRead(INPUT_PIN);
  unsigned long current_ms = millis();
  Update_(input_state, current_ms);
}

void Debouncer::Update_(const bool& input_state, const unsigned long& current_ms)
{
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
    rise = input_state && !output_state;
    fall = !input_state && output_state;
    edge = rise || fall;
    output_state = input_state;
    return;
  }
  edge = rise = fall = false;
}
