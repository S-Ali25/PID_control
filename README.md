# PID Controller: Simulink Design → Arduino Hardware Implementation

## Overview
This project designs a PID controller in Simulink, validates the tuning through 
simulation, then implements and runs the same controller on real Arduino hardware 
controlling an LED to compare the real hardware response against the simulated 
prediction.

The workflow mirrors how PID controllers are typically developed in practice: 
model and tune in simulation first, then deploy and validate on real hardware.

## Repository structure
- `/simulink` — Simulink model (`sim_PID_control.slx`) used to design and tune the controller
- `/matlab` — comparison script (`PID_control.m`) and logged hardware data (`arduino_log.csv`)
- `/arduino` — Arduino sketch (`LED_PID_control.ino`) implementing the same PID loop in C

## System design
<img width="758" height="386" alt="image" src="https://github.com/user-attachments/assets/dd0a7687-ca20-4c09-a34d-4592d18f9b95" />

The plant is modeled as a simple first-order lag, `1/(s+1)`, representing how the 
LED's brightness responds to a drive signal with some delay.

**Important note on the hardware implementation:** the Arduino has no physical sensor 
measuring the LED's actual brightness. The "plant" is simulated in software on the 
Arduino itself (`current_value += k * (drive_signal - current_value)`), and the LED's 
PWM brightness is a visual readout of that simulated value. The control loop is real 
and correctly implemented — the feedback signal is modeled rather than physically measured.

## Tuning
Tuned manually in Simulink using the Scope and step response, since automatic PID 
Tuner required a Simulink Control Design license unavailable on this setup.

| Gain | Value | Effect observed |
|------|-------|------------------|
| Kp   | 15    | Fast response, but ~6% steady-state error alone |
| Ki   | 10    | Eliminates steady-state error, no visible overshoot |
| Ki   | 100 (test) | Introduces overshoot — used to demonstrate the tradeoff |
| Kd   | 0     | Not needed for this plant/tuning; see Future Work |

**Final gains used: Kp = 15, Ki = 10, Kd = 0**


## Hardware validation
The tuned gains (Kp=15, Ki=10) were ported directly into the Arduino code and tested 
with a step change in setpoint (button press, jump from 0 to 20), then logged over 
serial and compared against the Simulink prediction.


Both converge to the same steady-state value with no overshoot, confirming the 
discrete C implementation is behaving equivalently to the continuous Simulink design. 
Minor shape differences between the two are expected: the real loop's `dt` (calculated 
from `millis()`) jitters slightly between iterations depending on what else the loop 
is doing, whereas Simulink's solver uses precise, fixed time steps — real hardware 
timing is never as clean as an idealized simulation.

## Controls
- Two push buttons increase/decrease the setpoint, with software debouncing to 
  register one step per press rather than repeating while held.
- LED brightness (PWM) reflects the simulated plant's current value.

## What I'd improve next
- Add a real sensor (e.g. a photoresistor) to close the loop with genuine physical 
  feedback instead of a simulated plant
- Add the derivative term and demonstrate it damping the Ki=100 overshoot case
- Extend to closed-loop stepper motor position control using a limit switch as a 
  crude position reference

## How to run
- **Simulink:** open `sim_PID_control.slx` in MATLAB, run the simulation, gains are 
  set in the PID Controller block
- **Arduino:** upload `LED_PID_control.ino` via Arduino IDE to a board with a 
  PWM-capable pin (used pin 11), buttons wired to A1/A2 with `INPUT_PULLUP`
- **Comparison:** run `PID_control.m` in MATLAB after logging fresh serial data 
  from the Arduino into `arduino_log.csv`
