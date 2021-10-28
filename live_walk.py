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
imu_files = [f for f in onlyfiles if 'imu_w' in f and '.csv' in f]

walking_datas = []
walking_ffts = []
fs = 0
for f in imu_files:
    # read in the data
    data = dict(pd.read_csv(f))

    # get sample rate
    fs = len(data['a_xyz']) / 10
    
    #sum up all the axis
    ax_y_z = np.array(data['a_xyz'])         #np.array(data['ax']) + np.array(data['ay']) + np.array(data['az'])
    
    # create a filtered data
    # 0.048
    b, a = butter(3, 2.4/(fs/2), 'low')
    y = filtfilt(b, a, ax_y_z)
    
    # calculate the fft on filtered data and take out DC
    ax_y_z_fft = np.abs(np.fft.rfft(y, n = 256))
    ax_y_z_fft[:1] = 0
    
    # add data to the lists
    walking_datas.append(ax_y_z)
    walking_ffts.append(ax_y_z_fft)
    
avg_walking_fft = np.average(walking_ffts, axis = 0)

def live_walking_detection():
    global stop_thread
    while(stop_thread):
        imu_buf = ble_itf.imu_buf
        # if len(imu_buf['ax'][-256:]) != len(imu_buf['ay'][-256:]) or len(imu_buf['ay'][-256:]) != len(imu_buf['az'][-256:]) or len(imu_buf['ax'][-256:]) != len(imu_buf['az'][-256:]):
        #     continue
        try:
            ax_y_z = np.array(imu_buf['a_xyz'][-256:])         #np.array(imu_buf['ax'][-256:]) + np.array(imu_buf['ay'][-256:]) + np.array(imu_buf['az'][-256:])
            if len(ax_y_z) == 0:
                continue
            b, a = butter(3, 2.4/(fs/2), 'low')
            y = filtfilt(b, a, ax_y_z)
            live_fft = np.abs(np.fft.rfft(y, n = 256))
            correlation = np.correlate(avg_walking_fft, live_fft)[0]
            if correlation > 700000:
                print('walking', correlation)
            else:
                print('not walking', correlation)
            sleep(0.5)
        except:
            continue

# def main():
ble_itf.mic_buf = []
ble_itf.imu_buf = {
    'a_xyz' : []
    # 'ax' : [],
    # 'ay' : [],
    # 'az' : [],
    'wx' : [],
    'wy' : [],
    'wz' : [],
    'mx' : [],
    'my' : [],
    'mz' : [],
}
global stop_thread
stop_thread = 1
walking_thread = threading.Thread(target = live_walking_detection, name = 'walking_thread')
walking_thread.start()

loop = asyncio.get_event_loop()
loop.run_until_complete(ble_itf.main())

stop_thread = 0
sleep(2)
# global stop_thread
stop_thread = 1
print('imu data_rate: ', len(ble_itf.imu_buf['a_xyz']) / 10)
print('mic data_rate: ', len(ble_itf.mic_buf) / 10)

# if __name__ == '__main__':
#     main()
