from ulab import numpy as np

print ( -0.3826834+0.9238795j + 0.5j )
print ( -0.3826834+0.9238795j + 0.5 )
print ( -0.3826834+0.9238795j - 0.5 )
print ( -0.3826834+0.9238795j - 0.5j )
print ( -0.3826834+0.9238795j * 0.5 )
print ( -0.3826834+0.9238795j / 0.5 )

a = np.array([3.14j], dtype=np.complex)
print(np.exp(a))

for n in [0,1j,2,0.3j,4,5j]:
    print(n**2)

x = (-1+1j)
print( np.sqrt(x) )