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

(res, err) = scipy.integrate.quad(f, a, b)
if isclose (res, 7.11263821415851) and isclose (err, 5.438231077315757e-14):
	print (res, err)
		
res = scipy.integrate.romberg(f, a, b)
if isclose (res, 7.112638214158507):
	print (res)

res = scipy.integrate.simpson(f, a, b)
if isclose (res, 7.112638214158494):
	print (res)

(res, err) = scipy.integrate.quadgk(f, a, b)
if isclose (res, 7.112638214158507) and isclose (err, 7.686723611780195e-14):
		print (res, err)

