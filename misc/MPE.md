### 2.1 MPE Functional Overview

This overview summarizes the main elements of the MPE specification, additional important details can be found in later sections.

MPE is switched on and configured using the following messages:
- MPE Configuration Message
- MIDI Mode
- Pitch Bend Sensitivity

MPE offers per-note expressive control using the following messages:
- Note On/Off
- Pitch Bend
- Channel Pressure
- Control Change #74

MPE establishes the following rules to coordinate per-note control:
- The MPE Configuration Message is a Registered Parameter Number which is used to set the range of
Channels over which notes are sent and received. The MIDI Channel space can be divided into two
sub-spaces called Zones, so that multi-timbral playing is still possible using only one MIDI
Transport.
- Each Zone has a number of Member Channels for notes plus a dedicated extra Channel, called the
Manager Channel, which conveys information common to all Active Notes in that Zone.
- Wherever possible, every note is assigned its own Channel for the lifetime of that note. This allows
MPE messages to be addressed uniquely to that Active Note.
MPE also defines these additional behaviors for senders and receivers:
- MPE Devices shall, by default, set Pitch Bend Sensitivity to a value of 48 semitones on all Member
Channels and 2 semitones on the Manager Channel. Either Pitch Bend Sensitivity values may be
changed to a number of semitones between 0 and 96 using RPN 0.
- Pressure is sent using the Channel Pressure message. To preserve compatibility with existing MIDI
Devices, Polyphonic Key Pressure may be used with Active Notes on the Manager Channel but shall
not be used on other Member Channels.
- In addition to being able to express per-note pitch (Pitch Bend) and pressure (Channel Pressure), a
third dimension of per-note-control may be expressed using MIDI Control Change #74.

An implementation summary is tabulated in Appendix F

## 2.2 Turning On and Configuring MPE

The following subsections specify how to turn MPE on, and how to configure MPE.

### 2.2.1 MPE Configuration Message (MCM)

The MPE mode of operation (“MPE Mode”) shall be enabled in a controller or a synthesizer when at least one
MPE Zone is configured.

All MPE-compatible Devices shall support the MPE Configuration Message, in addition to any optional means
(such as a power-up default, or via on-board selection) for configuring MPE Mode. The MPE Configuration
Message can be received by any MIDI device.

Setting both Zones on the Manager Channels to use no Channels, shall deactivate the MPE Mode. The behavior of
a Device when MPE operation is deactivated is left to the manufacturer.

The MPE Configuration Message is Registered Parameter Number "00 06" as defined in the MPE Configuration
RPN [MA03] specification. The MSB of Data Entry represents the number of MIDI Channels assigned, as
explained below. The LSB of Data Entry has no function.

[REGISTERED PARAMETER NUMBER]

| CC#101 (MSB) | CC#100 (LSB) | Function              |
|--------------|--------------|------------------------|
| 00           | 06           | MPE Configuration RPN |

Message Format: [0xBn 0x65 0x00] [0xBn 0x64 0x06] [0xBn 0x06 <mm>]

Where n = MIDI Channel Number:

n=0x0: Lower Zone Manager Channel

n=0xF: Upper Zone Manager Channel

All other values are invalid and should be ignored.

And mm = Number of Member MIDI Channels in the Zone:

mm=0x0: MPE is Off (No Channels)

mm=0x1 to 0xF: Assigns that number of MIDI Channels to the Zone (see below)


Each MCM shall define one MPE Zone, which is determined by the MIDI Channel Number (“n”) nibble of the
Message. When MPE is configured with the MCM, the MPE Device has either one Zone or two Zones: a ‘Lower
Zone’, or an ‘Upper Zone’, or both.

This terminology suggests the lower and upper sections of a split keyboard, but the Zones may map to a single
physical controller in many conceivable ways, the details of which are left to the manufacturer. The MCM shall
assign the number of MIDI Channels to be used for MPE Mode operation in that Zone. Any MIDI Channels not
assigned to any Zone remain available for conventional use. If an MPE Sender intends to use only one Zone, then it should send one MCM, not two MCMs.

The Lower Zone is controlled by Manager Channel 1, with Member Channels assigned sequentially from Channel
2 upwards. The Upper Zone is controlled by Manager Channel 16, with Member Channels assigned sequentially
from Channel 15 downwards. Each Zone is activated with its own MCM message, which can be sent in either
order. Sending an MCM with the number of Member Channels set to zero deactivates that Zone.

The Manager Channel of an unused Zone may be used as a Member Channel for the other Zone. Thus, if only one
Zone is active, it may use up to 15 Member Channels (“mm” = 0xF).

No MIDI Channel shall be assigned to more than one Zone at a time so, in the case where an MCM configures a
Zone to include MIDI Channels that were previously assigned to another Zone, the most recent message shall take precedence (those MIDI Channels are reassigned to the newer Zone). If this results in a Zone having no Member Channels, that Zone shall be deactivated.

Example MCM messages can be found in Appendix B.


### 2.2.2 Power-On Default Behavior

A Device may be configured to MPE Mode on power-up.

If a Device defaults to MPE Mode, then it should be configured to use the Lower Zone using Member Channels
2–16 in Poly Mode [MIDI Mode 3]. This would provide a good initial experience in monotimbral operation.
However, a Device may override its default mode. For example, a user might define a program with a different
configuration which is active as a default.

### 2.2.3 Receiver Behavior when Resetting Zones

To avoid the possibility of a sender leaving a receiver with hanging Sounding Notes when changing Zone
configurations, when a receiver changes its Zone configurations, the receiver shall stop all Sounding Notes and reset all controls to reasonable default values on each Channel entering or leaving MPE control.

### 2.2.4 MIDI Mode Messages

The default MIDI Mode for MPE Senders and MPE Receivers shall be MIDI Mode 3.

### 2.2.4.1 MIDI Mode 3 (“Poly Mode”)

MPE can be used with MIDI Mode 3 (Omni Off, Poly). In this Mode, a Channel is maximally polyphonic: it will
handle as many simultaneous notes as possible. When using MIDI Mode 3, an MPE controller shall assign every
new note its own MIDI Channel, until there are no unoccupied Channels available. An occupied Channel becomes
unoccupied when its Active Notes have all sent or received Note Off messages.
When there are more notes than unoccupied Channels, a new note shall share a MIDI Channel with an existing
note. Since Control Change and Pitch Bend are Channel Messages, they then affect all Active Notes on that
Channel.

When there is more than one concurrent Active Note on a Member Channel, implementation of how controllers
affect the notes is up to the Device.

Note: Recommendations about the ordering of note and control messages in MPE that help to improve
compatibility, editability, and the quality of rendered sound are presented in Appendix A.

### 2.2.4.2 MIDI Mode 4 (“Mono Mode”)

MPE may optionally be used with MIDI Mode 4 (Omni Off, Mono). Channels configured for MIDI Mode 4 are
monophonic. Starting a note in such a Channel when one is already playing shall stop the older note, possibly
invoking a legato transition between the old and the new notes. This mode is thus ideal for controllers that model stringed instruments, in which a ‘one Channel per string’ allocation assists realistic rendering of hammer-on and pull-off. MIDI Mode 4 is also suitable for controlling a collection of monophonic synthesizers.

When MPE is used with MIDI Mode 4, the MIDI 1.0 Global Channel for Global Controllers shall not be used.
MPE Devices are not required to support MIDI Mode 4.

### 2.2.4.3 Switching MIDI Modes

The default MIDI mode for MPE Senders and Receivers is MIDI Mode 3.

MPE senders which wish to switch receivers between MIDI Mode 3 and Mode 4, should send the appropriate
Mode Message to the lowest numbered Member Channel of a zone (not to the Manager Channel). MPE Devices
are not required to support MIDI Mode 4.

### 2.2.5 Pitch Bend Sensitivity

When a receiver receives an MPE Configuration Message, it shall set the Manager Channel Pitch Bend Sensitivity
to 2 semitones, and the Pitch Bend Sensitivity of every Member Channel to 48 semitones. The values may
subsequently be changed at any time using Registered Parameter Number [RPN] 0, in accordance with the MIDI
1.0 Specification. Because the Zone Pitch Bend Sensitivity on the Manager Channel is controllable independently from that of the Member Channels, setting them is accomplished as follows:

- Manager Channel Pitch Bend Sensitivity is set by sending RPN 0 to the Manager Channel.
- Pitch Bend Sensitivity on the Member Channels is set by sending RPN 0 to every Member Channel individually.

Member Channels within the same Zone shall not have different Pitch Bend Sensitivity values. A receiver shall
apply the last Pitch Bend Sensitivity message received on any Member Channel to all Member Channels in the
Zone. Sending Pitch Bend Sensitivity to every Member Channel individually improves compatibility with all
MIDI Devices.

Some devices which do not natively support MPE can be configured manually by the user to work well with MPE
or be highly compatible to MPE. A typical, non-MPE, 16 part synthesizer, often could be used in this manner. A
collection of monosynths spread across a number of channels can also be used in this manner.

The use of RPN 0 presents the option of supplying a less significant byte (LSB), for conveying the microtonal
fractions of Pitch Bend Sensitivity. It is recommended that MPE Devices use an integer number of semitones for
the range and either transmit the LSB as zero, or not transmit it at all so that the receiver infers zero. MPE Devices may still choose to respond to 14-bit Pitch Bend Sensitivity messages for compatibility with other equipment.

To simplify interface design, MPE Devices may limit their communication to a whole number of semitones
between 0 and 96. (At 96 semitone resolution, the granularity of 14-bit Pitch Bend data is still smaller than 1.2 cents.)

### 2.2.6 Pitch Bend

An MPE Device may send Pitch Bend messages on both the Manager Channel and on Member Channels. On the
Manager Channel, Pitch Bend is typically performed through movement of a global control (for example, a pitch
wheel or a tremolo bar). On Member Channels, Pitch Bend is typically performed by the movement of a single
finger on the playing surface.

The pitch of a new note is affected by the Pitch Bend message most recently received on both the Manager
Channel and that note’s Member Channel before Note On. A receiver shall continue to track Pitch Bend messages
from both the Manager Channel and the Member Channels even when no note is playing. Messages on the
Manager Channel continue to affect all Sounding Notes even after the Note Off message occurs. A Released Note
shall cease to be affected by Pitch Bend messages from the Member Channels after the Note Off message occurs.

If a Device receives Pitch Bend on both a Manager Channel and Member Channel, it shall combine such data
meaningfully and separately for each Sounding Note. It’s left to the manufacturer how to meaningfully combine
Manager Channel and Member Channel Pitch Bend data.

A number of examples and strategies are provided in Appendix C.

### 2.2.7 Channel Pressure and Polyphonic Key Pressure

An MPE Device may send Channel Pressure messages both on the Manager Channel and on Member Channels to
convey pressure. Polyphonic Key Pressure shall not be sent on Member Channels. Polyphonic Key Pressure may be sent for notes on the Manager Channel at the discretion of the implementer, to preserve compatibility with non-MPE-aware Devices.

The control of a new note is affected by the Channel Pressure message most recently received on its Channel
before Note On. A receiver shall continue to track Channel Pressure messages even when no note is playing.
Channel Pressure also influences the note’s initial state. The note will cease to be affected by Channel Pressure messages on its Channel after the Note Off message occurs.

All MPE receivers shall respond to Channel Pressure both on the Manager Channel and on each Member Channel.
If a Device receives Channel Pressure on both a Manager Channel and a Member Channel, then it shall combine
such data meaningfully and separately for each Sounding Note. It’s left to the manufacturer how to meaningfully combine Manager Channel and Member Channel, Channel Pressure data.

A number of examples and strategies are provided in Appendix D.

### 2.2.8 Control Change #74

In addition to Pitch Bend and Channel Pressure, MPE controllers may provide a third dimension of continuous
control. For example, some instruments inspired by the piano keyboard can track finger movement along the
length of the key. This additional dimension is mapped to Control Change #74. (See Appendix D).

The control of a new note is affected by a Control Change #74 message most recently received on its Channel
before Note On. Thus, a receiver shall continue to track Control Change #74 messages even when no note is
playing. Control Change #74 also influences the note’s initial state. The note will cease to be affected by Control

Change #74 messages on its Channel after the Note Off message occurs.

All MPE receivers shall respond to Control Change #74 on the Manager Channel as well as the Member
Channels. If a Device receives Control Change #74 on both a Manager Channel and Member Channels, it shall
combine such data meaningfully and separately for each Sounding Note. It’s left to the manufacturer how to
meaningfully combine this data. A number of examples and strategies are provided in Appendix D.

## 2.3 Manager Channel Messages and Member Channel Messages

### 2.3.1 Messages Only on Manager Channel

An MPE Zone normally represents one polyphonic instrument in which certain MIDI messages (for example,
Damper Pedal) can be expected to affect all Sounding Notes across the Manager Channel and all Member
Channels.

To reduce MIDI traffic and make event editing easier, those messages should be sent only on a Zone’s Manager
Channel (not on Member Channels). If an MPE Device receives any of those messages on a Member Channel, it
should ignore them. See Appendix E for other MIDI messages that are Manager Channel Messages but not
Member Channel Messages.

### 2.3.2 Messages on Both Manager Channel and Member Channels

Some MIDI messages are used on both the Manager Channel and on Member Channels. For example, Pitch Bend
messages from a pitch wheel on a typical MIDI controller affect all Sounding Notes, which makes them Manager
Channel Messages.

However, MPE also offers per-note Pitch Bend. Therefore, Pitch Bend is both a Manager Channel Message and a
Member Channel Message. If an MPE synthesizer receives Pitch Bend (for example) on both a Manager and a
Member Channel, it shall combine the data meaningfully.

The same is true for Channel Pressure and Control Change #74. See the table in Appendix E for other MIDI
messages that are both Manager Channel and Member Channel Messages.

Appendix C and Appendix D address MPE Receiver behavior when these messages are sent both on the Manager
Channel and on Member Channels, including suggested implementation strategies for handling the possible
interactions.

### 2.3.3 Channels for Program Change

Program Change is a special case. In MIDI Mode 3 it shall be applied only at the Manager Channel to enable
monotimbral playing across an entire Zone. In MIDI Mode 4, however, synthesizers might accommodate different
programs on different MIDI Channels within the Zone, to allow controllers that imitate string instruments to
assign a different program to each individual string. Therefore, Program Change messages may be sent on
Member Channels when a Device is operating in MIDI Mode 4, and a receiver operating in MIDI Mode 4 should
apply Program Change messages received on Manager Channels and Member Channels in the order in which they
are received. A receiver operating in MIDI Mode 3 shall ignore Program Change messages received on Member
Channels. See Appendix E for other MIDI messages that are special cases.

See Appendix E for a table indicating which MPE messages are used on the Manager Channel, which messages
are used on Member Channels, and which messages are used on both.

## 2.4 MPE Control Messages and Note On-Off messages

Senders that use MPE control messages (Pitch Bend, Channel Pressure, Control Change #74) should send initial
values for these controls before a Note On message. The order in which these controllers are sent does not matter.

Senders that also use other controller messages may decide whether sending an initial value for those controllers is necessary or not.

If the Sender does not use this mechanism, the Receiver will likely play notes with its own current values for these properties, which might not match the user intention or expectation.
For Receivers, the MPE control messages shall not affect a note after the Note Off message has been received.

### 2.4.1 Note On Setup Example

To play a note that sounds one quarter tone above middle C with an initial timbre value of 64, the following
controllers would be sent prior to the Note On (using MIDI Channel 3 as an example):

Table 4 Note On Setup Example

| Sequence | MIDI Bytes      | Description       | Effect                                                                 |
|----------|-----------------|-------------------|------------------------------------------------------------------------|
| 1        | 0xE2 0x2B 0x41  | Pitch Bend        | Quartertone bend upwards, assuming sensitivity has been set to 48 semitones |
| 2        | 0xB2 0x4A 0x40  | Controller Change | CC #74 = 0x40 (64 decimal)                                             |
| 3        | 0xD2 0x00       | Channel Pressure  | Set to zero (Appendix D)                                               |
| 4        | 0x92 0x3C 0x38  | Note On           | Note = Middle C, Velocity = 0x38 (56 decimal)                          |



# Appendix A : Best Practices for Compatibility Between Senders and Receivers.

These are guidelines based on experience and actual implementation is left to the manufacturer.

## A.1 MCM Strategies

The MPE Specification was developed before MIDI-CI, therefore bidirectional connections can lead to a MIDI feedback loop or an endless loop of changing MCM messages. Below are examples of strategies that can be adopted to prevent an MCM feedback loop from happening:
- Ignore a received MCM if an MCM was already sent by your Device.
- Adapt to a received MCM and change your Device’s internal MCP configuration without sending out a new MCM to confirm the new configuration.
- Ignore a received MCM if your Device can’t adapt to the MCM configuration and don’t send out a
new MCM to try to make other Devices change to your Device’s limitations.

## A.2 Use of MPE Zones

Although MPE supports the creation of two Zones, many MPE Devices only support one MPE Zone. These
Devices might only use the Lower Zone or might provide a way for the user to choose which Zone to use. Using
the Lower Zone by default provides the widest interoperability.

Where interface design permits, an instrument or controller should be able to display the currently selected
Manager Channel and the range of Member Channels.

## A.3 Allocation of Notes to Member Channels

An MPE Sender determines the allocation of each note to a Channel.
Simple circular assignment of new notes to Member Channels of a Zone will not usually provide satisfactory
results. In the simplest workable implementation, a new note will be assigned to the Channel with the lowest count
of Active Notes. Then, all else being equal, the Channel with the oldest last Note Off would be preferred.

MPE controllers can preferentially re-use a Channel that has been most recently deployed to play a certain Note
Number once the previous note has entered its Note Off state. This avoids stacking and chorusing identical notes,
which sounds bad in monotimbral applications, and affects synthesizers that are not designed specifically for
MPE.

However, in particular circumstances it is appropriate to have the same Note Number active on two different
MIDI Channels. For example, a note may start at a certain pitch and be bent to another before a second note is
initiated at the original pitch. Alternatively, a guitar-type controller might permit the same pitch to be played
simultaneously on different strings.

When all Channels are occupied by Active Notes, a controller may choose the Channel in which the change of
pitch for the new note requires the smallest adjustment of pitch for other playing notes. Alternatively, a controller
may provide gentle degradation of pitch control when all Channels are occupied by switching to a mode where
notes step discretely from one pitch to the next, permitting Pitch Bend to respond only to small vibrato gestures.

## A.4 Maximizing Compatibility and Sound Quality Under MPE

A good MPE design should be as transparent to the user as possible and may require a dedicated “MPE Mode”.

The ramifications for designers of more sophisticated MIDI Devices can be fairly demanding, particularly where
MIDI data can be edited, merged, and looped.

Making the MPE workflow transparent presents three challenges:

- Note Editing across channels. Editing MPE sequences should seamlessly handle notes across multiple
Channels, without requiring the user to determine MIDI Channel assignment for each note.
Notes might be moved and inserted, and MIDI streams might be merged. To make this easier in Poly Mode,
originating Channel numbers do not have to be preserved during editing. Member Channels could be
dynamically reassigned during playback or retransmission.
- Mono Mode and standard MIDI behavior still requires preservation of Channel numbers. From a
programmer’s perspective, this entails a far more sophisticated note model. A note is no longer just a pair of
time-stamped Note On and Note Off messages: it must become an entity with its own timeline of
multidimensional control data that can be moved across the time and channel spaces along with the note.
- Controller Message and Note State Devices and systems need to determine note state based on control data
from the Manager Channel and from Member Channels.

Values for Pitch Bend, Channel Pressure, Control Change #74, and all other control messages from both the
Manager Channel and the Member Channels should be tracked and stored even when there is no Sounding
Note, to provide an initial state for any future note (see Sections 2.2.6, 2.2.7, and 2.2.8).
In order to allow rapid reuse of unoccupied Member Channels, per-note control should stop after receiving the
Note Off message, regardless of whether notes are kept active by a damper pedal or long release envelopes.

## A.4.1 Pitch Bend

As stated in Section 2.2.6, per Member Channel Pitch Bend control ceases once Note Off has occurred.
Any feature which requires continual transmission of Pitch Bend shall send those Pitch Bend messages before
sending the Note Off.

However, Manager Pitch Bend applies to every Sounding Note within the Zone, even those that have passed into
their Note Off phase and are sustained only by a pedal or a release envelope.

## A.4.2 Channel Pressure

Many synthesizers are designed for use with controllers that set Channel Pressure to zero before the termination of
a note. For full compatibility with these, and to simplify sound design on MPE-compatible synthesizers, Channel
Pressure should be set to zero immediately before a Note On or a Note Off wherever it is appropriate to the design
of a controller.

Not all controllers can be expected to behave in this way. In the simulation of certain hand drums, for example,
pressure applied to the drum skin is adjustable independently from the note creation mechanism.

## A.4.3 Timbre Control

If a Sender offers a third dimension of control, it shall use Control Change #74 as defined in Section 2.2.8. This is
typically used to control timbre. Broadly, there are two schemes for transmitting this dimension from a control instrument:

- Absolute: The value of Control Change #74 at the time of Note On encodes the initial position of a performer’s
interaction with the instrument.
- Relative: The value of Control Change #74 at the time of Note On is always 0x40 (64 decimal), regardless of
the initial position of a performer’s interaction with the instrument. Subsequent movement of the performer's
interaction can follow in either a positive or negative direction.

For Example: Consider a controller keyboard which offers expressive, positional control from the bottom to the
top of a key.

When implementing Absolute control, if the performer plays the key starting from 75 percent from the bottom,
then the controller will send a value of 0x5F (95 decimal). Subsequent motion on the key from bottom to top will
send values from 0x00 to 0x7F (127 decimal) proportional to the height of the key.

When implementing Relative control, if the performer plays the key starting from 75 percent from the bottom,
then the controller will send a value of 0x40 (64 decimal). Subsequent motion on the key from bottom to top will
send values from 0x00 to 0x7F (127 Decimal) proportional to the position of the initial touch.

The choice of either scheme, or a mix of both, is up to the manufacturer.


# Appendix C : Pitch Bend Calculation in MPE

## C.1 Handling Pitch Bend in MPE
- The Manager Channel and the Member channels may have different Pitch Bend Sensitivities.
- The total Pitch Bend for a note should be the sum of the Pitch Bend for the Member Channel and the Pitch Bend of the Manager channel.
- Because Pitch Bend may span across multiple semitones, Pitch Bend should be linear across the
sensitivity range.

## C.2 Message
The RPN 0 message for a Manager or Member Channel is used to set Pitch Bend Sensitivity.

MSB LSB Function
=========================================
00 00 Pitch Bend Sensitivity RPN
Message Format: [0xBn 0x65 0x00] [0xBn 0x64 0x00] [0xBn 0x06 <sensitivity>]
Where n = MIDI Channel Number:
And sensitivity = +/- the range of Pitch Bend in semitones

## C.3 Equations for Senders

If an MPE controller sends Pitch Bend on a Member Channel or Manager Channel in a pitch-precise way dependent on the active Pitch Bend Sensitivity, it could calculate the data in the following way. Note that this is purposefully asymmetrical with the equations for Receivers described below due to the neutral Pitch Bend value being 8192, resulting in the upwards range being different from the downwards range.

- With pbSenseMember the +/- range of Pitch Bend in semitones and pbMem the Pitch Bend in semitones for the Member Channel, the Pitch Bend value for the Member Channel is pbValMem:

    pbValMem = min(round(pbMem * 8192 / pbSenseMember) + 8192, 16383)

- With pbSenseManager the +/- range of Pitch Bend in semitones and pbMan the Pitch Bend in semitones for the Manager Channel, the Pitch Bend value for the Manager Channel is pbValMan:

    pbValMan = min(round(pbMan * 8192 / pbSenseMan) + 8192, 16383)


## C.4 Sender Example

- The Member Channels have a Pitch Bend Sensitivity of 48 and the Manager Channel has a Pitch Bend Sensitivity of 2.
- The Member Channel has a Pitch Bend of +7 semitones
- The Manager Channel has Pitch Bend of + 2 semitones
- Here is an example of the computation:
    pbValMem = min(round(7 * 8192 / 48) + 8192, 16383) = 9387 Pitch Bend value
    pbValMan = min(round(2 * 8192 / 2) + 8192, 16383) = 16383 Pitch Bend value

## C.5 Equations for Receivers

If an MPE synthesizer receives Pitch Bend on a Manager and a Member Channel, it could combine the data in the following way

- With pbSenseMan being the +/- range of Pitch Bend in semitones. The Pitch Bend in semitones for the Manager Channel is:
    pMan = (pbSenseMan*((pbValMan - 8192)/8191)
- With pbSenseMember the +/- range of Pitch Bend in semitones. The Pitch Bend in semitones for the Member Channel is:
    pbMem = (pbSenseMem*((pbValMem - 8192)/8191)
- The total Pitch Bend in semitones of the sum of Manager Channel and Member channel Pitch Bends.
    pbTotal = pbMan + pbMem
- The variables pbMan and pbMem should be stateful so that when pbTotal is computed it is the sum of the most recent values for these variables.

## C.6 Receiver Example
- The Manager Channel has a Pitch Bend Sensitivity of 2 and the Member Channels have a Pitch Bend Sensitivity of 48.
- The manager channel has Pitch Bend of +2 semitones with a value of 16383
- The member channel has a Pitch Bend of +7 semitones with a value of 9387
- Here is an example of the computation:
    pbMan = 2*((16383-8192)/8191)) = 2 semitones
    pbMem = 48*((9387-8192)/8191) = 7 semitones
    pbTotal = 9 semitones


# Appendix D : Handling Channel Pressure and Control Change #74 in MPE

Typical uses for Channel Pressure and/or Control Change #74 might be for volume (a swell), expression, or a filter cutoff.

Channel Pressure is often generated by a pressure sensor and typically starts with a value of 0x00 at the time of Note On and ends with 0x00 at the time of Note Off.

Control Change #74 is often generated by a vertical position on a key. Note that Control Change #74 may not necessarily start from or end with a a value of 0x00

The Manager Channels and the Member Channels may both send these controls to a Sounding Note. There are several possible ways that these controls coming from both Manager Channel and Member Channel might be combined by a receiver.

- Add: The two controller values might be added together. As an example, Control Change #74 might be used to control a filter cutoff on a per Member Channel basis. The Manager Channel might also send a value for Control Change #74 which is intended to be a bias or Manager offset.
- Max: The max value of the two controller values might be used. As an example, Channel Pressure might be used to control volume (a swell) on a per Member Channel basis. The Manager Channel might also send a value for Channel Pressure which serves as an offset from the Sounding Note’s current value.
- Contention: It is possible that the controller values from the Manager Channel and the Member Channel could contend. This is not recommended. The actual implementation is left to the manufacturer.


# Appendix E : MIDI Messages Used on MPE Channels

Table 5 MIDI Messages Used on MPE Channels

| MIDI Message or Feature | Manager Tx | Manager Rx | Member Tx | Member Rx | Details |
|-------------------------|------------|------------|-----------|-----------|---------|
| RPN #6 [MPE Configuration Message] | M | M | P | P | See Section 2.2.1 |
| RPN #0 [Pitch Bend Sensitivity] | O | M | O | M | See Section 2.2.5, See Appendix C |
| Pitch Bend | O | O | M | M | See Section 2.2.6 |
| Channel Pressure | O | O | M | M | See Section 2.2.7 |
| Control Change #74 [Brightness] | O | O | M | M | See Section 2.2.8 |
| Polyphonic Key Pressure | O | O | P | P | See Section 2.2.7 |
| Control Change #120 [All Sounds Off] | P | O | O | O | See Section 2.2.4 |
| Control Change #121 [Reset all CC] | O | O | O | O | |
| Control Change #122 [Local Control] | O | O | O | O | |
| Control Change #123 [All Notes Off] | O | O | O | O | |
| Control Change #124 [Omni Off] | O | O | O | O | |
| Control Change #125 [Omni On] | P | O | O | O | |
| Control Change #126 [Mono Mode On] | P | P | P | P | See Section 2.2.4.3, Lowest Member Channel |
| Control Change #127 [Poly Mode On] | O | O | O | O | |
| All other Control Change, RPN, NRPN messages (e.g. CC#1, #33 [Modulation], CC#7, #39 [Volume], CC#64 [Damper Pedal]) | O | O | O | O | See Section 2.3 |
| Program Change, Bank Select (CC#0, CC#32) | O | O | P/O | P/O | See Section 2.3.3. On Member Channels. Prohibited in MIDI Mode 3. Optional in MIDI Mode 4. |
| Note On/Off messages | O | M | M | M | See Section 2.4 |
| System Common | O | O | O | O | |
| System Realtime | O | O | O | O | |
| System Exclusive | O | O | O | O | |

Tx: Transmit Rx: Receive

M: Mandatory O: Optional P: Prohibited

# Appendix F : Implementation Summary Tables

### Table 6 Implementation Summary for Senders

| Action | Method | Details |
|--------|--------|---------|
| Send the MCM message to enable MPE and assign Channels to Zones | Send up to two RPN 6 messages, on Channel 1 and Channel 16, to allocate Channels to the Lower and Upper Zones. | See Section 2.2.1, See Appendix B |
| Send the Pitch Bend Sensitivity on the Manager Channel (Zone) | Send a RPN 0 message (whole number of semitones recommended, range 1 to 96) on the Manager Channel. Default is 2 semitones. | See Section 2.2.5, See Appendix C |
| Send the Pitch Bend Sensitivity on Member Channels (note Channels) | Send individual RPN 0 messages (whole number of semitones recommended, range 1 to 96) to every Member Channel. Receiver Default is 48 semitones. | See Section 2.2.5, See Appendix C |
| Control Pitch Bend, for a Zone | Send a MIDI Pitch Bend message to the Manager Channel. | See Section 2.2.6, See Appendix C |
| Control Pitch Bend, for a note | Send a MIDI Pitch Bend message to the note's Channel. | See Section 2.2.6, See Appendix C |
| Control Pressure/Aftertouch, for a Zone | Send a Channel Pressure message to the Manager Channel. Polyphonic Key Pressure may be recognized on the Manager Channel for compatibility with existing MIDI usage. | See Section 2.2.7, See Appendix D |
| Control Pressure/Aftertouch, for a note | Send Channel Pressure message to the note's Channel. Do not send Polyphonic Key Pressure on a Member Channel. | See Section 2.2.7, See Appendix D |
| Control Third dimension, for a Zone (usually mapped to timbre) | Send Control Change #74 to the Manager Channel. | See Section 2.2.8, See Appendix D |
| Control Third dimension, for a note (usually mapped to timbre) | Send Control Change #74 to the note's Channel. | See Section 2.2.8, See Appendix D |
| Control Pedals, Reset All Controllers, and other Zone data | Send the appropriate message on the Manager Channel of the affected Zone. | See Section 2.3.3 |
| Send Program Change | Send the appropriate message on the Manager Channel of the affected Zone. | See Section 2.3 |
## Table 7 Implementation Summary for Receivers

| Action | Method | Details |
|--------|--------|---------|
| Respond to the MCM message | Stop all ongoing notes, assign Channels to Zones, and reset all controls to reasonable default values on each Channel entering or leaving MPE control. Set Manager Channel's Pitch Bend Sensitivity to the Default Value of 2. Set all Member Channel's Pitch Bend Sensitivity to the Default Value of 48. | See Section 2.2.1, See Appendix B, See Section 2.2.5 |
| Respond to the Pitch Bend Sensitivity for a Manager Channel (Zone) | Respond to an RPN 0 message (whole number of semitones recommended range 1 to 96) on the Manager Channel. Default is 2 semitones. | See Section 2.2.5, See Appendix C |
| Respond to the Pitch Bend Sensitivity for Member Channels (Note Channels) | Respond individual RPN 0 messages (whole number of semitones recommended range 1 to 96) to every Member Channel. Receiver Default is 48 semitones. | See Section 2.2.5, See Appendix C |
| Respond to Pitch Bend, for a Zone | Respond to a MIDI Pitch Bend message to the Manager Channel. | See Section 2.2.6, See Appendix C |
| Respond to Pitch Bend, for a note | Respond to a MIDI Pitch Bend message to the note's Channel. | See Section 2.2.6, See Appendix C |
| Respond to Pressure/Aftertouch, for a Zone | Respond to Channel Pressure message to the Manager Channel. Polyphonic Key Pressure may be recognized on the Manager Channel for compatibility with existing MIDI usage. | See Section 2.2.7, See Appendix D |
| Respond to Pressure/Aftertouch, for a note | Respond to Channel Pressure message to the note's Channel. Ignore Polyphonic Key Pressure on a Member Channel. | See Section 2.2.7, See Appendix D |
| Respond to Third Dimension, for a Zone. Usually mapped to timbre | Respond to Control Change #74 to the Manager Channel. | See Section 2.2.8, See Appendix D |
| Respond to Third Dimension, for a note. Usually mapped to timbre | Respond to Control Change #74 to the note's Channel. | See Section 2.2.8, See Appendix D |
| Respond to Pedals, Reset All Controllers, and other Zone data | Respond to the appropriate message on the Manager Channel of the affected Zone. | See Section 2.3 |
| Respond to Program Change | Respond to messages on the Manager Channel. Respond to messages to the Member Channels when in MIDI Mode 4. | See Section 2.3.3 |