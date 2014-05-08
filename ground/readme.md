Ground control
==============
Firmware defines C structs that define uplink and downlink packet structure.
Ground control software, written in C, refers to these same structs.

The packet-level transport protocol is as follows:

```
  [ 4-byte header ][ 1-byte message type ID ][ C struct message ][ 32-bit CRC ]
```

  * 4-byte header
    - Should never occur in any other part of packet, needs some thought.
  * 1-byte message type ID
    - 0: Request type
    - 1-255: Specifies type of current message
  * C struct message
    - Binary packed message
  * 32-bit CRC
    - Packet integrity check

At 38400 baud and 1+8+1=10 bits per UART byte, we theoretically transmit up to
96 bytes at 40 Hz. The packet structure above suffers 9 bytes of overhead.

C struct messages are defined in `firmware/msg_types`.

Uplink
------
Command uplink at 40 Hz:

```c
struct up_command_t {
  uint8_t mode;   /* Control mode (rate or position) */
  uint8_t axes[3];   /* Desired axis values (rad/s in XYZ or rad in XY) */
  uint8_t throttle;
};   /* 5 bytes */
```

Configuration uplink, request-reply:

```c
struct up_config_t {
  float trim[2];   /* XY trim, Euler angles */
};   /* 8 bytes */
```

Downlink
--------
(The telemetry scheme needs more thought.)

High-frequency telemetry:

```c
struct down_telem_highfreq_t {
  float dcm[9];   /* DCM, we really should be using quaternions */
};   /* 36 bytes */
```

Low-frequency telemetry:

```c
struct down_telem_lowfreq_t {
  float dc[4];   /* Motor/servo duty cycles */
};   /* 16 bytes */
```

Synchronization struct, only sent on request, ensure ground software is synced
to flight config.

```c
struct down_sync_t {
  float gains[6];   /* Velocity and position PID gains */
  float trim[2];   /* XY trim */
};   /* 32 bytes */
```

