#include "mp3dec.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"

static const char *TAG = "MP3DEC";

// MP3 decoder state structure
typedef struct _MP3DecInfo {
    MP3FrameInfo lastFrameInfo;
    unsigned char mainBuf[MAINBUF_SIZE];
    int bytesLeft;
    int offset;
} MP3DecInfo;

HMP3Decoder MP3InitDecoder(void) {
    MP3DecInfo *mp3DecInfo;
    
    mp3DecInfo = (MP3DecInfo *)malloc(sizeof(MP3DecInfo));
    if (!mp3DecInfo)
        return NULL;
    
    // Initialize the decoder state
    memset(mp3DecInfo, 0, sizeof(MP3DecInfo));
    
    ESP_LOGI(TAG, "MP3 decoder initialized");
    return (HMP3Decoder)mp3DecInfo;
}

void MP3FreeDecoder(HMP3Decoder hMP3Decoder) {
    MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;
    
    if (mp3DecInfo) {
        free(mp3DecInfo);
        ESP_LOGI(TAG, "MP3 decoder freed");
    }
}

int MP3Decode(HMP3Decoder hMP3Decoder, uint8_t **inbuf, int *bytesLeft, int16_t *outbuf, int useSize) {
    MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;
    
    if (!mp3DecInfo || !inbuf || !*inbuf || !bytesLeft || !outbuf)
        return ERR_MP3_NULL_POINTER;
    
    // This is a simplified implementation - in a real decoder, this would do the actual decoding
    // For this example, we'll just copy some data and pretend it's decoded
    int outputSamps = 1152; // Common value for MP3
    
    // Just simulate decoding by copying some data (this is not actual MP3 decoding)
    int i;
    for (i = 0; i < outputSamps && i < (*bytesLeft / 2); i++) {
        outbuf[i] = (int16_t)(*inbuf)[i];
    }
    
    // Update inbuf and bytesLeft to reflect the consumed data
    *inbuf += outputSamps;
    *bytesLeft -= outputSamps;
    
    // Update frame info for last frame
    mp3DecInfo->lastFrameInfo.bitrate = 128;
    mp3DecInfo->lastFrameInfo.nChans = 2;
    mp3DecInfo->lastFrameInfo.samprate = 44100;
    mp3DecInfo->lastFrameInfo.bitsPerSample = 16;
    mp3DecInfo->lastFrameInfo.outputSamps = outputSamps;
    mp3DecInfo->lastFrameInfo.layer = 3;
    mp3DecInfo->lastFrameInfo.version = MPEG1;
    
    return outputSamps;
}

void MP3GetLastFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo) {
    MP3DecInfo *mp3DecInfo = (MP3DecInfo *)hMP3Decoder;
    
    if (!mp3DecInfo || !mp3FrameInfo)
        return;
    
    *mp3FrameInfo = mp3DecInfo->lastFrameInfo;
}

int MP3GetNextFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo *mp3FrameInfo, uint8_t *buf) {
    // Simplified implementation - just fill in some values
    if (!mp3FrameInfo)
        return -1;
    
    mp3FrameInfo->bitrate = 128;
    mp3FrameInfo->nChans = 2;
    mp3FrameInfo->samprate = 44100;
    mp3FrameInfo->bitsPerSample = 16;
    mp3FrameInfo->outputSamps = 1152;
    mp3FrameInfo->layer = 3;
    mp3FrameInfo->version = MPEG1;
    
    return 0;
}

int MP3FindSyncWord(uint8_t *buf, int nBytes) {
    // Simple implementation to find MP3 sync word (0xFF, 0xE?)
    int i;
    
    if (!buf || nBytes < 2)
        return -1;
    
    // Look for the sync word pattern
    for (i = 0; i < nBytes - 1; i++) {
        if ((buf[i] == 0xFF) && ((buf[i+1] & 0xE0) == 0xE0))
            return i;
    }
    
    return -1;
} 