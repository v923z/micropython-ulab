try:
    from ulab import numpy as np
except:
    import numpy as np

a = np.array(range(8)).reshape((2, 4))
np.savetxt('loadtxt.dat', a, header='test file data')

print(np.loadtxt('loadtxt.dat'))

np.savetxt('loadtxt.dat', a, delimiter=',', header='test file data')

print(np.loadtxt('loadtxt.dat', delimiter=','))


np.savetxt('loadtxt.dat', a, delimiter=',', comments='!', header='test file data')

print(np.loadtxt('loadtxt.dat', delimiter=',', comments='!'))
print(np.loadtxt('loadtxt.dat', delimiter=',', comments='!', usecols=1))
print(np.loadtxt('loadtxt.dat', delimiter=',', comments='!', usecols=(0, 1)))


a = np.array(range(36)).reshape((9, 4))
np.savetxt('loadtxt.dat', a, header='9 data rows and a comment')
print(np.loadtxt('loadtxt.dat', max_rows=5))
