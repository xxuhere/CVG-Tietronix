# taken from scikit-image

import numpy as np

_integer_types = (
    np.byte,
    np.ubyte,  # 8 bits
    np.short,
    np.ushort,  # 16 bits
    np.intc,
    np.uintc,  # 16 or 32 or 64 bits
    int,
    np.int_,
    np.uint,  # 32 or 64 bits
    np.longlong,
    np.ulonglong,
)  # 64 bits
_integer_ranges = {t: (np.iinfo(t).min, np.iinfo(t).max) for t in _integer_types}
dtype_range = {
    bool: (False, True),
    np.bool_: (False, True),
    np.bool8: (False, True),
    float: (-1, 1),
    np.float_: (-1, 1),
    np.float16: (-1, 1),
    np.float32: (-1, 1),
    np.float64: (-1, 1),
}
dtype_range.update(_integer_ranges)


def dtype_limits(image, clip_negative=False):
    imin, imax = dtype_range[image.dtype.type]
    if clip_negative:
        imin = 0
    return imin, imax


def _offset_array(arr, low_boundary, high_boundary):
    """Offset the array to get the lowest value at 0 if negative."""
    if low_boundary < 0:
        offset = low_boundary
        dyn_range = high_boundary - low_boundary
        # get smallest dtype that can hold both minimum and offset maximum
        offset_dtype = np.promote_types(
            np.min_scalar_type(dyn_range), np.min_scalar_type(low_boundary)
        )
        if arr.dtype != offset_dtype:
            # prevent overflow errors when offsetting
            arr = arr.astype(offset_dtype)
        arr = arr - offset
    else:
        offset = 0
    return arr, offset


def _bincount_histogram(image, source_range):
    if source_range not in ["image", "dtype"]:
        raise ValueError(
            "Incorrect value for `source_range` argument: {}".format(source_range)
        )
    if source_range == "image":
        image_min = int(image.min().astype(np.int64))
        image_max = int(image.max().astype(np.int64))
    elif source_range == "dtype":
        image_min, image_max = dtype_limits(image, clip_negative=False)
    image, offset = _offset_array(image, image_min, image_max)
    hist = np.bincount(image.ravel(), minlength=image_max - image_min + 1)
    bin_centers = np.arange(image_min, image_max + 1)
    if source_range == "image":
        idx = max(image_min, 0)
        hist = hist[idx:]
    return hist, bin_centers


def histogram(image, nbins=256, source_range="image", normalize=False):

    image = image.flatten()
    # For integer types, histogramming with bincount is more efficient.
    if np.issubdtype(image.dtype, np.integer):
        hist, bin_centers = _bincount_histogram(image, source_range)
    else:
        if source_range == "image":
            hist_range = None
        elif source_range == "dtype":
            hist_range = dtype_limits(image, clip_negative=False)
        else:
            ValueError("Wrong value for the `source_range` argument")
        hist, bin_edges = np.histogram(image, bins=nbins, range=hist_range)
        bin_centers = (bin_edges[:-1] + bin_edges[1:]) / 2.0

    if normalize:
        hist = hist / np.sum(hist)
    return hist, bin_centers


def _validate_image_histogram(image, hist, nbins=None):
    if image is None and hist is None:
        raise Exception("Either image or hist must be provided.")

    if hist is not None:
        if isinstance(hist, (tuple, list)):
            counts, bin_centers = hist
        else:
            counts = hist
            bin_centers = np.arange(counts.size)
    else:
        counts, bin_centers = histogram(image.ravel(), nbins, source_range="image")
    return counts.astype(float), bin_centers
