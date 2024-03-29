/*

  debouncer.cpp

  Debounces an input using hysteresis.
  Detects rising edges, falling edges and both edges.
  Enable the repeat count by defining DEBOUNCER_REPEAT_COUNT as true.

  MIT License

  Copyright (c) 2021 Timothy Mathias

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  
*/

#ifdef __AVR__
#include <util/atomic.h>
#endif

#ifndef ATOMIC_BLOCK
  #define ATOMIC_BLOCK(x)
#endif

#include "debouncer.h"

bool Debouncer::Output() const
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    return output_state;
  }
}

bool Debouncer::Edge() const
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    return edge;
  }
}

bool Debouncer::Rise() const
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    return rise;
  }
}

bool Debouncer::Fall() const
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    return fall;
  }
}

#if DEBOUNCER_REPEAT_COUNT
unsigned long Debouncer::RepeatCount() const
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    return repeat_count;
  }
}
#endif

// To be called from a polling loop where interrupts may or may not be enabled.
void Debouncer::Update()
{
  // Temporarily disable interrupts to ensure an accurate time stamp for the sample, and that status flags are updated synchronously.
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    const bool input_state = digitalRead(INPUT_PIN);  // No interrupts will occur between here...
    const unsigned long current_ms = millis();        // ...and here, ensuring an accurate time stamp for the sample...
    Update_(input_state, current_ms);                 // ...and here, ensuring flags are updated synchronously.
  }
}

// For use as an Interrupt Service Routine where interrupts are disabled upon calling.
void Debouncer::UpdateISR()
{
  const bool input_state = digitalRead(INPUT_PIN);
  const unsigned long current_ms = millis();
  Update_(input_state, current_ms);
}

void Debouncer::Update_(const bool input_state, const unsigned long& current_ms)
{
  // Hysteresis:
  //   If there is no change, reset the debounce timer.
  //   Else, compare the time difference with the debounce delay.
  if (input_state == output_state)
  {
#if DEBOUNCER_REPEAT_COUNT
    if ((current_ms - previous_repeat_ms) >= REPEAT_DELAY_ms)
    {
      repeat_count++;
      previous_repeat_ms += REPEAT_DELAY_ms;
    }
#endif
    previous_ms = current_ms;
  }
  else if ((current_ms - previous_ms) >= DEBOUNCE_DELAY_ms)
  {
    // Successfully debounced, so reset the debounce timer and update the outputs.
#if DEBOUNCER_REPEAT_COUNT
    repeat_count = 0;
    previous_repeat_ms =
#endif
    previous_ms = current_ms;
    rise = input_state && !output_state;
    fall = !input_state && output_state;
    edge = rise || fall;
    output_state = input_state;
    return;
  }
  edge = rise = fall = false;
}
