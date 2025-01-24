Hardware RNG based on CPU timing jitter
=======================================

The Jitter RNG provides a noise source using the CPU execution timing jitter.
It does not depend on any system resource other than a high-resolution time
stamp. It is a small-scale, yet fast entropy source that is viable in almost
all environments and on a lot of CPU architectures.

The implementation of the Jitter RNG is independent of any operating system.
As such, it could even run on baremetal without any operating system.

The design of the RNG is given in the documentation found in at
http://www.chronox.de/jent . This documentation also covers the full
assessment of the SP800-90B compliance as well as all required test code.

API
---

The API is documented in the man page jitterentropy.3.

To use the Jitter RNG, the header file jitterentropy.h must be included.

Build Instructions
==================

To generate the shared library `make` followed by `make install`.

Android
-------

To compile the code on Android, use the following Makefile:

arch/android/Android.mk	-- NDK make file template that can be used to directly
			   compile the CPU Jitter RNG code into Android binaries

Direct CPU instructions
-----------------------

If the function in jent_get_nstime is not available, you can replace the
jitterentropy-base-user.h with examples from the arch/ directory.

Testing
=======

There are numerous tests around the Jitter RNG. Yet, they are too big to be
loaded into the official repository. Email me, if you want them.

Version Numbers
===============
The version numbers for this library have the following schema:
MAJOR.MINOR.PATCHLEVEL

Changes in the major number implies API and ABI incompatible changes, or
functional changes that require consumer to be updated (as long as this
number is zero, the API is not considered stable and can change without a
bump of the major version).

Changes in the minor version are API compatible, but the ABI may change.
Functional enhancements only are added. Thus, a consumer can be left
unchanged if enhancements are not considered. The consumer only needs to
be recompiled.

Patchlevel changes are API / ABI compatible. No functional changes, no
enhancements are made. This release is a bug fixe release only. The
consumer can be left unchanged and does not need to be recompiled.

Author
======
Stephan Mueller <smueller@chronox.de>

---

### **Enhancements and Modifications**
This section describes the changes and additions made to the project during the experimentation and improvement of the Jitter RNG implementation.

#### **Raw Entropy Collection**
- A new program `collect_entropy.c` was implemented to collect raw entropy using the jitterentropy library. It writes the raw timing jitter deltas to a binary file (`raw_entropy.bin`) for analysis.
- The program demonstrates the use of key library functions:
  - `jent_entropy_collector_alloc`: Initializes the entropy collector.
  - `jent_measure_jitter`: Measures timing jitter to generate entropy.
  - `jent_get_nstime`: Retrieves high-resolution timestamps.

#### **Entropy Analysis**
- The `ent` tool was used to evaluate the quality of the raw entropy data. Key metrics such as entropy per byte, chi-square distribution, and Monte Carlo approximations were analyzed.

#### **Testing and Debugging**
- Experiments were conducted to improve entropy quality:
  - Added artificial jitter via a busy loop to increase variability in timing deltas.
  - Adjusted the oversampling rate in the entropy collector to test its impact on randomness.
- Timer resolution issues were diagnosed by printing consecutive deltas, revealing that macOS's timer precision limits entropy quality.

#### **New Makefile for `collect_entropy.c`**
- A custom `Makefile.collect` was created to simplify building the `collect_entropy.c` program:
  ```bash
  make -f Makefile.collect
  ```
- This Makefile includes the necessary compilation flags and includes paths for building the program with dependencies in the `src/` directory.

---

### **Pending Improvements**
The following tasks and enhancements are recommended for future development:
1. **Platform-Specific Optimization:**
   - Investigate using Linux for testing, as macOS's timer resolution may inherently limit entropy quality.
   - Experiment with platform-specific enhancements to improve jitter measurement.

2. **Entropy Post-Processing:**
   - Explore additional techniques to process and enhance the raw entropy, such as alternative hashing algorithms or noise amplification strategies.

3. **Documentation Enhancements:**
   - Expand the API documentation to include usage examples of the key functions (e.g., `jent_measure_jitter`, `jent_get_nstime`).
   - Document the step-by-step process of using the `ent` tool to analyze entropy files.

4. **Testing with Reference Implementation:**
   - Compare results with the reference `jitterentropy-rng` implementation and document any differences or platform-specific behaviors.

5. **Error Handling:**
   - Enhance error handling in the `collect_entropy.c` program to ensure robustness, especially when initializing the entropy collector or handling file I/O.

6. **Code Modularization:**
   - Refactor the code to make it modular and reusable, enabling easier integration into future projects.

---

### **How to Use the New Features**
#### **Building and Running `collect_entropy.c`**
1. Build the program using the provided `Makefile.collect`:
   ```bash
   make -f Makefile.collect
   ```
2. Run the program to generate raw entropy:
   ```bash
   ./collect_entropy
   ```
3. Analyze the generated file using the `ent` tool:
   ```bash
   ent raw_entropy.bin
   ```

#### **Improving Randomness**
- To improve randomness, experiment with the oversampling rate and artificial jitter by modifying the following in `collect_entropy.c`:
  - Oversampling rate:
    ```c
    struct rand_data *entropy_collector = jent_entropy_collector_alloc(20, 0);
    ```
  - Adding artificial jitter:
    ```c
    for (volatile int j = 0; j < 1000; j++);
    ```

#### **Testing with Reference Implementation**
1. Build the reference `jitterentropy-rng` implementation:
   ```bash
   gcc -o jitterentropy-rng tests/raw-entropy/recording_userspace/jitterentropy-rng.c src/*.c -pthread -I./src
   ./jitterentropy-rng > rng_output.bin
   ```
2. Analyze its output:
   ```bash
   ent rng_output.bin
   ```

---

### **Acknowledgments**
These changes and experiments are part of an ongoing effort to understand and improve the Jitter RNG implementation, especially its ability to generate high-quality entropy across different platforms.

Second Author
======
Georgia Vrana <georgia.m.vrana@gmail.com>