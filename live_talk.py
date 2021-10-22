import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import scipy
import asyncio
from scipy.spatial.transform import Rotation
from scipy.signal import find_peaks, butter, filtfilt
from scipy.io import wavfile
from IPython.display import Audio
from os import listdir
from os.path import isfile, join
from time import sleep
import threading
import ble_itf

global stop_thread

onlyfiles = [f for f in listdir('.') if isfile(join('.', f))]
talk_files = [f for f in onlyfiles if 'talking_' in f and '.csv' in f and '_user' in f]

talking_datas = []
talking_ffts = []
fs = 0
for f in talk_files:
    # read in the data
    data = dict(pd.read_csv(f))
    
    #sum up all the axis
    mic_data = np.array(data['mic'])
    
    # create a filtered data
    fs = len(mic_data) / 10
    b, a = butter(3, 80 / (fs/2), 'high')      
    y = filtfilt(b, a, mic_data)
    
    # calculate the fft on filtered data and take out DC
    mic_data_fft = np.abs(np.fft.rfft(y, n = 1024))
    # mic_data_fft[:1] = 0
    
    # add data to the lists
    talking_datas.append(mic_data)
    talking_ffts.append(mic_data_fft)
    
avg_talking_fft = np.average(talking_ffts, axis = 0)

def live_talking_detection():
    global stop_thread
    while(stop_thread):
        mic_buf = ble_itf.mic_buf
        try:
            ax_y_z = np.array(mic_buf[-1024:])
            if len(ax_y_z) == 0:
                continue
            b, a = butter(3, 80 / (fs/2), 'high')
            y = filtfilt(b, a, ax_y_z)
            live_fft = np.abs(np.fft.rfft(y, n=1024))
            correlation = np.correlate(avg_talking_fft, live_fft)[0]
            if correlation > 45000000:
                print('talking', correlation)
            else:
                print('not talking', correlation)
            sleep(0.5)
        except:
            continue

# def main():
global stop_thread
stop_thread = 1
talking_thread = threading.Thread(target = live_talking_detection, name = 'talking_thread')
talking_thread.start()

loop = asyncio.get_event_loop()
loop.run_until_complete(ble_itf.main())

stop_thread = 0
sleep(2)
# global stop_thread
stop_thread = 1
print('imu data_rate: ', len(ble_itf.mic_buf['ax']) / 10)
print('mic data_rate: ', len(ble_itf.mic_buf) / 10)

# if __name__ == '__main__':
#     main()
