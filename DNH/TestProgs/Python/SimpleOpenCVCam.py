# Very simple hello-world example of OpenCV
# used to check the backends that get resolved
# for Linux and Windows.
#
# Also used as a performance baseline that can be
# expected in C++. In some cases, the C++ usage
# has been slow and this simple application was used
# to verify that the C++ had an issue that was causing
# it to run slower than expected.

import cv2

vcap = cv2.VideoCapture(0)
print( vcap.getBackendName())

while True:
    ret, frame = vcap.read()
    cv2.imshow('VIDEO', frame)
    cv2.waitKey(1)