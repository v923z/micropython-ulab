import sys
from math import *

# this test is meaningful only if ulab is compiled with ulab.scipy.integrate
# we're not going to test CPython scipy. 
try:
    from ulab import scipy
except Exception as e:
    print ("could not import ulab.scipy: ", e)
    sys.exit(1) 

i = scipy.integrate
try:
    if str(type(i)) != "<class 'module'>":
        print ("scipy.integrate is not available")
        sys.exit(1)
except Exception as e:         
    print ("scipy.integrate is not available")
    sys.exit(1)
        
f = lambda x: x * sin(x) * exp(x)
a=1
b=2
# what if they are not all available? 
algorithms = ( i.quad, i.romberg, i.simpson, i.quadgk )
for quad in algorithms:
	    print (quad(f, a, b))


