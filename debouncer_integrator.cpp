/*

  debouncer_integrator.cpp

  Debounces an input using integration.
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

#include "debouncer_integrator.h"

bool DebouncerIntegrator::Output() const
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

bool DebouncerIntegrator::Edge() const
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

bool DebouncerIntegrator::Rise() const
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

bool DebouncerIntegrator::Fall() const
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

#if DEBOUNCER_REPEAT_COUNT
unsigned long DebouncerIntegrator::RepeatCount() const
{
#ifdef ATOMIC_BLOCK
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
#endif
		return repeat_count;
#ifdef ATOMIC_BLOCK
	}
#endif
}
#endif

// To be called from a polling loop where interrupts may or may not be enabled.
void DebouncerIntegrator::Update()
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
void DebouncerIntegrator::UpdateISR()
{
	const bool input_state = digitalRead(INPUT_PIN);
	const unsigned long current_ms = millis();
	Update_(input_state, current_ms);
}

void DebouncerIntegrator::Update_(const bool& input_state, const unsigned long& current_ms)
{
	const unsigned long delta_ms = current_ms - previous_ms;
	previous_ms = current_ms;

	// Integrator:
	//    If there is no change, the sum tends towards zero.
	//    Else, the sum tends towards the debounce delay.
	if (input_state == output_state)
	{
#if DEBOUNCER_REPEAT_COUNT
		if ((current_ms - previous_repeat_ms) >= REPEAT_DELAY_ms)
		{
			repeat_count++;
			previous_repeat_ms += REPEAT_DELAY_ms;
		}
#endif
		if (sum_ms > delta_ms)  // Avoid going negative (underflow).
		{
			sum_ms -= delta_ms;  // Tend towards zero if input equals output.
		}
		else
		{
			sum_ms = 0;
		}
	}
	else
	{
		if (delta_ms < DEBOUNCE_DELAY_ms - sum_ms)  // Avoid overflow.
		{
			sum_ms += delta_ms;  // Tend towards DEBOUNCE_DELAY_ms if input and output differ.
		}
		else
		{
			// Successfully debounced, so reset the sum and update the outputs.
#if DEBOUNCER_REPEAT_COUNT
			repeat_count = 0;
			previous_repeat_ms = current_ms;
#endif
			sum_ms = 0;
			rise = input_state && !output_state;
			fall = !input_state && output_state;
			edge = rise || fall;
			output_state = input_state;
			return;
		}
	}
	edge = rise = fall = false;
}
