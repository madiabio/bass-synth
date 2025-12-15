# Monophonic Digital Bass Synthesizer

## Overview
This project implements a monophonic digital bass synthesizer on the TI TM4C129 series microcontroller. It combines real-time waveform generation, an ADS (attack/decay/sustain) envelope, and an SPI-driven TFT display to create an embedded bass instrument tailored for an Embedded Systems (student-defined) project by me. The firmware drives an external UDA1334A I2S DAC for audio output, samples three analog potentiometers for envelope control, and accepts note input from a 4x3 matrix keypad.

The codebase is organised into small modules responsible for waveform generation, hardware interfaces (I2S, SPI, DMA, ADC, GPIO), and user interaction. DMA-powered ping-pong buffering keeps the audio pipeline running while the CPU services input scanning and display updates.

## Features
- **Four waveform modes** (sine, saw, triangle, square) with precomputed lookup tables for low-jitter playback.
- **Monophonic keyboard** driven by a 4x3 matrix keypad mapped to two octaves of chromatic notes (C2–B3).
- **ADS envelope** with three potentiometers controlling attack, decay, and sustain in real time.
- **Visual feedback** on a 320×240 SPI TFT display showing the selected waveform and live ADS values.
- **I2S audio output** using SSI3 with DMA ping-pong buffers to stream 16-bit samples to a UDA1334A DAC breakout.
- **Interrupt-driven control loop** (Timer0A) for keypad scanning, waveform cycling button debouncing, and ADC refresh.
- **UART debug console** for runtime logging and system verification.

## Hardware Requirements
- TI TM4C1294NCPDT (Tiva-C) microcontroller or equivalent LaunchPad.
- UDA1334A (or compatible) I2S DAC module with line-level output.
- 4×3 membrane matrix keypad for note entry.
- Three analog potentiometers wired to ADC channels AIN6, AIN7, and AIN8.
- Momentary push button with two indicator LEDs to select waveforms.
- ILI9341-based SPI TFT display (used via `LCD_Display` driver library).
- D Latch as T Flip Flop connected to the SSI line of the uC and the FSS line of the DAC (converts SPI to I2S protocol)
- Optional: Audio amplifier, oscilloscope, headphones for evaluation.

> **Note:** Audio amplification is outside the firmware scope; feed the DAC output into an external amp or powered speakers.

## Firmware Architecture
### Signal Path
1. **Wave Tables** – `waveforms.c` fills 512-sample lookup tables for sine, saw, triangle, and square waves, plus a chromatic frequency table (`CHROMATIC_LEN = 24`).
2. **Function Generator** – `function_gen.c` maintains the phase accumulator, waveform selection state, and ADS envelope. `next_sample()` mixes the current waveform sample with the envelope level before handing it to the audio buffers.
3. **DMA Ping-Pong** – `dma.c` initialises uDMA channel 15 to alternately fill `pingBuffer` and `pongBuffer`. Each buffer holds 1024 frames and is replenished in the `SSI3_Handler` interrupt when the DMA mode stops.
4. **I2S Transport** – `SSI.c` configures SSI3 for 16-bit I2S-like transfers. When the transmit FIFO signals half-empty (DMATXMIS), the interrupt handler refills buffers and re-enables DMA.
5. **DAC Output** – Samples leave SSI3, clocked by `BCLK` at roughly `16 bits × 2 channels × SAMPLE_FREQ` (default 48 kHz) and latched by a T flip-flop generated `WSEL`.

### Control Loop & Inputs
- **Timer0A ISR** (`TIMER0A_Handler` in `input.c`) runs every 4 ms to scan the keypad, cycle waveforms on button press (`handle_waveform_state`), trigger ADC sampling, and feed the new ADS values into the envelope.
- **Keypad Mapping** – Key presses map to chromatic semitone steps, updating the phase increment and optionally resetting the phase for clean attacks.
- **Envelope Engine** – Attack, decay, and sustain values (0–1000 ms/level) come from the potentiometer ADC readings. 
- **Display Updates** – `display_utils.c` redraws the selected waveform and renders ADS values on the TFT. The main loop continually refreshes the display when `waveform_changed` is set by the button handler.

### Support Peripherals
- **ADC0 Sequencer 0** samples three channels back-to-back under Timer0A trigger.
- **UART0** provides serial logging at 115200 baud for diagnostics.
- **GPIO LEDs** on PG1/PK4 mirror the currently selected waveform.

## Building the Firmware
1. **Toolchain:** The project targets the Keil µVision environment (see `.uvprojx` and `.uvoptx` files) with TI’s TM4C device packs. Alternatively, you can port the build to GCC/Make by replicating the startup code and linker script used by the TM4C129.
2. **Dependencies:**
   - TI TM4C129 device headers (`TM4C129.h`) and `ES_Lib` utilities.
   - `LCD_Display` driver library for the ILI9341 TFT.
3. **Steps:**
   - Open `embedded-assignment.uvprojx` in Keil µVision.
   - Select the appropriate device (TM4C1294NCPDT) and ensure the system clock is set to 120 MHz (the firmware uses `ES_setSystemClk(120)`).
   - Build the project and flash it to the LaunchPad via the ICDI debugger.

### Configuration Tweaks
- Adjust `SAMPLE_FREQ`, `BASE_FREQ`, or SSI prescalers in `config.h` to experiment with different sample rates.
- Change `FRAME_COUNT` to trade audio latency for buffer refresh frequency.

## Usage
1. Power the LaunchPad and peripherals.
2. Connect the I2S lines (`BCLK`, `WSEL`, `DIN`) to the UDA1334A DAC; route the DAC output to speakers or headphones (through an amplifier).
3. Use the keypad to trigger notes. Keys map from `1` (C2) to `#` (B3).
4. Turn the attack, decay, and sustain potentiometers to sculpt the envelope. Values update every 4 ms.
5. Press the waveform button to cycle Sine → Saw → Triangle → Square; LEDs and the TFT label indicate the active mode.

## Testing & Validation
- **Aural Evaluation:** Waveforms were validated by ear in each mode. Clean tones emerge once the envelope reaches steady state; high sustain can introduce distortion due to quantisation and headroom limits.
- **Oscilloscope Capture:** DAC output (yellow) vs. digital data (green) confirms each waveform and the I2S timing (BCLK ≈ 1.54 MHz, WSEL ≈ 44–48 kHz).
- **Display Check:** Visual inspection confirmed waveform previews and ADS values, though minor rendering glitches remain.

## Known Issues & Future Work
- Sustain set to zero can lock the envelope, requiring a reset.
- Occasional display artefacts should be investigated in the SPI driver or drawing routines.
- Output headroom is limited; reducing waveform table amplitude or migrating to a higher-resolution DAC could lower distortion and hum.
- Planned enhancements include MIDI-over-USB input, polyphony, sequencer/arpeggiator features, preset storage (EEPROM), dedicated octave buttons, standalone ADC interrupt triggering, improved shielding, and live waveform visualisation.

## Acknowledgements
Developed as part of the Embedded Systems course project (September 2025). The audio demos, oscilloscope traces, and further documentation referenced in the project brief complement this firmware.
