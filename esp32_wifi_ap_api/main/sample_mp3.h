#ifndef SAMPLE_MP3_H
#define SAMPLE_MP3_H

// This is a very small MP3 file embedded as a C array
// In a real application, you would replace this with your actual MP3 data
// This is just a placeholder and won't play real audio

#include <stdint.h>

// Sample MP3 data (placeholder - not real MP3 data)
// In a real application, you would convert an actual MP3 file to a C array
// using a tool like xxd -i filename.mp3 > filename.h
const unsigned char sample_mp3[] = {
    // MP3 file header and minimal data to be recognized as MP3
    0xFF, 0xFB, 0x90, 0x44, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Add more valid MP3 data for an actual test
};

const unsigned int sample_mp3_len = sizeof(sample_mp3);

#endif /* SAMPLE_MP3_H */ 