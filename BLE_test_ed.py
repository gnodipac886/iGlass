import asyncio
import numpy as np
import pandas as pd
# import matplotlib.pyplot as plt
# import scipy
from sklearn import svm
# from scipy.spatial.transform import Rotation
# from scipy.signal import find_peaks, butter, filtfilt
# from scipy.io import wavfile
import time
import threading
import ble_itf

loop = asyncio.get_event_loop()
loop.run_until_complete(ble_itf.main())
# global stop_thread
# stop_thread = 1
# walking_thread.join()

