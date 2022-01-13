import math
import sys
sys.path.append('.')

from snippets import scipy
from ulab import numpy as np

np.set_printoptions(threshold=100)

a = [4, 3, 1, 2-2j, 2+2j, 2-1j, 2+1j, 2-1j, 2+1j, 1+1j, 1-1j]
#print('_cplxreal: ', scipy.cplxreal(a))
f = np.array([-1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0], dtype=np.float)
t = (0.9984772174419884+0.01125340518638924j)
w = 'real'
#print('nearest_real_complex_idx: ', scipy.nearest_real_complex_idx(f,t,w))


nyquistRate = 48000 * 2
centerFrequency_Hz = 480.0
lowerCutoffFrequency_Hz = centerFrequency_Hz/math.sqrt(2)
upperCutoffFrequenc_Hz = centerFrequency_Hz*math.sqrt(2)
wn = np.array([ lowerCutoffFrequency_Hz, upperCutoffFrequenc_Hz])/nyquistRate

#print('butter: ', scipy.butter(N=4, Wn=wn, btype='bandpass', analog=False, output='ba'))
#print('butter: ', scipy.butter(N=4, Wn=wn, btype='bandpass', analog=False, output='zpk'))
print('butter: ', scipy.butter(N=4, Wn=wn, btype='bandpass', analog=False, output='sos'))
#print('butter: ', scipy.butter(N=4, Wn=wn, btype='bandstop', analog=False, output='ba'))
#print('butter: ', scipy.butter(N=4, Wn=wn, btype='lowpass', analog=False, output='ba'))
#print('butter: ', scipy.butter(N=4, Wn=wn, btype='highpass', analog=False, output='ba'))

