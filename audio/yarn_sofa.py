"""Synthesize dry, close-up Foley for the rental cat's 20-second POV."""
import sys
import wave

import numpy as np

RATE = 48000
mix = np.zeros((20 * RATE, 2))
rng = np.random.default_rng()
# Starting position of the production Foley take, before the first yarn touch.
rng.bit_generator.state = {'bit_generator': 'PCG64', 'state': {'state': 15262891258693690930273076581716442322, 'inc': 116944256504649905348104141670785955935}, 'has_uint32': 0, 'uinteger': 0}


def times(duration):
    return np.arange(round(duration * RATE)) / RATE


def curve(x, start, end):
    v = np.clip((x - start) / (end - start), 0, 1)
    return v * v * (3 - 2 * v)


def envelope(x, rise, release):
    return curve(x, 0, rise) * (1 - curve(x, rise, release))


def gesture(at, signal, pan=0):
    offset = round(at * RATE)
    gains = np.sqrt(np.array([1 - pan, 1 + pan]) * .5)
    mix[offset:offset + len(signal)] += signal[:, None] * gains


def fibers(x, center, width):
    """Band-limited movement of fabric, generated only during a contact."""
    freq = np.fft.rfftfreq(len(x), 1 / RATE)
    band = np.exp(-.5 * ((freq - center) / width) ** 2)
    band *= curve(freq, 35, 100)
    sound = np.fft.irfft(np.fft.rfft(rng.standard_normal(len(x))) * band, n=len(x))
    return sound / np.sqrt(np.mean(sound ** 2))


# A brush across the yarn and a few diminishing rotations, without a hard impact.
for at, side, strength in [(1.25,-.14,1), (4.0,.17,1.13)]:
    x = times(.195)
    brush = fibers(x, 510, 220) * envelope(x, .043, .185)
    pad = fibers(x, 180, 65) * envelope(x, .050, .14)
    gesture(at - .025, strength * (.038 * brush + .022 * pad), side)
    for delay, length, level in [(.17,.125,.024), (.36,.16,.016), (.64,.19,.009)]:
        x = times(length)
        gesture(at + delay, level * fibers(x, 320, 115) * envelope(x, length*.38, length), side)

# Fabric gives under the cat's weight on the sofa, followed by two small shifts.
for at, duration, level in [(13.10,.16,.022),(13.71,.34,.055),(14.06,.23,.031),
                            (14.63,.30,.026),(15.38,.29,.019),(16.16,.22,.012)]:
    x = times(duration)
    cloth = .53*fibers(x,185,85) + .32*fibers(x,515,175)
    gesture(at, level*cloth*envelope(x,.27*duration,duration), -.06)

with wave.open(sys.argv[1], 'wb') as out:
    out.setparams((2, 2, RATE, 0, 'NONE', 'not compressed'))
    out.writeframes((np.clip(mix, -1, 1)*32767).round().astype('<i2').tobytes())
