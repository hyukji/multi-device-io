# multi-device-io
Project of system_programming lecture in DGIST

## Introduction
Our project involves running tasks (such as game, coding, etc.) on a server Raspberry Pi, which are performed by different client devices (Raspberry Pi, Windows/Ubuntu, and a light detecting sensor - CDS). Our project's first objective is to transmit input data from multiple client devices to a single server Raspberry Pi. Once the first objective is achieved, the second objective is to successfully complete tasks such as coding and Tetris by controlling the server's event processing through multi devices.


## Implementation
We used TCP/IP socket protocol to transfer I/O data between different devices and   utilized multi-processes and multi-threads to implement communication between a single server and multiple clients.

In unix-based OS such as ubuntu, I/O is applied to the system by putting a struct called "input_event" into the directory ‘dev/input/event’. So, if you want to apply I/O event of client to server, then just read input_event written in client and write them in the ‘dev/input/event’ file of the server. Therefore, the server only updated the "time_val" value, which means the input time from received data. 

In other OS clients, the input data should be converted to input_event format. So,  the server code includes the process of converting input data to the input_event format if the client is not a UNIX-based OS. It was same with sensor inputs using CDS.

As for sensors, we connected an ultrasonic sensor to the GPIO to control inputs based on distance. However, the resolution of the analog sensor was significantly low, making fast response impossible. Therefore, we utilized a light sensor (CDS) to receive inputs digitally and implemented an interesting feature by using it to input signals. Thinking that it would be more interesting to make the input of the sensor out of the user's control, we developed one mechatronics device through 3D modeling, 3D printers, and motor drivers, motors, and sensors.

By adding all the functions to the server implemented as a thread, multiple clients have completed the function of putting input into one server. To prevent delay in sensor input, sensor input was input using GPIO at the server stage. However, this function is also possible on the client.


