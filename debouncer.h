/*

  debouncer.h

  Debounces an input using hysteresis.
  Detects rising edges, falling edges and both edges.

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

public:

  Debouncer(const byte INPUT_PIN, const unsigned long DEBOUNCE_DELAY_ms = 50)
    : INPUT_PIN(INPUT_PIN)
    , DEBOUNCE_DELAY_ms(DEBOUNCE_DELAY_ms)
    , output_state(digitalRead(INPUT_PIN))
    , edge(false)
    , rise(false)
    , fall(false)
    , previous_ms(millis())
  {
  }

  bool Output() const;
  bool Edge() const;
  bool Rise() const;
  bool Fall() const;

  // To be called from a polling loop where interrupts may or may not be enabled.
  void Update();

  // For use as an Interrupt Service Routine where interrupts are disabled upon calling.
  void UpdateISR();

private:

  void Update_(const bool& input_state, const unsigned long& current_ms);
};

#endif /* DEBOUNCER_H */
