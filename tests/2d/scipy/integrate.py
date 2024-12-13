import sys
from math import *
from ulab import scipy 

# this test is meaningful only if ulab is compiled with ulab.scipy.integrate
try:
    if str(type(scipy.integrate)) != "<class 'module'>":
        print ("scipy.integrate is not available")
        sys.exit(1)
except Exception as e:         
    print ("scipy.integrate is not available")
    sys.exit(1)
        
f = lambda x: x * sin(x) * exp(x)
a=1
b=2
functions = ( scipy.integrate.quad, scipy.integrate.romberg, scipy.integrate.simpson, scipy.integrate.quadgk )
for function in functions:
	    print (function(f, a, b))


