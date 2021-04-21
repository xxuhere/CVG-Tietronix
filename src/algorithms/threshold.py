# part of this file was taken from scikit-image

import cv2
import numpy as np

from src.algorithms.utils import _validate_image_histogram


def threshold_yen(image=None, nbins=256, *, hist=None):
    counts, bin_centers = _validate_image_histogram(image, hist, nbins)

    # On blank images (e.g. filled with 0) with int dtype, `histogram()`
    # returns ``bin_centers`` containing only one value. Speed up with it.
    if bin_centers.size == 1:
        return bin_centers[0]

    # Calculate probability mass function
    pmf = counts.astype(np.float32) / counts.sum()
    P1 = np.cumsum(pmf)  # Cumulative normalized histogram
    P1_sq = np.cumsum(pmf ** 2)
    # Get cumsum calculated from end of squared array:
    P2_sq = np.cumsum(pmf[::-1] ** 2)[::-1]
    # P2_sq indexes is shifted +1. I assume, with P1[:-1] it's help avoid
    # '-inf' in crit. ImageJ Yen implementation replaces those values by zero.
    crit = np.log(((P1_sq[:-1] * P2_sq[1:]) ** -1) * (P1[:-1] * (1.0 - P1[:-1])) ** 2)
    return bin_centers[crit.argmax()]


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
