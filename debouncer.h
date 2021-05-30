/*

  debouncer.h

  Debounces an input using hysteresis.
  Detects rising edges, falling edges and both edges.

*/

#pragma once
#ifndef DEBOUNCER_H
#define DEBOUNCER_H

class Debouncer
{
private:

  const int INPUT_PIN;
  volatile bool output_state : 1;
  volatile bool edge : 1;
  volatile bool rise : 1;
  volatile bool fall : 1;
  const unsigned long DEBOUNCE_DELAY_ms;
  volatile unsigned long previous_ms;

public:

  Debouncer(const int INPUT_PIN, const unsigned long DEBOUNCE_DELAY_ms = 50)
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
