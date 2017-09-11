// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Wed Sep 6 17:19:29 2017
//  Last Modified : <170907.0921>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2017  Robert Heller D/B/A Deepwoods Software
//			51 Locke Hill Road
//			Wendell, MA 01379-9728
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// 
//
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Sounds.h"

#include <asoundlib.h>
#include <map>
#include <iostream>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <stdint.h>


AlertSoundFileMap AlertSound::soundFileMap;

void AlertSound::InitializeAlertSoundFileMap(std::string soundPath) {
    soundFileMap[ToDoToday] = soundPath + "/ToDoToday.wav";
    soundFileMap[EventToDay] = soundPath + "/EventToDay.wav";
    soundFileMap[EventSoon] = soundPath + "/EventSoon.wav";
}

AlertSound::AlertSound(AlertType alert) {
    alertSoundFile = soundFileMap[alert];
}

#define SoundDevice "plughw:0,0"

#define bytestoframes(bytes) ((bytes) / (format_chunk.NumChannels * (format_chunk.BitsPerSample/8)))

/*
 *  *   Underrun and suspend recovery
 *  */

static int xrun_recovery(snd_pcm_t *handle, int err)
{
    if (err == -EPIPE) {    /* under-run */
        err = snd_pcm_prepare(handle);
        if (err < 0)
            std::cerr << "Can't recovery from underrun, prepare failed: " << snd_strerror(err) << std::endl;
        return 0;
    } else if (err == -ESTRPIPE) {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN)
            sleep(1);       /* wait until the suspend flag is released */
        if (err < 0) {
            err = snd_pcm_prepare(handle);
            if (err < 0)
                std::cerr << "Can't recovery from suspend, prepare failed: " <<  snd_strerror(err) << std::endl;
        }
        return 0;
    }
    return err;
}


void AlertSound::Play() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;
    FILE *f;
    char errormessage[256];
    unsigned char *buffer;
    char message[256];
    snd_pcm_uframes_t framecount, size, buffer_size, period_size;
    size_t bytesread, buffersize;
    int err, dir;
    static unsigned int buffer_time = 500000;               /* ring buffer length in us */
    static unsigned int period_time = 100000;               /* period time in us */
    
    struct  {
        char chunkId[4]; // "RIFF"
        uint32_t ChunkSize; // File size - 8
        char Format[4];  // "WAVE"
    } riff_chunk;
    
    struct {
        char SubchunkID[4]; // "fmt "
        uint32_t SubchunkSize; // chunk size
        uint16_t AudioFormat; // PCM = 1 (i.e. Linear quantization)
        uint16_t NumChannels;
        uint32_t SampleRate;
        uint32_t ByteRate;
        uint16_t BlockAlign;
        uint16_t BitsPerSample;
    } format_chunk;
    
    struct {
        char SubchunkID[4]; // "data"
        uint32_t SubchunkSize; // chunk size
    } data_chunk_header;
    
    std::cerr << "*** AlertSound::Play: alertSoundFile is " << alertSoundFile << std::endl;
    
    f = fopen(alertSoundFile.c_str(),"rb");
    if (f == NULL) {
        std::cerr << "Error opening " << alertSoundFile << ", because " << strerror(errno) << std::endl;
        return;
    }
    
    if (fread(&riff_chunk,sizeof(riff_chunk),1,f) < 1) {
        strerror_r(errno,errormessage,sizeof(errormessage));
        std::cerr << "Error reading WAVE header " << alertSoundFile << ", because " << errormessage << std::endl;
        fclose(f);
        return;
    }
    if (strncmp(riff_chunk.chunkId,"RIFF",4) != 0) {
        std::cerr << "Not a RIFF file: " << alertSoundFile << std::endl;
        fclose(f);
        return;
    }
    if (strncmp(riff_chunk.Format,"WAVE",4) != 0) {
        std::cerr << "Not a WAVE file: " << alertSoundFile << std::endl;
        fclose(f);
        return;
    }
    if (fread(&format_chunk,sizeof(format_chunk),1,f) < 1) {
        strerror_r(errno,errormessage,sizeof(errormessage));
        std::cerr << "Error reading format header " << alertSoundFile << ", because " << errormessage << std::endl;
        fclose(f);
        return;
    }
    if (strncmp(format_chunk.SubchunkID,"fmt ",4) != 0) {
        std::cerr << "Missing fmt header: " << alertSoundFile << std::endl; 
        fclose(f);
        return;
    }
    if (fread(&data_chunk_header,sizeof(data_chunk_header),1,f) < 1) {
        strerror_r(errno,errormessage,sizeof(errormessage));
        std::cerr << "Error reading data chunk header " << alertSoundFile << ", because " << errormessage << std::endl;
        fclose(f);
        return;
    }
    if (strncmp(data_chunk_header.SubchunkID,"data",4) != 0) {
        std::cerr << "Missing data header: " << alertSoundFile << std::endl; 
        fclose(f);
        return;
    }
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_sw_params_alloca(&swparams);
    
    if ((err = snd_pcm_open(&handle, SoundDevice, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        std::cerr << "Playback open error: " <<  snd_strerror(err) << std::endl;
        fclose(f);
        return;
    }
    
    // hwparams
    err = snd_pcm_hw_params_any(handle, hwparams);
    if (err < 0) {
        std::cerr << "Broken configuration for playback: no configurations available: " << snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 1);
    if (err < 0) {
        std::cerr << "Resampling setup failed for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        std::cerr << "Access type not available for playback: " <<  snd_strerror(err) << std::endl;
        return;
    }
    if (format_chunk.BitsPerSample == 8) {
        err = snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_U8 );
    } else if (format_chunk.BitsPerSample == 16) {
        err = snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_S16_LE );
    } else {
        err = -ENODEV;
    }
    if (err < 0) {
        std::cerr << "Sample format not available for playback: " <<  snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_hw_params_set_channels(handle, hwparams, format_chunk.NumChannels);
    if (err < 0) {
        std::cerr << "Channels count (%i) not available for playbacks: " <<  snd_strerror(err) << std::endl;
        return;
    }
    unsigned int rrate = format_chunk.SampleRate;
    err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
    if (err < 0) {
        std::cerr << "Rate " << format_chunk.SampleRate <<
              "Hz not available for playback: " <<  snd_strerror(err) << std::endl;
        return;
    }
    if (rrate != format_chunk.SampleRate) {
        std::cerr << "Rate doesn't match (requested " << format_chunk.SampleRate <<
              "Hz, get " << rrate << "Hz)" << std::endl;
        return;
    }
    err = snd_pcm_hw_params_set_buffer_time_near(handle, hwparams, &buffer_time, &dir);
    if (err < 0) {
        std::cerr << "Unable to set buffer time " << buffer_time << " for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_hw_params_get_buffer_size(hwparams, &size);
    if (err < 0) {
        std::cerr << "Unable to get buffer size for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    buffer_size = size;
    err = snd_pcm_hw_params_set_period_time_near(handle, hwparams, &period_time, &dir);
    if (err < 0) {
        std::cerr << "Unable to set period time " << period_time << " for playback: " <<  snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_hw_params_get_period_size(hwparams, &size, &dir);
    if (err < 0) {
        std::cerr << "Unable to get period size for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    period_size = size;
    err = snd_pcm_hw_params(handle, hwparams);
    if (err < 0) {
        std::cerr << "Unable to set hw params for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    // swparams
    err = snd_pcm_sw_params_current(handle, swparams);
    if (err < 0) {
        std::cerr << "Unable to determine current swparams for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
    if (err < 0) {
        std::cerr << "Unable to set start threshold mode for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_size);
    if (err < 0) {
        std::cerr << "Unable to set avail min for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    err = snd_pcm_sw_params(handle, swparams);
    if (err < 0) {
        std::cerr << "Unable to set sw params for playback: " << snd_strerror(err) << std::endl;
        return;
    }
    
    buffersize = period_size * format_chunk.NumChannels * (format_chunk.BitsPerSample/8);
    buffer = new unsigned char[buffersize];
    bytesread = fread(buffer,sizeof(unsigned char),buffersize,f);
    while (bytesread > 0) {
        framecount = bytestoframes(bytesread);
        err = snd_pcm_writei(handle, buffer, framecount);
        if (err == -EAGAIN) continue;
        if (err < 0) {
            if (xrun_recovery(handle, err) < 0) {
                std::cerr << "Write error: " <<  snd_strerror(err) << std::endl;
                return;
            }
            break;  /* skip one period */
        }
        bytesread = fread(buffer,sizeof(unsigned char),buffersize,f);
    }
    delete buffer;
    fclose(f);
}

    
