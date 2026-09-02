# /firmware

ESP32 control code goes here once it exists. Planned scope:

- STS3215 bus servo driver over UART (via the Waveshare Bus Servo Adapter)
- Joint position read/write + torque-disable for kinesthetic teach mode
- Teach-and-repeat: record demonstrated motions, play them back
- Eventually: a tool-head interface layer so swapping tool heads (dial gauge, vacuum, 4th-DOF wrist) doesn't require rewriting the core motion code

Nothing's written yet.. this is a placeholder so the repo structure is ready when it is.
