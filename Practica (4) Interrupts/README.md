**Diseño de sistemas en Chip** - 04/22/2026

<img width="777" height="568" alt="image" src="https://github.com/user-attachments/assets/7c621691-6515-45bc-8bb9-3624184c8f19" />




**Example 1**
Part 1. Simple GPIO Interrupt. Implement the example one seen in class: the main program toggles
the red LED continuously and can be interrupted by a push button connected to port PTA1. The
interrupt routine service (ISR) for this button is simply toggle the green LED for a short time, then
moves back to tread mode to toggle de red LED again.

link del video - https://youtube.com/shorts/EL9aAxboWqA

**Example 2**
Part 2. Distinguishing interrupts from different pins. In this part, you should implement the second
example seen in class: two buttons are connected to the KL25Z board though the port A (PTA1 and
PTA2), and both can interrupt the main process running in thread mode is the processor (again, just
the red LED being toggled continuously).
As there is just one interrupt for the port A, we need to implement a mechanism for differentiating
between the two ports; this can be done through flags in the ISFR register. Please remember to
properly disable interrupts before the initialization code in your program. Also, it iis important to
clear the interrupt after having served it, otherwise we might never go back to the main program!!!

link del video - https://youtube.com/shorts/g_VeJAytbeo

**Example 3**
Part 3. Event counter. The goal of this last part of the lab is to integrate concepts of interrupts and timers, and put together a simple application involving the LCD and the 4x4 matrix keyboard. The main idea is to reuse the code of part 1 of this lab, but instead of toggling LEDs, we will integrate into the second part of the previous lab (ascending timer): when the counter is counting, if interrupt from PTA1 is activated, the code should execute and IS that halts the counter and shows a message such as "PAUSED", the main program can resume if you press the * key.

link del video -

