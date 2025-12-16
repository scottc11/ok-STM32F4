**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

**DACx311 2-V to 5.5-V, 80-µA, 8-Bit, 10-Bit, and 12-Bit, Low-Power, Single-Channel, Digital-to-Analog Converters in SC70 Package** 

**1 Features** 

• Relative accuracy: 

– 0.25 LSB INL (DAC5311: 8-bit) 

– 0.5 LSB INL (DAC6311: 10-bit) 

– 1 LSB INL (DAC7311: 12-bit) 

• *micro*Power operation: 80 μA at 2.0 V • Power-down: 0.5 μA at 5 V, 0.1 μA at 2.0 V • Wide power supply: 2.0 V to 5.5 V 

• Power-on reset to zero scale 

• Straight binary data format 

• Low power serial interface with Schmitt-triggered  inputs: up to 50 MHz 

• On-chip output buffer amplifier, rail-to-rail operation • SYNC interrupt facility 

• Extended temperature range –40°C to \+125°C • Pin-compatible family in a tiny, 6-pin SC70  package 

**2 Applications** 

• Portable, battery-powered instruments • 4-mA to 20-mA loop-powered applications • Process control and industrial automation • Programmable voltage and current sources 

AVDD GND 

Power-On   
Reset   
**3 Description** 

The 8-bit DAC5311, 10-bit DAC6311, and 12-bit  DAC7311 (DACx311) are low-power, single-channel,  voltage output digital-to-analog converters (DACs).  The low power consumption in normal operation (0.55  mW at 5 V, reducing to 2.5 μW in power-down mode)  makes the DACx311 an excellent choice for portable,  battery-operated applications. 

These devices are monotonic by design, provide  excellent linearity, and minimize undesired code to-code transient voltages while offering an easy  upgrade path within a pin-compatible family. All  devices use a versatile, three-wire serial interface  that operates at clock rates of up to 50 MHz, and is  compatible with standard SPI, QSPI, Microwire, and  digital signal processor (DSP) interfaces. 

All devices use an external power supply as a  reference voltage to set the output range. The devices  incorporate a power-on reset (POR) circuit that  powers up the DAC output at 0 V and remains at 0 V  until a valid write to the device occurs. The DACx311  contain a power-down feature, accessed over the  serial interface, that reduces current consumption of  the device to 0.1 μA at 2.0 V in power-down mode. 

These devices are pin-compatible with the DAC8311  and DAC8411, offering an easy upgrade path from  

DAC   
REF(+) 

Output   
8‑bit, 10‑bit, and 12‑bit resolution to 14‑bit and    
Register 8-/10-/12-Bit DAC 

Buffer   
VOUT 

16‑bit. All devices are available in a small, 6‑pin,  SC70 (SOT) package. This package offers a flexible,  pin- and function-compatible, drop-in DAC within the  

Input Control   
Logic 

SYNC SCLK DIN   
Power-Down 

Control Logic Resistor Network 

**Simplified Schematic**   
family over an extended temperature range of –40°C  to \+125°C. 



**www.ti.com** 

**5 Device Comparison** 

**Table 5-1. Related Devices**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

| RELATED DEVICES  | 16-BIT  | 14-BIT  | 12-BIT  | 10-BIT  | 8-BIT |
| :---: | :---: | :---: | :---: | :---: | :---: |
| Pin and Function Compatible  | DAC8411  | DAC8311  | DAC7311  | DAC6311  | DAC5311 |

**Table 5-2. Relative Accuracy and**  

**Differential Nonlinearity** 

| DEVICE | MAXIMUM  RELATIVE  ACCURACY  (LSB) | MAXIMUM  DIFFERENTIAL  NONLINEARITY  (LSB) |
| :---: | :---: | :---: |
| DAC5311  | ±0.25  | ±0.25 |
| DAC6311  | ±0.5  | ±0.5 |
| DAC7311  | ±1  | ±1 |

**6 Pin Configuration and Functions** 

SYNC 1 6 VOUT   
SYNC 1 6 VOUT 

SCLK 2 5 GND   
SCLK 2 5 GND 

DIN 3 4 AVDD/VREF   
DIN 3 4 AVDD/VREF 

Not to scale   
Not to scale 

**Figure 6-1. DCK Package, 6-Pin SC70 (Top View)** 

**Table 6-1. Pin Functions** 

| PIN |  | TYPE  | DESCRIPTION |
| ----- | :---: | :---: | ----- |
| **NAME**  | **NO.** |  |  |
| AVDD/VREF  | 4  | Input  | Power supply input, 2.0 V to 5.5 V. |
| DIN  | 3  | Input  | Serial Data Input. Data are clocked into the 16-bit input shift register on the falling edge of the serial  clock input. |
| GND  | 5  | —  | Ground reference point for all circuitry on the part. |
| SCLK  | 2  | Input  | Serial clock input. Data are transferred at rates up to 50 MHz. |
| ~~SYNC~~  | 1  | Input | Level-triggered control input (active low). This pin is the frame synchronization signal for the input  data. When SYNC goes low, the input shift register is enabled and data are transferred in on the  falling edges of the following clocks. The DAC is updated following 16th clock cycle, unless SYNC  is taken high before this edge, in which case the rising edge of SYNC acts as an interrupt and the  write sequence is ignored by the DACx311. See the *SYNC Interrupt* section for more details. |
| VOUT  | 6  | Output  | Analog output voltage from DAC. The output amplifier has rail-to-rail operation. |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 3 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com 7 Specifications**   
**7.1 Absolute Maximum Ratings** 

over operating free-air temperature range (unless otherwise noted) (1) 

|  |  |  | MIN MAX  | UNIT |
| :---- | :---- | :---- | ----- | :---: |
|  | Voltage | AVDD to GND  | –0.3 \+6  | V |
|  |  | Digital input voltage to GND  | –0.3 \+AVDD \+ 0.3  | V |
|  |  | VOUT to GND  | –0.3 \+AVDD \+ 0.3  | V |
| TJ  | Junction temperature  |  | 150  | °C |
| Tstg  | Storage temperature  |  | –65 150  | °C |

(1) Stresses beyond those listed under *Absolute Maximum Ratings* may cause permanent damage to the device. These are stress  ratings only, which do not imply functional operation of the device at these or any other conditions beyond those indicated under  *Recommended Operating Conditions*. Exposure to absolute-maximum-rated conditions for extended periods may affect device  reliability. 

**7.2 ESD Ratings** 

|  |  |  | VALUE  | UNIT |
| :---- | :---- | :---- | :---: | :---: |
| V(ESD) | Electrostatic   discharge | Human-body model (HBM), per ANSI/ESDA/JEDEC JS-001(1)  | ±1000 | V |
|  |  | Charged-device model (CDM), per JEDEC specification JESD22-C101(2)  | ±500 |  |

(1) JEDEC document JEP155 states that 500-V HBM allows safe manufacturing with a standard ESD control process. (2) JEDEC document JEP157 states that 250-V CDM allows safe manufacturing with a standard ESD control process. 

**7.3 Recommended Operating Conditions** 

over operating free-air temperature range (unless otherwise noted) 

|  |  | MIN NOM MAX  | UNIT |
| :---- | :---- | ----: | :---: |
| TA  | Operating temperature  | –40 125  | °C |
| AVDD  | Supply voltage  | 2 5.5  | V |

**7.4 Thermal Information** 

| THERMAL METRIC(1) |  | DACx311 | UNIT |
| ----- | :---- | :---: | :---: |
|  |  | **DCK (SC70)**  |  |
|  |  | **6 PINS** |  |
| RθJA  | Junction-to-ambient thermal resistance  | 216.4  | °C/W |
| RθJC(top)  | Junction-to-case (top) thermal resistance  | 52.1  | °C/W |
| RθJB  | Junction-to-board thermal resistance  | 65.9  | °C/W |
| ψJT  | Junction-to-top characterization parameter  | 1.3  | °C/W |
| ψJB  | Junction-to-board characterization parameter  | 65.2  | °C/W |
| RθJC(bot)  | Junction-to-case (bottom) thermal resistance  | N/A  | °C/W |

(1) For more information about traditional and new thermal metrics, see the application report, *Semiconductor and IC Package Thermal  Metrics* application note. 

4 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.5 Electrical Characteristics**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at AVDD \= 2.0 V to 5.5 V, RL \= 2 kΩ to GND, CL \= 200 pF to GND, and TA \= –40°C to \+125°C (unless otherwise noted) 

| PARAMETER  |  | TEST CONDITIONS  | MIN TYP MAX  | UNIT |
| ----- | :---- | ----- | ----- | :---: |
| **STATIC PERFORMANCE**(1) |  |  |  |  |
| DAC5311 | Resolution |  | 8  | Bits |
| DAC6311  |  |  | 10  | Bits |
| DAC7311  |  |  | 12  | Bits |
| DAC5311 | Relative accuracy | Measured by the line passing through codes  3 and 252  | ±0.01 ±0.25  | LSB |
| DAC6311  |  | Measured by the line passing through codes  12 and 1012  | ±0.06 ±0.5  | LSB |
| DAC7311  |  | Measured by the line passing through codes  30 and 4050  | ±0.3 ±1  | LSB |
| DAC5311 | Differential   nonlinearity |  | ±0.01 ±0.25  | LSB |
| DAC6311  |  |  | ±0.03 ±0.5  | LSB |
| DAC7311  |  |  | ±0.2 ±1  | LSB |
| Offset error  |  | Measured by the line passing through two  codes(2)  | ±0.05 ±4  | mV |
| Offset error drift  |  |  | 3  | μV/°C |
| Zero code error  |  | All zeros loaded to the DAC register  | 0.2  | mV |
| Full-scale error  |  | All ones loaded to DAC register  | 0.04 0.2  | % of   FSR |
| Gain error  |  |  | 0.05 ±0.15  | % of   FSR |
| Gain temperature coefficient |  | AVDD \= 5 V  | ±0.5  | ppm of   FSR/°C  |
|  |  | AV DD \= 2.0 V  | ±1.5 |  |
| **OUTPUT CHARACTERISTICS** |  |  |  |  |
| Output voltage range  |  |  | 0 AVDD  | V |
| Output voltage settling time(3) |  | RL \= 2 kΩ, CL \= 200 pF, AVDD \= 5 V, 1/4 scale to 3/4 scale  | 6 10  | μs |
|  |  | RL \= 2 MΩ, CL \= 470 pF  | 12  | μs |
| Slew rate  |  |  | 0.7  | V/μs |
| Capacitive load stability |  | RL \= ∞  | 470  | pF |
|  |  | RL \= 2 kΩ  | 1000  | pF |
| Code change glitch impulse  |  | 1 LSB change around major carry  | 0.5  | nV-s |
| Digital feedthrough  |  |  | 0.5  | nV-s |
| Power-on glitch impulse  |  | RL \= 2 kΩ, CL \= 200 pF, AVDD \= 5 V  | 17  | mV |
| DC output impedance  |  |  | 0.5  | Ω |
| Short circuit current |  | AVDD \= 5 V  | 50  | mA |
|  |  | AVDD \= 3 V  | 20  | mA |
| Power-up time  |  | Coming out of power-down mode  | 50  | μs |
| **AC PERFORMANCE** |  |  |  |  |
| SNR |  | TA \= 25°C, BW \= 20 kHz, 12-bit level, AVDD \= 5 V, fOUT \= 1 kHz, 1st 19 harmonics  removed for SNR calculation | 81  | dB |
| THD  |  |  | –65  | dB |
| SFDR  |  |  | 65  | dB |
| SINAD  |  |  | 65  | dB |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 5 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com 7.5 Electrical Characteristics (continued)**   
at AVDD \= 2.0 V to 5.5 V, RL \= 2 kΩ to GND, CL \= 200 pF to GND, and TA \= –40°C to \+125°C (unless otherwise noted) 

| PARAMETER  |  | TEST CONDITIONS  |  | MIN TYP MAX  | UNIT |
| ----- | ----- | ----- | :---- | ----- | :---: |
| DAC output noise density(4) |  | TA \= 25°C, at zero-scale input,  fOUT \= 1 kHz, AVDD \= 5 V  |  | 17  | nV/√Hz |
|  |  | TA \= 25°C, at mid-code input,  fOUT \= 1 kHz, AVDD \= 5 V  |  | 110  | nV/√Hz |
| DAC output noise(5)  |  | TA\= \+25°C, at mid-code input,  0.1 Hz to 10 Hz, AVDD \= 5 V  |  | 3  | μVPP |
| **LOGIC INPUTS**(3) |  |  |  |  |  |
| Input current  |  |  |  | ±1  | μA |
| VINL, Input low voltage |  | AVDD \= 2.7 V to 5.5 V  |  | 0.3 × AVDD  | V |
|  |  | AVDD \= 2.0 V to 2.7 V  |  | 0.1 × AVDD  | V |
| VINH, Input high voltage |  | AVDD \= 2.7 V to 5.5 V  |  | 0.7 × AVDD  | V |
|  |  | AVDD \= 2.0 V to 2.7 V  |  | 0.9 × AVDD  | V |
| Pin capacitance  |  |  |  | 1.5 3  | pF |
| **POWER REQUIREMENTS** |  |  |  |  |  |
| AVDD  |  |  |  | 2.0 5.5  | V |
| IDD | Normal mode | VINH \= AVDD and  VINL \= GND, at   midscale code(6) | AVDD \= 3.6 V to 5.5 V  | 110 180  | μA |
|  |  |  | AVDD \= 2.7 V to 3.6 V  | 95 150  | μA |
|  |  |  | AVDD \= 2.0 V to 2.7 V  | 80 140  | μA |
|  | All power-down mode | VINH \= AVDD and  VINL \= GND, at   midscale code(6) | AVDD \= 3.6 V to 5.5 V  | 0.5 3.5  | μA |
|  |  |  | AVDD \= 2.7 V to 3.6 V  | 0.4 3  | μA |
|  |  |  | AVDD \= 2.0 V to 2.7 V  | 0.1 2  | μA |
| Power dissipation | Normal mode | VINH \= AVDD and  VINL \= GND, at   midscale code(6) | AVDD \= 3.6 V to 5.5 V  | 0.55 0.99  | mW |
|  |  |  | AVDD \= 2.7 V to 3.6 V  | 0.25 0.54  | mW |
|  |  |  | AVDD \= 2.0 V to 2.7 V  | 0.14 0.38  | mW |
|  | All power-down mode | VINH \= AVDD and  VINL \= GND, at   midscale code(6) | AVDD \= 3.6 V to 5.5 V  | 2.50 19.2  | µW |
|  |  |  | AVDD \= 2.7 V to 3.6 V  | 1.08 10.8  | µW |
|  |  |  | AVDD \= 2.0 V to 2.7 V  | 0.72 8.1  | µW |

(1) Linearity calculated using a reduced code range of 3 to 252 for 8-bit, 12 to 1012 for 10bit, and 30 to 4050 for 12-bit, output unloaded. (2) Straight line passing through codes 3 and 252 for 8-bit, 12 and 1012 for 10-bit, and 30 and 4050 for 12-bit, output unloaded. (3) Specified by design and characterization, not production tested. 

(4) For more details, see Figure 7-23.   
(5) For more details, see Figure 7-24.   
(6) For more details, see Figure 7-16 and Figure 7-58. 

6 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.6 Timing Requirements** 

at –40°C to 125°C, and AVDD \= 2 V to 5.5 V (unless otherwise noted)(1)   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

|  |  | MIN NOM MAX  | UNIT |
| ----- | :---- | ----- | :---: |
| f(SCLK) Serial clock frequency | AVDD \= 2.0 V to 3.6 V  | 20 | MHz |
|  | AVDD \= 3.6 V to 5.5 V  | 50 |  |
| t1 SCLK cycle time | AVDD \= 2.0 V to 3.6 V  | 50 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 20 |  |
| t2 SCLK high time | AVDD \= 2.0 V to 3.6 V  | 25 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 10 |  |
| t3 SCLK low time | AVDD \= 2.0 V to 3.6 V  | 25 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 10 |  |
| t4 SYNC to SCLK rising edge setup time | AVDD \= 2.0 V to 3.6 V  | 0 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 0 |  |
| t5 Data setup time | AVDD \= 2.0 V to 3.6 V  | 5 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 5 |  |
| t6 Data hold time | AVDD \= 2.0 V to 3.6 V  | 4.5 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 4.5 |  |
| t7 SCLK falling edge to SYNC rising edge | AVDD \= 2.0 V to 3.6 V  | 0 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 0 |  |
| t8 Minimum SYNC high time | AVDD \= 2.0 V to 3.6 V  | 50 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 20 |  |
| t9 16th SCLK falling edge to SYNC falling edge | AVDD \= 2.0 V to 3.6 V  | 100 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 100 |  |
| t10SYNC rising edge to 16th SCLK falling edge (for successful SYNC interrupt) | AVDD \= 2.0 V to 3.6 V  | 15 | ns |
|  | AVDD \= 3.6 V to 5.5 V  | 15 |  |

(1) All input signals are specified with tR \= tF \= 3 ns (10% to 90% of AVDD) and timed from a voltage level of (VIL \+ VIH) / 2\. **7.7 Timing Diagrams** 

t9   
t1 

SCLK 1 16 

t8 

SYNC 

t4 

t5 

t3t2   
t7 

t10 t6 

DIN DB15 DB0 DB15 

**Figure 7-1. Serial Write Operation** 

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 7 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**7.8 Typical Characteristics: AVDD \= 5 V** 

at TA \= 25°C, AVDD \= 5 V, and DAC loaded with midscale code (unless otherwise noted) 

| 0.50  AV \= 5V DD  LE (LSB)  0.25  0  −0.25  −0.50  0.2  DLE (LSB)  0.1  0  −0.1  −0.2  0 512 1024 1536 2048  2560 3072 3584 4096  Digital Input Code  Figure 7-2. DAC7311 12-Bit Linearity Error and Differential  Linearity Error vs Code (–40°C) | 0.10  AVDD \= 5V  LE (LSB)  0.05  0  −0.05  −0.10  0.06  DLE (LSB)  0.03  0  −0.03  −0.06  0 128 256 384 512  640 768 896 1024  Digital Input Code  Figure 7-3. DAC6311 10-Bit Linearity Error and Differential  Linearity Error vs Code (–40°C) |
| ----- | ----- |
| 0.50  AV \= 5V DD  LE (LSB)  0.25  0  −0.25  −0.50  0.2  DLE (LSB)  0.1  0  −0.1  −0.2  0 512 1024 1536 2048  2560 3072 3584 4096  Digital Input Code  **Figure 7-4. DAC7311 12-Bit Linearity Error and Differential  Linearity Error vs Code (25°C)** | 0.10  AV \= 5V DD  LE (LSB)  0.05  0  −0.05  −0.10  0.06  DLE (LSB)  0.03  0  −0.03  −0.06  0 128 256 384 512  640 768 896 1024  Digital Input Code  **Figure 7-5. DAC6311 10-Bit Linearity Error and Differential  Linearity Error vs Code (25°C)** |
| 0.50  AV \= 5V DD  LE (LSB)  0.25  0  −0.25  −0.50  0.2  DLE (LSB)  0.1  0  −0.1  −0.2  0 512 1024 1536 2048  2560 3072 3584 4096  Digital Input Code  **Figure 7-6. DAC7311 12-Bit Linearity Error and Differential  Linearity Error vs Code (125°C)** | 0.10  AV \= 5V DD  LE (LSB)  0.05  0  −0.05  −0.10  0.06  DLE (LSB)  0.03  0  −0.03  −0.06  0 128 256 384 512  640 768 896 1024  Digital Input Code  **Figure 7-7. DAC6311 10-Bit Linearity Error and Differential  Linearity Error vs Code (125°C)** |

8 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.8 Typical Characteristics: AVDD \= 5 V (continued)**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at TA \= 25°C, AVDD \= 5 V, and DAC loaded with midscale code (unless otherwise noted) 

| 0.02  AV \= 5V DD  LE (LSB)  0.01  0  −0.01  −0.02  0.02  DLE (LSB)  0.01  0  −0.01  −0.02  0 32 64 96 128  160 192 224 256  Digital Input Code  Figure 7-8. DAC5311 8-Bit Linearity Error and Differential  Linearity Error vs Code (–40°C) | 0.4  AV \= 5V DD  Zero-Code Error (mV)  0.3  0.2  0.1  0  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  Figure 7-9. Zero-Code Error vs Temperature  |
| ----- | ----- |
| 0.02  AV \= 5V DD  LE (LSB)  0.01  0  −0.01  −0.02  0.02  DLE (LSB)  0.01  0  −0.01  −0.02  0 32 64 96 128  160 192 224 256  Digital Input Code  **Figure 7-10. DAC5311 8-Bit Linearity Error and Differential  Linearity Error vs Code (25°C)** | 0.6  AV \= 5V DD  0.4  Offset Error (mV)  0.2  0  −0.2  −0.4  −0.6  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-11. Offset Error vs Temperature**  |
| 0.02  AV \= 5V DD  LE (LSB)  0.01  0  −0.01  −0.02  0.02  DLE (LSB)  0.01  0  −0.01  −0.02  0 32 64 96 128  160 192 224 256  Digital Input Code  **Figure 7-12. DAC5311 8-Bit Linearity Error and Differential  Linearity Error vs Code (125°C)** | 0.06  AV \= 5V DD  0.04  Full-Scale Error (mV)  0.02  0  −0.02  −0.04  −0.06  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-13. Full-Scale Error vs Temperature**  |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 9 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**7.8 Typical Characteristics: AVDD \= 5 V (continued)** 

at TA \= 25°C, AVDD \= 5 V, and DAC loaded with midscale code (unless otherwise noted) 

| 5.5  5.0  Analog Output Voltage (V)  4.5  4.0  3.5  3.0  AV \= 5V DD  DAC Loaded with FFFh  2.5  0 2 4 6 8 10 I (mA) SOURCE  Figure 7-14. Source Current at Positive Rail | 0.6  AV \= 5V DD  DAC Loaded with 000h  Analog Output Voltage (V)  0.4  0.2  0  0 2 4 6 8 10 I (mA) SINK  Figure 7-15. Sink Current at Negative Rail |
| ----- | ----- |
| 120  AV \= 5.5V DD  Power-Supply Current ( A) μ  100  80  60  0 512 1024 1536 2048 2560 3072 3584 4096 Digital Input Code  **Figure 7-16. Power-Supply Current vs Digital Input Code**  | 2000  SYNC Input (all other digital inputs \= GND)  Power-Supply Current ( A) μ  1500  Sweep from  0V to 5.5V  1000  Sweep from  5.5V to 0V  500  0  0 0.5 1.0 1.5 2.0 2.5 3.0 3.5 4.0 4.5 5.0 V (V) LOGIC  **Figure 7-17. Power-Supply Current vs Logic Input Voltage**  |
| 140  AV \= 5V DD  Power-Supply Current ( A) μ  130  120  110  100  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-18. Power-Supply Current vs Temperature**  | 1.6  AVDD \= 5V  Quiescent Current ( A) μ  1.2  0.8  0.4  0  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-19. Power-Down Current vs Temperature**  |

10 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.8 Typical Characteristics: AVDD \= 5 V (continued)**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at TA \= 25°C, AVDD \= 5 V, and DAC loaded with midscale code (unless otherwise noted) 

| −20  AVDD S \= 5V, f \= 225kSPS,  −1dB FSR Digital Input,  Measurement Bandwidth \= 20kHz  −40  THD  THD (dB)  −60  2nd Harmonic  −80  3rd Harmonic  −100  0 1 2 3 4 5 f (kHz) OUT  Figure 7-20. Total Harmonic Distortion vs Output Frequency  | 102  AVDD S \= 5V, f \= 225kSPS,  −1dB FSR Digital Input,  Measurement Bandwidth \= 20kHz  94  SNR (dB)  86  78  70  0 1 2 3 4 5 f (kHz) OUT  Figure 7-21. Signal-to-Noise Ratio vs Output Frequency  |
| ----- | ----- |
| 0  AV \= 5V, DD  f \= 1kHz, f \= 225kSPS, OUT S  20  Measurement Bandwidth \= 20kHz  −40  Gain (dB)  −60  −80  −100  −120  −140  0 5 10 15 20 Frequency (kHz)  **Figure 7-22. Power Spectral Density** | 300  AV \= 5V DD  250   )  200  √Hz  Noise (nV/ 150  Midscale  100  Full Scale Zero Scale  50  0  10 100 1k 10k 100k Frequency (Hz)  **Figure 7-23. DAC Output Noise Density vs Frequency**  |
| AV \= 5V, DD  DAC \= Midscale, No Load   V/div) μ  VNOISE (1 3μVPP  Time (2s/div)  **Figure 7-24. DAC Output Noise, 0.1-Hz to 10-Hz Bandwidth** |  V/div) μ   (500 OUT  V AV \= 5V  DD  Clock Feedthrough Impulse \~0.5nV-s  Time (500ns/div)  **Figure 7-25. Clock Feedthrough, 5-V, 2-MHz, Midscale** |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 11 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**7.8 Typical Characteristics: AVDD \= 5 V (continued)** 

at TA \= 25°C, AVDD \= 5 V, and DAC loaded with midscale code (unless otherwise noted) 

| AV \= 5V  DD  From Code: 800h  To Code: 801h   V/div) μ   (500 OUT  Glitch Impulse  \< 0.5nV-s  V Clock  Feedthrough  \~0.5nV-s  Time (5 s/div) μ  Figure 7-26. Glitch Energy, 5-V, 12-Bit, 1-LSB Step, Rising Edge | AV \= 5V  DD  From Code: 801h   V/div) μ  To Code: 800h   (500 OUTClock  V Feedthrough  \~0.5nV-sGlitch Impulse  \< 0.5nV-s  Time (5 s/div) μ  Figure 7-27. Glitch Energy, 5-V, 12-Bit, 1-LSB Step, Falling Edge |
| ----- | ----- |
| Glitch Impulse  \~1nV-s   (5mV/div)  OUT  V AV \= 5V  Clock  DD  From Code: 80h  Feedthrough  To Code: 81h  \~0.5nV-s  Time (5 s/div) μ  **Figure 7-28. Glitch Energy, 5-V, 8-Bit, 1-LSB Step, Rising Edge** | AV \= 5V  DD  From Code: 81h   (5mV/div)  To Code: 80h  Glitch Impulse  OUTClock  \~1nV-s  V Feedthrough  \~0.5nV-s  Time (5 s/div) μ  **Figure 7-29. Glitch Energy, 5-V, 8-Bit, 1-LSB Step, Falling Edge** |
| AV \= 5V  DD  From Code: 000h  To Code: FFFh  Rising Edge  1V/div  Zoomed Rising Edge  100 V/div μ  Trigger Pulse 5V/div  Time (2 s/div) μ  **Figure 7-30. Full-Scale Settling Time, 5-V Rising Edge** | AV \= 5V  DD  From Code: FFFh  To Code: 000h  Falling Edge  Zoomed Falling Edge  1V/div  100 V/div μ  Trigger Pulse 5V/div  Time (2 s/div) μ  **Figure 7-31. Full-Scale Settling Time, 5-V Falling Edge** |

12 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.8 Typical Characteristics: AVDD \= 5 V (continued)**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at TA \= 25°C, AVDD \= 5 V, and DAC loaded with midscale code (unless otherwise noted) 

| Rising  Edge  Zoomed Rising Edge  1V/div  100 V/div μ  AV \= 5V  Trigger  DD  From Code: 400h  Pulse  To Code: C00h  5V/div  Time (2 s/div) μ  Figure 7-32. Half-Scale Settling Time, 5-V Rising Edge | AV \= 5V  Falling DD  From Code: C00h  Edge  To Code: 400h  1V/div  Zoomed Falling Edge  100 V/div μ  Trigger  Pulse  5V/div  Time (2 s/div) μ  Figure 7-33. Half-Scale Settling Time 5-V Falling Edge |
| ----- | ----- |
|  (2V/div)  DD  AV AV \= 5V  DD  DAC \= Zero Scale  Load \= 200pF || 10kΩ   (20mV/div)  17mV  OUT  V Time (5ms/div)  **Figure 7-34. Power-On Reset to 0-V Power-On Glitch** | AV \= 5V  DD   (2V/div)  DAC \= Zero Scale  Load \= 200pF || 10kΩ  DD  AV  (20mV/div)  OUT  V Time (10ms/div)  **Figure 7-35. Power-Off Glitch** |
| 120  AV \= 2.0V to 5.5V DD  Power-Supply Current ( A) μ  110  100  90  80  70  2.0 2.5 3.0 4.5 5.5  3.5 4.0 5.0  AV (V) DD  **Figure 7-36. Power-Supply Current vs Power-Supply Voltage**  | 0.4  AV \= 2.0V to 5.5V DD  Quiescent Current ( A) μ  0.3  0.2  0.1  0  2.0 2.5 3.5 4.5 5.5  3.0 4.0 5.0  AV (V) DD  **Figure 7-37. Power-Down Current vs Power-Supply Voltage**  |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 13 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**7.8 Typical Characteristics: AVDD \= 5 V (continued)** 

at TA \= 25°C, AVDD \= 5 V, and DAC loaded with midscale code (unless otherwise noted) 

| 50  AV \= 5.5V DD  45  40  35  Occurrences  30  25  20  15  10  5  0  80  84  88  92  96  100  104  108  112  116  120  124  128  132  136  140  I (μA) DD  Figure 7-38. Power-Supply Current Histogram |
| ----- |

14 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.9 Typical Characteristics: AVDD \= 3.6 V**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at TA \= 25°C, AVDD \= 3.6 V, and DAC loaded with midscale code (unless otherwise noted) 

| 1.2  AVDD \= 3.6V  Quiescent Current ( A) μ  0.8  0.4  0  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  Figure 7-39. Power-Down Current vs Temperature  | 1200  SYNC Input (all other digital inputs \= GND)  Power-Supply Current ( A) μ  900  Sweep from  0V to 3.6V  600  300  Sweep from  3.6V to 0V  0  0 0.5 1.0 1.5 2.0 2.5 3.0 3.5 4.0 V (V) LOGIC  Figure 7-40. Power-Supply Current vs Logic Input Voltage  |
| ----- | ----- |
| 50  AV \= 3.6V DD  45  40  35  Occurrences  30  25  20  15  10  5  0  70  74  78  82  86  90  94  98  102  106  110  114  118  122  126  130  I (μA) DD  **Figure 7-41. Power-Supply Current Histogram** | 3.7  3.5  Analog Output Voltage (V)  3.3  3.1  2.9  2.7  AV \= 3.6V DD  DAC Loaded with FFFFh  2.5  0 2 4 6 8 10 I (mA) SOURCE  **Figure 7-42. Source Current at Positive Rail** |
| 0.6  AV \= 3.6V DD  DAC Loaded with 0000h  Analog Output Voltage (V)  0.4  0.2  0  0 2 4 6 8 10  I (mA) SINK  **Figure 7-43. Sink Current at Negative Rail** |  |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 15 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**7.10 Typical Characteristics: AVDD \= 2.7 V** 

at TA \= 25°C, AVDD \= 2.7 V, and DAC loaded with midscale code (unless otherwise noted) 

| 0.50  AV \= 2.7V DD  LE (LSB)  0.25  0  −0.25  −0.50  0.2  DLE (LSB)  0.1  0  −0.1  −0.2  0 512 1024 1536 2048  2560 3072 3584 4096  Digital Input Code  Figure 7-44. DAC7311 12-Bit Linearity Error and Differential  Linearity Error vs Code (–40°C) | 0.10  AV \= 2.7V DD  LE (LSB)  0.05  0  −0.05  −0.10  0.06  DLE (LSB)  0.03  0  −0.03  −0.06  0 128 256 384 512  640 768 896 1024  Digital Input Code  Figure 7-45. DAC6311 10-Bit Linearity Error and Differential  Linearity Error vs Code (–40°C) |
| ----- | ----- |
| 0.50  AV \= 2.7V DD  LE (LSB)  0.25  0  −0.25  −0.50  0.2  DLE (LSB)  0.1  0  −0.1  −0.2  0 512 1024 1536 2048  2560 3072 3584 4096  Digital Input Code  **Figure 7-46. DAC7311 12-Bit Linearity Error and Differential  Linearity Error vs Code (25°C)** | 0.10  AV \= 2.7V DD  LE (LSB)  0.05  0  −0.05  −0.10  0.06  DLE (LSB)  0.03  0  −0.03  −0.06  0 128 256 384 512  640 768 896 1024  Digital Input Code  **Figure 7-47. DAC6311 10-Bit Linearity Error and Differential  Linearity Error vs Code (25°C)** |
| 0.50  AV \= 2.7V DD  LE (LSB)  0.25  0  −0.25  −0.50  0.2  DLE (LSB)  0.1  0  −0.1  −0.2  0 512 1024 1536 2048  2560 3072 3584 4096  Digital Input Code  **Figure 7-48. DAC7311 12-Bit Linearity Error and Differential  Linearity Error vs Code (125°C)** | 0.10  AV \= 2.7V DD  LE (LSB)  0.05  0  −0.05  −0.10  0.06  DLE (LSB)  0.03  0  −0.03  −0.06  0 128 256 384 512  640 768 896 1024  Digital Input Code  **Figure 7-49. DAC6311 10-Bit Linearity Error and Differential  Linearity Error vs Code (125°C)** |

16 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.10 Typical Characteristics: AVDD \= 2.7 V (continued)**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at TA \= 25°C, AVDD \= 2.7 V, and DAC loaded with midscale code (unless otherwise noted) 

| 0.02  AV \= 2.7V DD  LE (LSB)  0.01  0  −0.01  −0.02  0.02  DLE (LSB)  0.01  0  −0.01  −0.02  0 32 64 96 128  160 192 224 256  Digital Input Code  Figure 7-50. DAC5311 8-Bit Linearity Error and Differential  Linearity Error vs Code (–40°C) | 0.4  AV \= 2.7V DD  Zero-Code Error (mV)  0.3  0.2  0.1  0  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  Figure 7-51. Zero-Code Error vs Temperature  |
| ----- | ----- |
| 0.02  AV \= 2.7V DD  LE (LSB)  0.01  0  −0.01  −0.02  0.02  DLE (LSB)  0.01  0  −0.01  −0.02  0 32 64 96 128  160 192 224 256  Digital Input Code  **Figure 7-52. DAC5311 8-Bit Linearity Error and Differential  Linearity Error vs Code (25°C)** | 0.6  AV \= 2.7V DD  0.4  Offset Error (mV)  0.2  0  −0.2  −0.4  −0.6  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-53. Offset Error vs Temperature**  |
| 0.02  AV \= 2.7V DD  LE (LSB)  0.01  0  −0.01  −0.02  0.02  DLE (LSB)  0.01  0  −0.01  −0.02  0 32 64 96 128  160 192 224 256  Digital Input Code  **Figure 7-54. DAC5311 8-Bit Linearity Error and Differential  Linearity Error vs Code (125°C)** | 0.06  AV \= 2.7V DD  0.04  Full-Scale Error (mV)  0.02  0  −0.02  −0.04  −0.06  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-55. Full-Scale Error vs Temperature**  |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 17 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**7.10 Typical Characteristics: AVDD \= 2.7 V (continued)** 

at TA \= 25°C, AVDD \= 2.7 V, and DAC loaded with midscale code (unless otherwise noted) 

| 2.8  Analog Output Voltage (V)  2.6  2.4  2.2  AV \= 2.7V DD  DAC Loaded with FFFh  2.0  0 2 4 6 8 10 I (mA) SOURCE  Figure 7-56. Source Current at Positive Rail | 0.6  AV \= 2.7V DD  DAC Loaded with 000h  Analog Output Voltage (V)  0.4  0.2  0  0 2 4 6 8 10 I (mA) SINK  Figure 7-57. Sink Current at Negative Rail |
| ----- | ----- |
| 100  AV \= 2.7V DD  Power-Supply Current ( A) μ  90  80  70  60  50  0 512 1024 1536 2048 2560 3072 3584 4096 Digital Input Code  **Figure 7-58. Power-Supply Current vs Digital Input Code**  | 800  SYNC Input (all other digital inputs \= GND)  Power-Supply Current ( A) μ  600  Sweep from  0V to 2.7V  400  Sweep from  2.7V to 0V  200  0  0 0.5 1.5 2.5 1.0 2.0 3.0 V (V) LOGIC  **Figure 7-59. Power-Supply Current vs Logic Input Voltage**  |
| 120  AVDD \= 2.7V  Power-Supply Current ( A) μ  110  100  90  80  70  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-60. Power-Supply Current vs Temperature**  | 1.0  AVDD \= 2.7V  0.8  Quiescent Current ( A) μ  0.6  0.4  0.2  0  −40 −25 −10 5 20 35 50 65 80 95 110 125 Temperature ( C) °  **Figure 7-61. Power-Down Current vs Temperature**  |

18 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.10 Typical Characteristics: AVDD \= 2.7 V (continued)**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at TA \= 25°C, AVDD \= 2.7 V, and DAC loaded with midscale code (unless otherwise noted) 

| −20  AV \= 2.7V, f \= 225kSPS, DD S  −1dB FSR Digital Input,  Measurement Bandwidth \= 20kHz THD  −40  THD (dB)  −60  2nd Harmonic  −80  3rd Harmonic  −100  0 1 2 3 4 5 f (kHz) OUT  Figure 7-62. Total Harmonic Distortion vs Output Frequency  | 94  AVDD S \= 2.7V, f \= 225kSPS,  −1dB FSR Digital Input,  90  Measurement Bandwidth \= 20kHz  86  SNR (dB)  82  78  74  70  0 1 2 3 4 5 f (kHz) OUT  Figure 7-63. Signal-to-Noise Ratio vs Output Frequency  |
| ----- | ----- |
| 0  AV \= 2.7V, DD  AV \= 2.7V, DD  fOUT \= 1kHz, fS \= 225kSPS,  f \= 1kHz, f \= 225kSPS, OUT S  20  Measurement Bandwidth \= 20kHz  −40  Gain (dB)  −60  −80  −100  −120  −140  0 5 10 15 20 Frequency (kHz)  **Figure 7-64. Power Spectral Density** | 50  AV \= 2.7V DD  45  40  35  Occurrences  30  25  20  15  10  5  0  60  64  68  72  76  80  84  88  92  96  100  104  I (μA) DD  **Figure 7-65. Power-Supply Current Histogram** |
|  V/div) μ   (500 OUT  V AV \= 2.7V  DD  Clock Feedthrough Impulse \~0.4nV-s  Time (5 s/div) μ  **Figure 7-66. Clock Feedthrough 2.7-V, 20-MHz, Midscale** |  V/div) μ  Glitch Impulse  \< 0.3nV-s   (200 OUT  V AV \= 2.7V  DD  Clock  From Code: 800h  Feedthrough  To Code: 801h  \~0.4nV-s  Time (5 s/div) μ  **Figure 7-67. Glitch Energy, 2.7-V, 12-Bit, 1-LSB Step, Rising  Edge** |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 19 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**7.10 Typical Characteristics: AVDD \= 2.7 V (continued)** 

at TA \= 25°C, AVDD \= 2.7 V, and DAC loaded with midscale code (unless otherwise noted) 

| AV \= 2.7V  DD  From Code: 801h   V/div) μ  To Code: 800h   (200 OUTClock  V Feedthrough  \~0.4nV-s Glitch Impulse  \< 0.3nV-s  Time (5 s/div) μ  Figure 7-68. Glitch Energy, 2.7-V, 12-Bit, 1-LSB Step, Falling  Edge |  (2mV/div)  Glitch Impulse  \~1nV-s  OUTClock  Feedthrough  V \~0.4nV-s  AV \= 2.7V  DD  From Code: 80h  To Code: 81h  Time (5 s/div) μ  Figure 7-69. Glitch Energy, 2.7-V, 8-Bit, 1-LSB Step, Rising Edge |
| ----- | ----- |
| AV \= 2.7V  DD  From Code: 81h   (2mV/div)  To Code: 80h  OUTClock  V Feedthrough  Glitch Impulse  \~0.4nV-s  \~1nV-s  Time (5 s/div) μ  **Figure 7-70. Glitch Energy, 2.7-V, 8-Bit, 1-LSB Step, Falling Edge** | AV \= 2.7V  DD  From Code: 000h  To Code: FFFh  Rising Edge  1V/div  Zoomed Rising Edge  100 V/div μ  Trigger  Pulse  2.7V/div  Time (2 s/div) μ  **Figure 7-71. Full-Scale Settling Time, 2.7-V Rising Edge** |
| AV \= 2.7V  DD  From Code: FFFh  To Code: 000h  Falling Edge  1V/div  Zoomed Falling Edge  100 V/div μ  Trigger Pulse 2.7V/div  Time (2 s/div) μ  **Figure 7-72. Full-Scale Settling Time, 2.7-V Falling Edge** | AV \= 2.7V  DD  From Code: 400h  To Code: C00h  Rising  Edge  1V/div  Zoomed Rising Edge  100 V/div μ  Trigger  Pulse  2.7V/div  Time (2 s/div) μ  **Figure 7-73. Half-Scale Settling Time, 2.7-V Rising Edge** |

20 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**7.10 Typical Characteristics: AVDD \= 2.7 V (continued)**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

at TA \= 25°C, AVDD \= 2.7 V, and DAC loaded with midscale code (unless otherwise noted) 

| AV \= 2.7V  DD  From Code: C00h  To Code: 400h  Falling  Edge  1V/div  Trigger  Pulse  2.7V/div  Zoomed Falling Edge  100 V/div μ  Time (2 s/div) μ  Figure 7-74. Half-Scale Settling Time, 2.7-V Falling Edge |  (1V/div)  DD  AV AV \= 2.7V  DD  DAC \= Zero Scale  Load \= 200pF || 10kΩ   (20mV/div)  17mV  OUT  V Time (5ms/div)  Figure 7-75. Power-On Reset to 0-V Power-On Glitch |
| ----- | ----- |
| AV \= 2.7V  DD   (1V/div)  DAC \= Zero Scale  Load \= 200pF || 10kΩ  DD  AV  (20mV/div)  OUT  V Time (10ms/div)  **Figure 7-76. Power-Off Glitch** |  |

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 21 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com 8 Detailed Description**   
**8.1 Overview** 

The 8-bit DAC5311, 10-bit DAC6311, and 12-bit DAC7311 devices (DACx311) are low-power, single-channel,  voltage output DACs. These devices are monotonic by design, provide excellent linearity, and minimize  undesired code-to-code transient voltages while offering an easy upgrade path within a pin-compatible family. All  devices use a versatile, three-wire serial interface that operates at clock rates of up to 50 MHz and is compatible  with standard SPI, QSPI, Microwire, and digital signal processor (DSP) interfaces. 

**8.2 Functional Block Diagram** 

AVDD GND 

Power-On   
Reset 

DAC   
REF(+) 

Output   
Register 8-/10-/12-Bit DAC   
Buffer 

VOUT 

Input Control   
Logic 

SYNC SCLK DIN 

**8.3 Feature Description** 

**8.3.1 DAC Section**   
Power-Down   
Control Logic Resistor Network 

The DACx311 are fabricated using Texas Instruments' proprietary HPA07 process technology. The architecture  consists of a string DAC followed by an output buffer amplifier. Because there is no reference input pin, the  power supply (AVDD) acts as the reference. Figure 8-1 shows a block diagram of the DAC architecture. 

AVDD 

REF (+) 

DAC Register Resistor String GND   
VOUT 

Output   
Amplifier 

**Figure 8-1. DACx311 Architecture** 

The input coding to the DACx311 is straight binary, so the ideal output voltage is given by: 

D   
V AV2   
\= ×   
OUT DD n   
(1) 

where 

• n \= resolution in bits; either 8 (DAC5311), 10 (DAC6311), or 12 (DAC7311). 

• D \= decimal equivalent of the binary code that is loaded to the DAC register. D ranges from 0 to 255 for 8-bit  DAC5311, 0 to 1023 for the 10-bit DAC6311, and 0 to 4095 for the 12-bit DAC7311. 

22 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**8.3.2 Resistor String**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

Figure 8-2 shows the resistor string section, which is a string of resistors, each of value R. The code loaded  into the DAC register determines at which node on the string the voltage is tapped off to be fed into the output  amplifier by closing one of the switches connecting the string to the amplifier. The resistor string architecture is  inherently monotonic. 

VREF 

RDIVIDER 

V   
REF   
2 

R 

R 

R 

R 

To Output Amplifier 

**Figure 8-2. Resistor String** 

**8.3.3 Output Amplifier** 

The output buffer amplifier is capable of generating rail-to-rail voltages on the output, which gives an output  range of 0 V to AVDD. The output amplifier is capable of driving a load of 2 kΩ in parallel with 1000 pF to GND.  The source and sink capabilities of the output amplifier can be seen in the *Typical Characteristics* section for  the given voltage input. The slew rate is 0.7 V/μs with a half-scale settling time of typically 6 μs with the output  unloaded. 

**8.3.4 Power-On Reset** 

The DACx311 contain a power-on reset circuit that controls the output voltage during power up. On power up,  the DAC register is filled with zeros and the output voltage is 0 V. The DAC register remains that way until a valid  write sequence is made to the DAC. This design is useful in applications where knowing the state of the DAC  output while powering up is important. 

The occurring power-on glitch impulse is only a few millivolts (typically, 17 mV; see Figure 7-34). 

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 23 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com 8.4 Device Functional Modes** 

**8.4.1 Power-Down Modes** 

The DACx311 contain four separate modes of operation. These modes are programmable by setting two bits  (PD1 and PD0) in the control register. Table 8-1 shows how the state of the bits corresponds to the mode of  operation of the device. 

**Table 8-1. Modes of Operation for the DACx311** 

| PD1  | PD0  | OPERATING MODE |
| ----- | :---: | ----- |
| **NORMAL MODE** |  |  |
| 0  | 0  | Normal Operation |
| **POWER-DOWN MODES** |  |  |
| 0  | 1  | Output 1 kΩ to GND |
| 1  | 0  | Output 100 kΩ to GND |
| 1  | 1  | High-Z |

When both bits are set to 0, the device works normally with a standard power consumption of typically 80 μA at  2 V. However, for the three power-down modes, the typical supply current falls to 0.5 μA at 5 V, 0.4 μA at 3 V,  and 0.1 μA at 2 V. Not only does the supply current fall, but the output stage is also internally switched from the  output of the amplifier to a resistor network of known values. The advantage of this architecture is that the output  impedance of the part is known while the part is in power-down mode. There are three different options: the  output is connected internally to GND either through a 1-kΩ resistor or a 100-kΩ resistor, or is left open-circuited  (High-Z). Figure 8-3 illustrates the output stage. 

Amplifier 

Resistor   
String DAC 

Power-down   
Circuitry 

VOUT 

Resistor   
Network 

**Figure 8-3. Output Stage During Power-Down** 

All linear circuitry is shut down when the power-down mode is activated. However, the contents of the DAC  register are unaffected when in power-down. The time to exit power-down is typically 50 μs for AVDD \= 5 V and  AVDD \= 3 V. 

24 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**8.5 Programming 8.5.1 Serial Interface**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

The DACx311 has a 3-wire serial interface (SYNC, SCLK, and DIN) compatible with SPI, QSPI, and Microwire  interface standards, as well as most DSPs. For an example of a typical write sequence, see Figure 7-1. 

***8.5.1.1 Input Shift Register*** 

The input shift register is 16 bits wide, as shown in Table 8-2. The first two bits (PD0 and PD1) are reserved  control bits that set the desired mode of operation (normal mode or any one of three power-down modes) as  indicated in Table 8-1. 

The remaining data bits are either 12 (DAC7311), 10 (DAC6311), or 8 (DAC5311) data bits, followed by *don't  care* bits, as shown in Table 8-2, Table 8-3, and Table 8-4, respectively. 

**Table 8-2. DAC5311 8-Bit Data Input Register** 

| DB15 DB14 DB6 DB5 DB0 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| PD1  | PD0  | D7  | D6  | D5  | D4  | D3  | D2  | D1  | D0  | X  | X  | X  | X  | X  | X |

LEGEND: R/W \= Read/Write; R \= Read only; \-n \= value after reset 

**Table 8-3. DAC6311 10-Bit Data Input Register** 

| DB15 DB14 DB4 DB3 DB0 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| PD1  | PD0  | D9  | D8  | D7  | D6  | D5  | D4  | D3  | D2  | D1  | D0  | X  | X  | X  | X |

LEGEND: R/W \= Read/Write; R \= Read only; \-n \= value after reset 

**Table 8-4. DAC7311 12-Bit Data Input Register** 

| DB15 DB14 DB2 DB1 DB0 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| PD1  | PD0  | D11  | D10  | D9  | D8  | D7  | D6  | D5  | D4  | D3  | D2  | D1  | D0  | X  | X |

LEGEND: R/W \= Read/Write; R \= Read only; \-n \= value after reset 

The write sequence begins by bringing the SYNC line low. Data from the DIN line are clocked into the 16-bit shift  register on each falling edge of SCLK. The serial clock frequency can be as high as 50 MHz, making 

the DACx311 compatible with high-speed DSPs. On the 16th falling edge of the serial clock, the last data bit is  clocked in and the programmed function is executed. 

At this point, the SYNC line can be kept low or brought high. In either case, SYNC must be brought high for  a minimum of 20 ns before the next write sequence so that a falling edge of SYNC can initiate the next write  sequence. 

***8.5.1.2 SYNC Interrupt*** 

In a normal write sequence, the SYNC line is kept low for at least 16 falling edges of SCLK and the DAC is  updated on the 16th falling edge. However, bringing SYNC high before the 16th falling edge acts as an interrupt  to the write sequence. The shift register is reset and the write sequence is seen as invalid. Neither an update of  the DAC register contents nor a change in the operating mode occurs, as shown in Figure 8-4. 

CLK 

SYNC 

DIN DB15 DB0 DB15 DB0 SYNC HIGH before 16th Falling EdgeValid Write Sequence:   
Invalid Write Sequence: 

Output Updates on 16th Falling Edge 

**Figure 8-4. DACx311 SYNC Interrupt Facility** 

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 25 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com 9 Application and Implementation**   
**Note** 

Information in the following applications sections is not part of the TI component specification,  and TI does not warrant its accuracy or completeness. TI’s customers are responsible for  determining suitability of components for their purposes, as well as validating and testing their design  implementation to confirm system functionality. 

**9.1 Application Information** 

**9.1.1 Microprocessor Interfacing** 

***9.1.1.1 DACx311 to 8051 Interface*** 

Figure 9-1 shows a serial interface between the DACx311 and a typical 8051-type microcontroller. The setup for  the interface is as follows: TXD of the 8051 drives SCLK of the DACx311, while RXD drives the serial data line  of the device. The SYNC signal is derived from a bit programmable pin on the port. In this case, port line P3.3 is  used. When data are to be transmitted to the DACx311, P3.3 is taken low. The 8051 transmits data only in 8-bit  bytes; thus, only eight falling clock edges occur in the transmit cycle. To load data to the DAC, P3.3 remains low  after the first eight bits are transmitted, and a second write cycle is initiated to transmit the second byte of data.  P3.3 is taken high following the completion of this cycle. The 8051 outputs the serial data in a format that has the  LSB first. The DACx311 requires data with the MSB as the first bit received. Therefore, the 8051 transmit routine  must take this requirement into account, and *mirror* the data as needed. 

80C51/80L51(1) 

P3.3 

TXD 

RXD 

NOTE: (1) Additional pins omitted for clarity. 

**DACx311**(1) SYNC 

SCLK 

DIN 

**Figure 9-1. DACx311 to 80C51/80l51 Interfaces** 

***9.1.1.2 DACx311 to Microwire Interface*** 

Figure 9-2 shows an interface between the DACx311 and any Microwire-compatible device. Serial data (SO) are  shifted out on the falling edge of the serial clock (SK) and are clocked into the DACx311 on the rising edge of the  SK signal. 

Microwire 

CS 

SK 

SO 

NOTE: (1) Additional pins omitted for clarity.   
**DACx311**(1) SYNC 

SCLK 

DIN 

**Figure 9-2. DACx311 to Microwire Interface** 

26 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

***9.1.1.3 DACx311 to 68HC11 Interface***   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

Figure 9-3 shows a serial interface between the DACx311 and the 68HC11 microcontroller. SCK of the 68HC11  drives the SCLK of the DACx311, while the MOSI output drives the serial data line of the DAC. The SYNC signal  is derived from a port line (PC7), similar to what was done for the 8051\. 

68HC11(1) 

PC7 

SCK 

MOSI 

NOTE: (1) Additional pins omitted for clarity.   
**DACx311**(1) SYNC 

SCLK 

DIN 

**Figure 9-3. DACx311 to 68HC11 Interface** 

Configure the 68HC11 so that the CPOL bit is 0 and the CPHA bit is 1\. This configuration causes data appearing  on the MOSI output to be valid on the falling edge of SCK. When data are being transmitted to the DAC, the  SYNC line is taken low (PC7). Serial data from the 68HC11 are transmitted in 8-bit bytes with only eight falling  clock edges occurring in the transmit cycle. Data are transmitted MSB first. To load data to the DACx311, PC7  is held low after the first eight bits are transferred, and a second serial write operation is performed to the DAC;  PC7 is taken high at the end of this procedure. 

**9.2 Typical Applications** 

**9.2.1 Loop Powered Transmitter** 

The described loop powered transmitter can accurately source currents from 4 mA to 20 mA. 

VREG 

Regulator V+ 

V /V REG REF 

R5 

R2   
122.15 kΩ \+   
Q1 30.542 kΩ 

U1 

OPA317 

R6 

4.32 kΩ 26.7 Ω   
R3 R4 

**Figure 9-4. Loop Powered Transmitter Schematic** 

***9.2.1.1 Design Requirements*** 

60.4 Ω 

Return 

The transmitter has only two external input pins; a supply connection and a ground (or return) connection. The  transmitter communicates back to the host, typically a PLC analog input module, by precisely controlling the  magnitude of the return current. To conform to the 4-mA to 20-mA communication standards, the complete  transmitter must consume less than 4 mA of current. 

The complete design of this circuit is outlined in TIPD158, *Low Cost Loop-Powered 4-20mA Transmitter  EMC/EMI Tested Reference Design*. The design is expected to be low-cost and deliver immunity to the  IEC61000-4 suite of tests with minimum impact on the accuracy of the system. Reference design TIPD158  includes the design goals, simulated results, and measured performance. 

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 27 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

***9.2.1.2 Detailed Design Procedure*** 

Amplifier U1 uses negative feedback to make sure that the potentials at the inverting (V–) and noninverting (V+)  input terminals are equal. In this configuration, V– is directly tied to the local GND; therefore, the potential at the  noninverting input terminal is driven to local ground. Thus, the voltage difference across R2 is the DAC output  voltage (VOUT), and the voltage difference across R5 is the regulator voltage (VREG). These voltage differences  cause currents to flow through R2 and R5, as illustrated in Figure 9-5. 

VREG 

Regulator V+ 

V /R REG 2 R5   
V /V REG REF DAC 

R2   
VOUT 

V\+ 

\+   
U1 

Q1   
V /R DAC 1 0 A 

V–*iloop*   
R6 

*iq* 

*i*1   
*i* R 2 3 R4 

*iout* 

Return 

**Figure 9-5. Voltage to Current Conversion** 

The currents from R2 and R5 sum into i1 (defined in Equation 2), and i1 flows through R3. V V iR R   
DAC REG   
1   
     
2 5 

(2) 

Amplifier U2 drives the base of Q1, the NPN bipolar junction transistor (BJT), to allow current to flow through  R4 so that the voltage drops across R3 and R4 remain equal. This design keeps the inverting and noninverting  terminals at the same potential. A small part of the current through R4 is sourced by the quiescent current of  all of the components used in the transmitter design (regulator, amplifier, and DAC). The voltage drops across  R3 and R4 are equal; therefore, different-sized resistors cause different current flow through each resistor. Use  these different-sized resistors to apply gain to the current flow through R4 by controlling the ratio of resistor R3 to  R4, as shown in Equation 3: 

V i Ri R 9± L 5 LR   
  òò   
1 3 

 ò ü    
2 4 2 

9 9± 

1 3 4   
  (3) 

The current gain in the circuit helps allow a majority of the output current to come directly from the loop through  Q1 instead of from the voltage-to-current converter. This current gain, in addition to the low-power components,  keeps the current consumption of the voltage-to-current converter low. Currents i1 and i2 sum to form output  current iout, as shown in Equation 4: 

ß · ß · ß ·   
V V R V V V V R i i i 1   
DAC REG 3 DAC REG DAC REG 3   
       ò     ò   ® ¸ ® ¸ ® ¸ © ¹ © ¹ © ¹(4)   
out 1 2   
R R R R R R R R 2 5 4 2 5 2 5 4 

The complete transfer function, arranged as a function of input code, is shown in Equation 5. The remaining  sections divide this circuit into blocks for simplified discussion. 

28 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

ß · ò ß ·   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

V Code V R i Code 1 

   REG REG 3   
   ò   ® ¸ ® ¸   
out Resolution2 5 4   
2 R R R   
© ¹ ò © ¹(5) 

Resistor R6 is included to reduce the gain of transistor Q1, and therefore, reduce the closed-loop gain of the  voltage-to-current converter for a stable design. Size resistors R2, R3, R4, and R5 based on the full-scale range  of the DAC, regulator voltage, and the desired current output range of the design. 

***9.2.1.3 Application Curves*** 

Figure 9-6 shows the measured transfer function of the circuit. Figure 9-7 shows the total unadjusted error (TUE)  of the circuit, staying below 0.15 %FSR. 

| 20  16  Output Current (mA)  12  8  4  0 1024 2048 3072 4096 DAC Code  Figure 9-6. Output Current vs Code | 0.20  0.15  Output Current TUE (%FSR)  0.10  0.05  0.00  \-0.05  \-0.10  \-0.15  \-0.20  0 1024 2048 3072 4096 DAC Code  Figure 9-7. Current Total Unadjusted Error vs Code |
| ----- | ----- |

**9.2.2 Using the REF5050 as a Power Supply for the DACx311** 

As a result of the extremely low supply current required by the DACx311, an alternative option is to use a  REF5050 5-V precision voltage reference to supply the required voltage to the part, as shown in Figure 9-8. This  option is especially useful if the power supply is too noisy or if the system supply voltages are at some value  other than 5 V. The REF5050 outputs a steady supply voltage for the DACx311. If the REF5050 is used, the  current needed to supply DACx311 is typically 110 μA at 5 V, with no load on the output of the DAC. When the  DAC output is loaded, the REF5050 also needs to supply the current to the load. The total current required (with  a 5 kΩ load on the DAC output) is: 

110 μA \+ (5 V / 5 kΩ) \= 1.11 mA (6) 

The load regulation of the REF5050 is typically 0.002%/mA, which results in an error of 90 μV for the 1.1 mA  current drawn from the device. This value corresponds to a 0.07 LSB error at 12 bits (DAC7311). 

\+5.5V 

**REF5050**   
\+5V 

110 Aμ   
1 Fμ 

Three-Wire   
VOUT \= 0V to 5V SYNC   
Serial   
Interface   
SCLK DIN 

**DACx311** 

**Figure 9-8. REF5050 as Power Supply to DACx311** 

For other power-supply voltages, alternative references such as the REF3030 (3 V), REF3033 (3.3 V), or  REF3220 (2.048 V) are recommended. For a full list of available voltage references from TI, see the TI web site  at www.ti.com. 

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 29 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**9.2.3 Bipolar Operation Using the DACx311** 

The DACx311 has been designed for single-supply operation but a bipolar output range is also possible using  the circuit in Figure 9-9. The circuit shown gives an output voltage range of ±5 V. Rail-to-rail operation at the  amplifier output is achievable using an OPA211, OPA340, or OPA703 as the output amplifier. For a full list of  available operational amplifiers from TI, see the TI web site at www.ti.com 

The output voltage for any input code can be calculated as follows: 

⎡ ⎛ ⎞ ⎛ ⎞ \+ ⎤ ⎛ ⎞   
D R R R V AV AV   
1 2 2   
\= × × − × ⎢ ⎜ ⎟ ⎜ ⎟ ⎜ ⎟⎥   
O DD DD n1 1   
2 R R   
⎢⎣⎝ ⎠ ⎝ ⎠ ⎝ ⎠⎥⎦(7) 

where 

• n \= resolution in bits; either 8 (DAC5311), 10 (DAC6311), or 12 (DAC7311). 

• D \= decimal equivalent of the binary code that is loaded to the DAC register. D ranges from 0 to 255 for 8-bit  DAC5311, 0 to 1023 for the 10-bit DAC6311 and 0 to 4095 for the 12-bit DAC7311. 

With AVDD \= 5 V, R1 \= R2 \= 10 kΩ: 

2n  5V(8)   
VO    10   D 

The resulting output voltage range is ±5 V. Code 000h corresponds to a –5-V output and FFFh (12-bit level)  corresponding to a \+5-V output. 

R   
\+5V 2   
10kΩ 

10 Fμ 0.1 Fμ 

R   
10kΩ1 

AVDD **DACx311** 

Three-Wire   
Serial   
Interface 

VOUT   
\+5.5V 

**OPA211** 5.5V 

±5V 

**Figure 9-9. Bipolar Operation With the DACx311** 

**9.3 Power Supply Recommendations** 

The DACx311 is designed to operate with a unipolar analog power supply ranging from 2.0 V to 5.5 V on the  AVDD pin. The AVDD pin supplies power to the digital and analog circuits (including the resistor string) inside the  DAC. The current consumption of this pin is specified in the *Electrical Characteristics* table. Use a 1 μF to 10 μF  capacitor in parallel with a 0.1 μF bypass capacitor on this pin to remove high-frequency noise. 

30 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*

**www.ti.com** 

**9.4 Layout** 

**9.4.1 Layout Guidelines**   
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 

A precision analog component requires careful layout, adequate bypassing, and clean, well-regulated power  supplies. 

The DACx311 offers single-supply operation, and is often used in close proximity to digital logic,  microcontrollers, microprocessors, and digital signal processors. The more digital logic present in the design and  the higher the switching speed, the more difficult the task is to achieve good performance from the converter. 

As a result of the single ground pin of the DACx311, all return currents, including digital and analog return  currents, must flow through the GND pin. Ideally, GND is connected directly to an analog ground plane. Separate  this plane from the ground connection for the digital components until connected at the power entry point of the  system. 

The power applied to AVDD must be well-regulated and low-noise. Switching power supplies and dc/dc  converters often have high-frequency glitches or spikes riding on the output voltage. In addition, digital  components can create similar high-frequency spikes as the internal logic switches state. This noise can easily  couple into the DAC output voltage through various paths between the power connections and analog output.  This condition is particularly true for the DACx311, as the power supply is also the reference voltage for the  DAC. 

As with the GND connection, connect AVDD to a 5‑V power supply plane or trace that is separate from the  connection for digital logic until connected at the power entry point. In addition, 1-μF to 10-μF and 0.1-μF bypass  capacitors are strongly recommended. In some situations, additional bypassing can be required, such as a 100  μF electrolytic capacitor or even a *Pi* filter made up of inductors and capacitors—all designed to essentially  low-pass filter the 5-V supply and remove high-frequency noise. 

**9.4.2 Layout Example** 

**U1 Analog IO** 

**Digital IO**   
**Bypass**  

**Capacitors** 

**Figure 9-10. Recommended Layout** 

Copyright © 2023 Texas Instruments Incorporated *Submit Document Feedback* 31 Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**DAC5311, DAC6311, DAC7311**   
SBAS442D – AUGUST 2008 – REVISED AUGUST 2023 **www.ti.com** 

**10 Device and Documentation Support** 

**10.1 Receiving Notification of Documentation Updates** 

To receive notification of documentation updates, navigate to the device product folder on ti.com. Click on  *Subscribe to updates* to register and receive a weekly digest of any product information that has changed. For  change details, review the revision history included in any revised document. 

**10.2 Support Resources** 

TI E2E™ support forums are an engineer's go-to source for fast, verified answers and design help — straight  from the experts. Search existing answers or ask your own question to get the quick design help you need. 

Linked content is provided "AS IS" by the respective contributors. They do not constitute TI specifications and do  not necessarily reflect TI's views; see TI's Terms of Use. 

**10.3 Trademarks** 

TI E2E™ is a trademark of Texas Instruments. 

All trademarks are the property of their respective owners. 

**10.4 Electrostatic Discharge Caution** 

This integrated circuit can be damaged by ESD. Texas Instruments recommends that all integrated circuits be handled  with appropriate precautions. Failure to observe proper handling and installation procedures can cause damage. 

ESD damage can range from subtle performance degradation to complete device failure. Precision integrated circuits may  be more susceptible to damage because very small parametric changes could cause the device not to meet its published  specifications. 

**10.5 Glossary** 

TI Glossary This glossary lists and explains terms, acronyms, and definitions. 

**11 Mechanical, Packaging, and Orderable Information** 

The following pages include mechanical, packaging, and orderable information. This information is the most  current data available for the designated devices. This data is subject to change without notice and revision of  this document. For browser-based versions of this data sheet, refer to the left-hand navigation. 

32 *Submit Document Feedback* Copyright © 2023 Texas Instruments Incorporated Product Folder Links: *DAC5311 DAC6311 DAC7311*  
**PACKAGE OPTION ADDENDUM** 

www.ti.com 9-Nov-2025 

**PACKAGING INFORMATION** 

| Orderable part number Status  Material type  Package | Pins Package qty | Carrier RoHS  Lead finish/  MSL rating/  Op temp (°C) Part marking  Ball material  Peak reflow  (1)  (2)  (3)  (6) (4)  (5)  |
| ----- |
| DAC5311IDCKR Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D53 |
| DAC5311IDCKR.A Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D53 |
| DAC5311IDCKRG4 Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D53 |
| DAC5311IDCKRG4.A Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D53 |
| DAC5311IDCKT Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D53 |
| DAC5311IDCKT.A Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D53 |
| DAC6311IDCKR Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D63 |
| DAC6311IDCKR.A Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D63 |
| DAC6311IDCKRG4 Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D63 |
| DAC6311IDCKRG4.A Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D63 |
| DAC6311IDCKT Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D63 |
| DAC6311IDCKT.A Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D63 |
| DAC6311IDCKTG4 Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D63 |
| DAC7311IDCKR Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D73 |
| DAC7311IDCKR.A Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D73 |
| DAC7311IDCKRG4 Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D73 |
| DAC7311IDCKRG4.A Active Production SC70 (DCK) | 6 3000 | LARGE T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D73 |
| DAC7311IDCKT Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D73 |
| DAC7311IDCKT.A Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D73 |
| DAC7311IDCKTG4 Active Production SC70 (DCK) | 6 250 | SMALL T\&R Yes NIPDAU Level-1-260C-UNLIM \-40 to 125 D73 |

**(1) Status:** For more details on status, see our product life cycle. 

**(2) Material type:** When designated, preproduction parts are prototypes/experimental devices, and are not yet approved or released for full production. Testing and final process, including without limitation quality assurance, reliability performance testing, and/or process qualification, may not yet be complete, and this item is subject to further changes or possible discontinuation. If available for ordering, purchases will be subject to an additional waiver at checkout, and are intended for early internal evaluation purposes only. These items are sold without warranties of any kind. 

**(3) RoHS values:** Yes, No, RoHS Exempt. See the TI RoHS Statement for additional information and value definition. 

Addendum-Page 1  
**PACKAGE OPTION ADDENDUM** 

www.ti.com 9-Nov-2025 

**(4) Lead finish/Ball material:** Parts may have multiple material finish options. Finish options are separated by a vertical ruled line. Lead finish/Ball material values may wrap to two lines if the finish value exceeds the maximum column width. 

**(5) MSL rating/Peak reflow:** The moisture sensitivity level ratings and peak solder (reflow) temperatures. In the event that a part has multiple moisture sensitivity ratings, only the lowest level per JEDEC standards is shown. Refer to the shipping label for the actual reflow temperature that will be used to mount the part to the printed circuit board. 

**(6) Part marking:** There may be an additional marking, which relates to the logo, the lot trace code information, or the environmental category of the part. 

Multiple part markings will be inside parentheses. Only one part marking contained in parentheses and separated by a "\~" will appear on a part. If a line is indented then it is a continuation of the previous line and the two combined represent the entire part marking for that device. 

**Important Information and Disclaimer:**The information provided on this page represents TI's knowledge and belief as of the date that it is provided. TI bases its knowledge and belief on information provided by third parties, and makes no representation or warranty as to the accuracy of such information. Efforts are underway to better integrate information from third parties. TI has taken and continues to take reasonable steps to provide representative and accurate information but may not have conducted destructive testing or chemical analysis on incoming materials and chemicals. TI and TI suppliers consider certain information to be proprietary, and thus CAS numbers and other limited information may not be available for release. 

In no event shall TI's liability arising out of such information exceed the total purchase price of the TI part(s) at issue in this document sold by TI to Customer on an annual basis. 

**OTHER QUALIFIED VERSIONS OF DAC5311 :** 

•Automotive : DAC5311-Q1 

NOTE: Qualified Version Definitions: 

•Automotive \- Q100 devices qualified for high-reliability automotive applications targeting zero defects 

Addendum-Page 2  
**PACKAGE MATERIALS INFORMATION** 

www.ti.com 18-Jun-2025 **TAPE AND REEL INFORMATION** 

**REEL DIMENSIONS** 

|  |
| :---- |

Reel   
Diameter   
**TAPE DIMENSIONS** 

K0 P1 

B0 W 

Cavity A0 

|  |  |
| :---- | :---- |

\*All dimensions are nominal   
A0   
Dimension designed to accommodate the component width   
B0 

Dimension designed to accommodate the component length   
Dimension designed to accommodate the component thickness   
K0 

Overall width of the carrier tape   
W   
P1 

Pitch between successive cavity centers 

Reel Width (W1) 

**QUADRANT ASSIGNMENTS FOR PIN 1 ORIENTATION IN TAPE** Sprocket Holes 

Q1 Q2 Q1 Q2 

Q3 Q4 Q3 Q4 User Direction of Feed 

Pocket Quadrants 

| Device  | Package Type | Package Drawing | Pins  | SPQ  | Reel  Diameter (mm) | Reel  Width  W1 (mm) | A0  (mm) | B0  (mm) | K0  (mm) | P1  (mm) | W  (mm) | Pin1  Quadrant |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| DAC5311IDCKR  | SC70  | DCK  | 6  | 3000  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC5311IDCKRG4  | SC70  | DCK  | 6  | 3000  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC5311IDCKT  | SC70  | DCK  | 6  | 250  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC6311IDCKR  | SC70  | DCK  | 6  | 3000  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC6311IDCKRG4  | SC70  | DCK  | 6  | 3000  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC6311IDCKT  | SC70  | DCK  | 6  | 250  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC7311IDCKR  | SC70  | DCK  | 6  | 3000  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC7311IDCKRG4  | SC70  | DCK  | 6  | 3000  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |
| DAC7311IDCKT  | SC70  | DCK  | 6  | 250  | 178.0  | 9.0  | 2.4  | 2.5  | 1.2  | 4.0  | 8.0  | Q3 |

Pack Materials-Page 1  
**PACKAGE MATERIALS INFORMATION** 

www.ti.com 18-Jun-2025 **TAPE AND REEL BOX DIMENSIONS** 

***Width (mm)***   
H 

W 

\*All dimensions are nominal 

L 

| Device  | Package Type  | Package Drawing  | Pins  | SPQ  | Length (mm)  | Width (mm)  | Height (mm) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| DAC5311IDCKR  | SC70  | DCK  | 6  | 3000  | 180.0  | 180.0  | 18.0 |
| DAC5311IDCKRG4  | SC70  | DCK  | 6  | 3000  | 180.0  | 180.0  | 18.0 |
| DAC5311IDCKT  | SC70  | DCK  | 6  | 250  | 180.0  | 180.0  | 18.0 |
| DAC6311IDCKR  | SC70  | DCK  | 6  | 3000  | 180.0  | 180.0  | 18.0 |
| DAC6311IDCKRG4  | SC70  | DCK  | 6  | 3000  | 180.0  | 180.0  | 18.0 |
| DAC6311IDCKT  | SC70  | DCK  | 6  | 250  | 180.0  | 180.0  | 18.0 |
| DAC7311IDCKR  | SC70  | DCK  | 6  | 3000  | 180.0  | 180.0  | 18.0 |
| DAC7311IDCKRG4  | SC70  | DCK  | 6  | 3000  | 180.0  | 180.0  | 18.0 |
| DAC7311IDCKT  | SC70  | DCK  | 6  | 250  | 180.0  | 180.0  | 18.0 |

Pack Materials-Page 2

**PACKAGE OUTLINE** 

**DCK0006A SOT \- 1.1 max height** 

SCALE 5.600

SMALL OUTLINE TRANSISTOR 

| C |
| :---: |

2.4 

PIN 1   
INDEX AREA 

1 

| 4X 0.65 |
| :---- |

| 1.3 |
| :---: |

2 

6X 0.30   
0.15 

|  |  | 0.1  |  | C A B |  |
| :---- | :---- | :---- | :---- | :---: | :---- |

NOTE 5  
NOTE 5  
1.8 

1.4   
1.1 

|  |  |
| :---- | :---- |
|  |  |
|  |  |

|  |
| :---- |
|  |

3

| B  |
| :---: |

|  |
| :---- |

|  |
| :---- |

|  |
| :---- |

6 4 

| A |
| :---: |

2.15   
1.85 

4X 0 \-12 

|  | 0.1 |  C |
| :---- | :---: | :---: |

1.1   
0.8 

|  |  |  |
| :---- | :---- | :---- |
|  |  |  |
|  |  |  |
|  |  |  |
|  |  |  |
|  |  |  |
|  |  |  |

0.1   
0.0 TYP 

| 0.15 |
| :---: |

GAGE PLANE 

8   
0 TYP 

0.46   
4X 4 \-15 

0.22   
0.08 TYP 

0.26 TYP 

4214835/D 11/2024 

NOTES:    
~~S~~EATING PLANE 

1\. All linear dimensions are in millimeters. Any dimensions in parenthesis are for reference only. Dimensioning and tolerancing  per ASME Y14.5M.   
2\. This drawing is subject to change without notice.   
3\. Body dimensions do not include mold flash or protrusion. Mold flash and protrusion shall not exceed 0.15 per side. 4\. Falls within JEDEC MO-203 variation AB. 

|  |  |  |
| :---- | :---- | :---- |
|  |  |  |

|  |  |
| :---- | :---- |
|  |  |

|  |  |  |  |  |  |  |  |  |  |  |
| :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- |
|  |  |  |  |  |  |  |  |  |  |  |

www.ti.com   
**EXAMPLE BOARD LAYOUT** 

**DCK0006A SOT \- 1.1 max height** SMALL OUTLINE TRANSISTOR 

PKG 

1   
6X (0.9) 

6X (0.4) 

4X (0.65) 

|  |
| :---- |

2 

6 

SYMM 

(R0.05) TYP   
3 4 (2.2) 

LAND PATTERN EXAMPLE   
EXPOSED METAL SHOWN 

SCALE:18X 

METAL SOLDER MASK OPENING 

EXPOSED METAL

0.07 MAX   
ARROUND 

NON SOLDER MASK   
DEFINED   
(PREFERRED)   
SOLDER MASK   
OPENING METAL UNDER SOLDER MASK 

EXPOSED METAL 

0.07 MIN   
ARROUND 

SOLDER MASK   
DEFINED 

NOTES: (continued) 

5\. Publication IPC-7351 may have alternate designs.    
SOLDER MASK DETAILS 

4214835/D 11/2024 

6\. Solder mask tolerances between and around signal pads can vary based on board fabrication site. 

|  |  |  |
| :---- | :---- | :---- |
|  |  |  |
|  |  |  |

|  |  |  |  |
| :---- | :---- | :---- | :---- |

www.ti.com   
**EXAMPLE STENCIL DESIGN** 

**DCK0006A SOT \- 1.1 max height** SMALL OUTLINE TRANSISTOR 

PKG 

1   
6X (0.9) 

6X (0.4) 

4X(0.65) 

|  |
| :---- |

2

6 

SYMM 

(R0.05) TYP 

4214835/D 11/2024 

NOTES: (continued)   
3 4 (2.2) 

SOLDER PASTE EXAMPLE   
BASED ON 0.125 THICK STENCIL 

SCALE:18X 

7\. Laser cutting apertures with trapezoidal walls and rounded corners may offer better paste release. IPC-7525 may have alternate  design recommendations.    
8\. Board assembly site may have different recommendations for stencil design. 

|  |  |  |
| :---- | :---- | :---- |
|  |  |  |

|  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- | :---- |
|  |  |  |  |  |  |  |  |  |  |  |  |  |  |

www.ti.com   
**IMPORTANT NOTICE AND DISCLAIMER** 

TI PROVIDES TECHNICAL AND RELIABILITY DATA (INCLUDING DATASHEETS), DESIGN RESOURCES (INCLUDING REFERENCE  DESIGNS), APPLICATION OR OTHER DESIGN ADVICE, WEB TOOLS, SAFETY INFORMATION, AND OTHER RESOURCES “AS IS”  AND WITH ALL FAULTS, AND DISCLAIMS ALL WARRANTIES, EXPRESS AND IMPLIED, INCLUDING WITHOUT LIMITATION ANY  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT OF THIRD  PARTY INTELLECTUAL PROPERTY RIGHTS. 

These resources are intended for skilled developers designing with TI products. You are solely responsible for (1) selecting the appropriate  TI products for your application, (2) designing, validating and testing your application, and (3) ensuring your application meets applicable  standards, and any other safety, security, regulatory or other requirements. 

These resources are subject to change without notice. TI grants you permission to use these resources only for development of an  application that uses the TI products described in the resource. Other reproduction and display of these resources is prohibited. No license  is granted to any other TI intellectual property right or to any third party intellectual property right. TI disclaims responsibility for, and you fully  indemnify TI and its representatives against any claims, damages, costs, losses, and liabilities arising out of your use of these resources. 

TI’s products are provided subject to TI’s Terms of Sale, TI’s General Quality Guidelines, or other applicable terms available either on  ti.com or provided in conjunction with such TI products. TI’s provision of these resources does not expand or otherwise alter TI’s applicable  warranties or warranty disclaimers for TI products. Unless TI explicitly designates a product as custom or customer-specified, TI products  are standard, catalog, general purpose devices. 

TI objects to and rejects any additional or different terms you may propose. 

IMPORTANT NOTICE 

Copyright © 2025, Texas Instruments Incorporated 

Last updated 10/2025