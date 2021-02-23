import numpy as np
import cv2
from skimage.filters import threshold_yen


def thresholding(image: np.ndarray):
    grey = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    clahe = cv2.createCLAHE(clipLimit=2.7)
    cl = clahe.apply(grey)

    thresh = threshold_yen(cl)
    binary = cl.copy()
    binary[binary <= thresh] = 0
    binary = cv2.medianBlur(binary, 7)
    edges = cv2.Canny(binary, 120, 220)
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3), anchor=(1, 1))
    dilated = cv2.dilate(edges, kernel)

    flood = dilated.copy()
    cv2.floodFill(flood, mask=None, seedPoint=(0, 0), newVal=255)
    return ~flood
