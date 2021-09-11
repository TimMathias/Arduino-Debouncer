/*

  debouncer.h

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

#pragma once
#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <Arduino.h>

#define DEBOUNCER_REPEAT_COUNT false

class Debouncer
{
private:

  const byte INPUT_PIN;
  volatile bool output_state : 1;
  volatile bool edge : 1;
  volatile bool rise : 1;
  volatile bool fall : 1;
  const unsigned long DEBOUNCE_DELAY_ms;
  volatile unsigned long previous_ms;

#if DEBOUNCER_REPEAT_COUNT
  const unsigned long REPEAT_DELAY_ms;
  volatile unsigned long previous_repeat_ms;
  volatile unsigned long repeat_count;
#endif

public:

  Debouncer
  (
    const byte INPUT_PIN
    , const unsigned long DEBOUNCE_DELAY_ms = 50
#if DEBOUNCER_REPEAT_COUNT
    , const unsigned long REPEAT_DELAY_ms = 100
#endif
  )
    : INPUT_PIN(INPUT_PIN)
    , DEBOUNCE_DELAY_ms(DEBOUNCE_DELAY_ms)
#if DEBOUNCER_REPEAT_COUNT
    , REPEAT_DELAY_ms(REPEAT_DELAY_ms)
#endif
    , output_state(digitalRead(INPUT_PIN))
    , edge(false)
    , rise(false)
    , fall(false)
    {
#if DEBOUNCER_REPEAT_COUNT
    previous_repeat_ms =
#endif
    previous_ms = millis();
  }

  // Non-copyable.
  Debouncer(const Debouncer&) = delete;

  // Non-copyable.
  Debouncer& operator=(const Debouncer&) = delete;

  bool Output() const;
  bool Edge() const;
  bool Rise() const;
  bool Fall() const;

#if DEBOUNCER_REPEAT_COUNT
  unsigned long RepeatCount() const;
#endif

  // To be called from a polling loop where interrupts may or may not be enabled.
  void Update();

  // For use as an Interrupt Service Routine where interrupts are disabled upon calling.
  void UpdateISR();

private:

  void Update_(const bool input_state, const unsigned long& current_ms);
};

#endif /* DEBOUNCER_H */
