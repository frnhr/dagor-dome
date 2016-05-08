import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import math

def polyfit2d(X, Y, Z):
    X = X.flatten()
    Y = Y.flatten()
    B = Z.flatten()

    A = np.array([X * 0 + 1, X, Y, X ** 2, X ** 2 * Y, X ** 2 * Y ** 2, Y ** 2, X * Y ** 2, X * Y]).T

    coeff, r, rank, s = np.linalg.lstsq(A, B)

    return coeff

def findDomePosition(telAz, telAlt, points, radius=335):
    points = np.swapaxes(points, 0, 1)
    print points.shape[0]
    choosenPoints = np.empty((3, points.shape[0]))
    i = 0

    for az, alt, domeAz in points:
        if ((telAz > 0 and telAlt > 0) and not (az > 0 and alt > 0) or
        (telAz > 0 and telAlt <= 0) and not (az > 0 and alt <= 0) or
        (telAz <= 0 and telAlt > 0) and not (az <= 0 and alt > 0) or
        (telAz <= 0 and telAlt <= 0) and not (az <= 0 and alt <= 0)):
            continue

        distance = math.sqrt((telAz-az) ** 2  + (telAlt-alt) ** 2)
        if(distance < radius):
            choosenPoints[0][i] = az
            choosenPoints[1][i] = alt
            choosenPoints[2][i] = domeAz
            i += 1

    print i
    pol = polyfit2d(choosenPoints[0], choosenPoints[1], choosenPoints[2])
    print pol
    fittedAz = pol[0] + pol[1]*telAz + pol[2]*telAlt + pol[3]*telAz*telAz + pol[4]*telAz*telAz*telAlt + pol[5]*telAz*telAz*telAlt*telAlt + pol[6]*telAlt*telAlt + pol[7]*telAz*telAlt*telAlt + pol[8]*telAz*telAlt

    return fittedAz

inp = np.loadtxt("aazaltaz", dtype="f5,S12,S12")
points = np.empty((3, inp.size)) # Az, ALt, Dome Az

i = 0
for arr in inp:
    points[2][i] = arr[0]

    s = arr[1].split(":")
    y = float(s[0])
    y += float(s[1]) / 60
    y += float(s[2]) / 3600
    points[1][i] = y

    s = arr[2].split(":")
    x = float(s[0])
    x += float(s[1]) / 60
    x += float(s[2]) / 3600
    points[0][i] = x

    i += 1

newAz = 165
newAlt = 70


cartX = np.sin(np.deg2rad(90 - points[1]))*np.cos(np.deg2rad(points[0]))
cartY = np.sin(np.deg2rad(90 - points[1]))*np.sin(np.deg2rad(points[0]))
cartZ = points[2] - points[0]

newDomeAz = findDomePosition(np.sin(np.deg2rad(90 - newAlt))*np.cos(np.deg2rad(newAz)), np.sin(np.deg2rad(90 - newAlt))*np.sin(np.deg2rad(newAz)), [cartX, cartY, cartZ])

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(cartX, cartY, cartZ, c='r')
ax.scatter(np.sin(np.deg2rad(90 - newAlt))*np.cos(np.deg2rad(newAz)), np.sin(np.deg2rad(90 - newAlt))*np.sin(np.deg2rad(newAz)), newDomeAz, c='g')

ax.set_xlabel('Tel Az')
ax.set_ylabel('Tel ALt')
ax.set_zlabel('Dome Az')

plt.show()