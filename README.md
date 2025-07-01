# Satellite Communication Simulator

A C++ program that simulates the core processes of a satellite communication system: encoding data, transmitting it over a noisy channel, and decoding it on the receiver side. This simulator demonstrates how information is protected, transmitted, and recovered despite errors caused by noise.

## Features

- **Multiple Error-Correcting Codes**
  - Hamming(7,4) code - Can correct 1-bit errors per 7-bit block
  - Reed-Solomon code - Powerful code used in CDs, DVDs, and deep-space communication
  - BCH code - Used in satellite systems and modern communication

- **Data Encoding**
  - Converts text input into a bitstream
  - Implements various error-correcting codes
  - User can select the coding strategy at runtime

- **Transmission Channel Simulation**
  - Simulates a noisy communication channel by randomly flipping bits
  - Configurable bit error rate

- **Data Decoding**
  - Receives the noisy bitstream
  - Uses selected error correction code to detect and correct errors
  - Recovers the original data or reports failure

- **Visualization and Metrics**
  - ASCII-based visualization of bitstreams and error positions
  - Colorized output for better readability
  - Performance metrics including BER (Bit Error Rate)
  - Timing and throughput measurements

- **User Interface**
  - Command-line interface for user interaction
  - Algorithm selection at runtime
  - Detailed display of encoding, transmission, and decoding steps

## How It Works

1. **Encoding**: The input message is converted to bits, then encoded using the selected error correction algorithm, which adds redundancy to detect and correct errors.

2. **Transmission**: The encoded bitstream is passed through a simulated noisy channel where bits may be flipped based on the specified error rate.

3. **Decoding**: The received bitstream is decoded using the selected algorithm to detect and correct errors, recovering the original message.

4. **Analysis**: Performance metrics show effectiveness of the error correction, bit error rate, and processing efficiency.

## Building and Running

### Prerequisites

- C++17 compatible compiler (g++ or clang++)
- Make

### Build Instructions

1. Clone the repository
2. Run `make` in the project directory

```bash
git clone https://github.com/Albab-Hasan/Satellite-Communication-Simulator.git
cd Satellite-Communication-Simulator
make
```

### Running the Simulator

```bash
./sat-com
```

Or use the make run command:

```bash
make run
```

## Usage Example

```
===============================================================
           SATELLITE COMMUNICATION SIMULATOR
===============================================================

This program simulates a satellite communication system with:
  1. Data encoding with various error correction codes
  2. Noisy channel simulation
  3. Data decoding with error detection and correction
  4. Performance metrics and visualization

Enter message to transmit: Hello, World!

Available error correction codes:
  1. Hamming(7,4)
     Hamming(7,4) code can detect up to 2-bit errors and correct 1-bit errors per 7-bit block
  2. Reed-Solomon
     Reed-Solomon code can detect and correct multiple symbol errors, commonly used in CDs, DVDs, and deep-space communication
  3. BCH
     BCH codes are powerful cyclic error-correcting codes used in satellite communication and storage systems

Select error correction code (1-3): 1
Enter bit error rate (0.0 to 1.0): 0.05

=== SIMULATION RESULTS ===

Original bits: 01001000 01100101 01101100 01101100 01101111 00101100 00100000 01010111 01101111 01110010 01101100 01100100 00100001
Encoded bits (Hamming(7,4)): 1101000 0011101 1110110 1001001 1110110 0011100...

Transmission with noise:
  Original: 1101000 0011101 1110110...
  Modified: 1101000 0011101 1110110...
  Errors:                   ^

Bit errors introduced: 1 of 147 bits (0.68%)

Errors fixed by Hamming(7,4): 1

Received message: "Hello, World!"
Transmission SUCCESSFUL

=== Performance Metrics ===
Error Statistics:
  Total bits transmitted: 147
  Bit errors introduced: 1
  Bit error rate: 0.680272%
  Errors detected: 1
  Errors corrected: 1
  Error correction rate: 100.00%
Transmission Statistics:
  Message bits: 104
  Encoded bits: 147
  Code rate: 0.707
  Redundancy: 29.3%
Timing Information:
  String to Bits Conversion: 0.012 ms
  Encoding: 0.054 ms
  Channel Transmission: 0.037 ms
  Decoding: 0.065 ms
  Bits to String Conversion: 0.009 ms
  Total processing time: 0.177 ms
  Throughput: 830.508 bits/ms (830508.475 bits/second)

Run another simulation? (y/n):
```

## Technical Details

- Implementation of multiple error correction codes:
  - Hamming(7,4) code (4 data bits, 3 parity bits)
  - Reed-Solomon code with configurable parameters
  - BCH code for more powerful error correction
- Object-oriented design with Strategy pattern for coding algorithms
- Visualization with colored ASCII output
- Performance metrics for analyzing error correction effectiveness
