// filepath: /Users/georgia.vrana/Desktop/jitterentropy-library-master/collect_entropy.c

#include <stdio.h>                    
#include <stdlib.h>                   // Standard library for memory management.
#include "./jitterentropy.h"              // Main header file for jitterentropy.
#include "./src/jitterentropy-noise.h"      // Noise-related entropy functions.

#define OUTPUT_FILE "raw_entropy.bin" // File to save raw entropy.
#define NUM_ENTRIES 1000000           // Number of raw entropy entries to collect.

int main() {

    // Initialize entropy collector
    struct rand_data *entropy_collector = jent_entropy_collector_alloc(20, 0); // OSR=20, no special flags
    if (!entropy_collector) {
        fprintf(stderr, "Failed to allocate entropy collector.\n");
        return 1;
    }

    // Open binary file for writing
    FILE *output_file = fopen(OUTPUT_FILE, "wb");
    if (!output_file) {
        fprintf(stderr, "Failed to open output file: %s\n", OUTPUT_FILE);
        jent_entropy_collector_free(entropy_collector); // Free collector on failure.
        return 1;
    }

    // Collect raw entropy and write to the file
    for (size_t i = 0; i < NUM_ENTRIES; i++) {
        uint64_t current_delta = 0;

        // Add a busy loop to create artificial jitter
        for (volatile int j = 0; j < 1000; j++);

        // Measure raw entropy (time delta)
        jent_measure_jitter(entropy_collector, 0, &current_delta);

        // Debug: Print the collected delta
        printf("Raw delta [%zu]: %lu\n", i, current_delta);

        // Write the raw delta to the binary file
        if (fwrite(&current_delta, sizeof(uint64_t), 1, output_file) != 1) {
            fprintf(stderr, "Failed to write data to file.\n");
            fclose(output_file);
            jent_entropy_collector_free(entropy_collector);
            return 1;
        }
    }

    uint64_t start, end, delta;

    for (int i = 0; i < 10; i++) {
        jent_get_nstime(&start);
        jent_get_nstime(&end);
        delta = end - start;
        printf("Timer delta: %llu\n", delta);
    }


    // Clean up
    fclose(output_file);
    jent_entropy_collector_free(entropy_collector);

    printf("Raw entropy successfully written to %s\n", OUTPUT_FILE);
    return 0;
}

