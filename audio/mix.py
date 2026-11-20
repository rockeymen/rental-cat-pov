"""Mix two CC0 field recordings into the approved yarn and sofa effects."""
from pathlib import Path
import subprocess
import sys
import wave
import numpy as np

rate = 48000
assets = Path(__file__).resolve().parent / 'assets'
with wave.open(sys.argv[1], 'rb') as bed:
    mix = np.frombuffer(bed.readframes(bed.getnframes()), '<i2').reshape(-1,2) / 32768.


def recording(name, filters):
    pcm = subprocess.check_output(['ffmpeg','-v','error','-i',str(assets/name),
                                  '-af',filters,'-ar',str(rate),'-ac','1','-f','f32le','-'])
    return np.frombuffer(pcm, '<f4')


def place(sound, source_in, source_out, at, peak, fade_in, fade_out):
    clip = sound[round(source_in*rate):round(source_out*rate)].astype(float)
    t = np.arange(len(clip)) / rate
    attack = np.sin(np.pi/2*np.clip(t/fade_in,0,1))**2
    release = np.sin(np.pi/2*np.clip((t[-1]-t)/fade_out,0,1))**2
    clip *= attack*release
    clip *= peak / np.max(np.abs(clip))
    start = round(at*rate)
    mix[start:start+len(clip)] += clip[:,None]


# Use contiguous runs of real steps, retaining the original irregular spacing.
# Trim stair resonance, particularly on the rug. Leave the stalking and pauses quiet.
paws = recording('paws.mp3','highpass=f=110,equalizer=f=235:t=o:w=1:g=-4,lowpass=f=1350')
rug = recording('paws.mp3','highpass=f=105,equalizer=f=240:t=o:w=1:g=-4,lowpass=f=840')
place(paws, .55, 1.12, 1.83, .044, .07, .095)
place(paws, 1.19, 1.79, 2.66, .053, .07, .11)
place(rug, .76, 1.98, 4.65, .045, .10, .22)
place(rug, 2.10, 2.67, 6.00, .018, .09, .16)
place(rug, 1.34, 1.93, 11.66, .032, .07, .12)
place(rug, .70, 1.10, 12.50, .026, .06, .12)

# ZJ-Davis recorded their own cat in a sound booth. Keep its pitch and timing.
mew = recording('mew.mp3','highpass=f=90,highshelf=f=3400:g=-2')
place(mew, .055, 1.01, 8.18, .115, .030, .105)

with wave.open(sys.argv[2],'wb') as out:
    out.setparams((2,2,rate,0,'NONE','not compressed'))
    out.writeframes((mix*32767).round().astype('<i2').tobytes())
