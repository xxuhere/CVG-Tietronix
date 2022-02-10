# Extended version of SimpleOpenCVCam that runs
# two direct camera streams at once.

import cv2

# For code simplicity, the assumed USB webcam 
# paths are hard coded.

vcap1 = cv2.VideoCapture("/dev/video0")
print( vcap1.getBackendName())

vcap2 = cv2.VideoCapture("/dev/video2")
print( vcap2.getBackendName())

while True:
    ret, frame = vcap1.read()
    
    if ret:
        cv2.imshow('VIDEO_1', frame)
    
    ret, frame = vcap2.read()
    if ret:
        cv2.imshow('VIDEO_2', frame)
    
    cv2.waitKey(1)