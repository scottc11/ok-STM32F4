## MIDI 1.0 Specification Message Summary

### Channel Voice Messages (`nnnn = 0-15`, MIDI Channel `1-16`)

| Status (`D7-D0`) | Data Byte(s) (`D7-D0`) | Description |
| --- | --- | --- |
| `1000nnnn` | `0kkkkkkk`<br>`0vvvvvvv` | **Note Off**. Sent when a note is released. `kkkkkkk` = key number, `vvvvvvv` = velocity. |
| `1001nnnn` | `0kkkkkkk`<br>`0vvvvvvv` | **Note On**. Sent when a note is depressed. `kkkkkkk` = key number, `vvvvvvv` = velocity. |
| `1010nnnn` | `0kkkkkkk`<br>`0vvvvvvv` | **Polyphonic Key Pressure (Aftertouch)**. `kkkkkkk` = key number, `vvvvvvv` = pressure value. |
| `1011nnnn` | `0ccccccc`<br>`0vvvvvvv` | **Control Change**. `ccccccc` = controller number (`0-119`), `vvvvvvv` = controller value (`0-127`). Controllers `120-127` are Channel Mode Messages. |
| `1100nnnn` | `0ppppppp` | **Program Change**. `ppppppp` = new program number. |
| `1101nnnn` | `0vvvvvvv` | **Channel Pressure (Aftertouch)**. Sends a single pressure value for the channel. `vvvvvvv` = pressure value. |
| `1110nnnn` | `0lllllll`<br>`0mmmmmmm` | **Pitch Bend Change**. 14-bit value where `lllllll` = LSB and `mmmmmmm` = MSB. Center is `2000H`. |

### Channel Mode Messages

Base message format (same status as Control Change):  
`1011nnnn`, data bytes: `0ccccccc` and `0vvvvvvv`

| Controller (`c`) | Value (`v`) | Mode Command | Description |
| --- | --- | --- | --- |
| `120` | `0` | All Sound Off | Turns off all oscillators and sets volume envelopes to zero as soon as possible. |
| `121` | `x` | Reset All Controllers | Resets controller values to defaults (`v` should be `0` unless otherwise specified). |
| `122` | `0` | Local Control Off | Device responds only to incoming MIDI data. |
| `122` | `127` | Local Control On | Restores normal local controller behavior. |
| `123` | `0` | All Notes Off | Turns off all oscillators. |
| `124` | `0` | Omni Mode Off | Disables Omni mode. |
| `125` | `0` | Omni Mode On | Enables Omni mode. |
| `126` | `M` | Mono Mode On (Poly Off) | `M` = number of channels (Omni Off) or `0` (Omni On). |
| `127` | `0` | Poly Mode On (Mono Off) | Also causes All Notes Off (with `124-127`). |

### System Common Messages

| Status (`D7-D0`) | Data Byte(s) (`D7-D0`) | Description |
| --- | --- | --- |
| `11110000` ... `11110111` | `0iiiiiii` `[0iiiiiii 0iiiiiii]` `0ddddddd ... 0ddddddd` | **System Exclusive (SysEx)**. Starts with `F0`, ends with `F7`. Manufacturer ID is 1 or 3 bytes. Payload is manufacturer/universal data. |
| `11110001` | `0nnndddd` | **MIDI Time Code Quarter Frame**. `nnn` = message type, `dddd` = value. |
| `11110010` | `0lllllll`<br>`0mmmmmmm` | **Song Position Pointer**. 14-bit pointer in MIDI beats (`1 beat = 6 MIDI clocks`), `l` = LSB, `m` = MSB. |
| `11110011` | `0sssssss` | **Song Select**. Selects which sequence/song to play. |
| `11110100` | - | **Undefined (Reserved)**. |
| `11110101` | - | **Undefined (Reserved)**. |
| `11110110` | - | **Tune Request**. Analog synths should tune oscillators. |
| `11110111` | - | **End of Exclusive (EOX)**. Terminates a SysEx message. |

### System Real-Time Messages

| Status (`D7-D0`) | Message | Description |
| --- | --- | --- |
| `11111000` | Timing Clock | Sent 24 times per quarter note for synchronization. |
| `11111001` | Undefined | Reserved. |
| `11111010` | Start | Starts current sequence (followed by Timing Clocks). |
| `11111011` | Continue | Continues sequence from stop point. |
| `11111100` | Stop | Stops current sequence. |
| `11111101` | Undefined | Reserved. |
| `11111110` | Active Sensing | Optional keepalive; if not received within ~300 ms after initial receipt, receiver assumes disconnect and turns voices off. |
| `11111111` | Reset | Resets receivers to power-up status; should be used sparingly. |