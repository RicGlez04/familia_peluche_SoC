**Practica (6) PWM**

<img width="962" height="632" alt="image" src="https://github.com/user-attachments/assets/281230b4-6fdb-4847-8e80-02068bf374a5" />


**Part 1**
PWM Part 1. Modify the first example seen in class to generate PWM signals with a frequency
of 60Hz (TPMx_MOD = 43702) but different duty cycles: 0, 25%, 50%,75% and 100% (since we are
using non -inverted PWM we will have the LED completely on for 0 DT and off for 100%.
You have to calculate the values for the TPMx_CnV register for the different duty cycle values

**Video link:**

**Part 2** 
PWM Part 2. Implement the code for the second example seen in class, in which the PWM
signal is changed by increments of 1% in the CnV register. Observe how the intensity of the LED
decreases as we increment the duty cycle (because the LED is active low)

**Video link:** https://youtube.com/shorts/fLBnMiKT5WU?feature=share

**Part 3**
PWM Part 3. Simple industrial control with PWM. Imagine that we want to create an industrial grinder.
Usually, just like with a house blender, these systems have several power configurations depending
on how “hard” are the components (i.e. gravel, stones) we wish to grind and thus the motor changes
speed (and thus torque) to do the work. In this sense, we will create a simple application that can
change the speed depending on a setting defined by the user
1. When you start your application, the following message should be displayed.
Set input mode
                                  Mode 1: M Mode 2: A

Mode M stands for manual and mode 2 stands for Automatic. We will see what each mode implies.
You can use each mode by pressing whichever button in the keyboard you choose and pressing # to
execute the rest of your application.

2. In manual mode, the idea is that you integrate the first part of this lab, but modifying the code
for generating inverted PWM signals. Thus, the LCD should display
Select Speed
                                      1: L 2:M 3:MH 4:H
                     (standing for Low, Medium, Medium High and High)

   
After pressing one of the keys the corresponding CnV register values should be sent to a function
the initializes the PWM (duty cycles of 25%, 50%, 75% and 100%). Please send the value as an
argument to the function (if you use if statements the code becomes very long and it is very
inefficient). The LED is this case will go from very low intensity to completely on (as the case of a
motor).

If you have a fan or a small motor you can try as we do not require and H bridge

2. After this, if we want to modify the speed of the motor, we can use a push button to send an
interrupt to the MCU and display the above menu again. A second button can be used to stop the
system the motor at any given time (emergency signal)
Just as a note, the ADC could be used here to monitor the motor current or heat and send a signal to
the microcontroller if there is an overload (the system cannot grind something for instance) and
increase the power or switch the motor off. Something in the context is explored in the next part of
the lab.

**Automatic Mode**
1. This system is very similar to the previous case: there are 4 predefined power levels, which are
chosen depending on the value of the CnV register. However, in this case, we monitor continuously
the value of the ADC connected to a potentiometer (simulating the load of the motor). If the value
is between 0 – 0.75V the motor should run on Mode 1; between 0.76 and 1.5V on Mode 2; between
1.51 and 2.25V on mode 3 and finally, between 2.25 and 3V on Mode 4. We also should use a button
to go the main menu and a second button to stop the motor.

Video link: 
