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
fs_imu = 0
for f in talk_files:
    # read in the data
    data = dict(pd.read_csv(f))
    
    #sum up all the axis
    # nan_idx_x = np.argwhere(np.isnan(np.array(data['ax'])))[0][0]
    # nan_idx_y = np.argwhere(np.isnan(np.array(data['ay'])))[0][0]
    # nan_idx_z = np.argwhere(np.isnan(np.array(data['az'])))[0][0]
    imu_data = np.array(data['a_xyz'])   #np.array(data['ax'])[:nan_idx_x] + np.array(data['ay'])[:nan_idx_y] + np.array(data['az'])[:nan_idx_z]
    
    # create a filtered data
    fs_imu = len(imu_data) / 10
    b_imu, a_imu = butter(3, 35 / (fs_imu/2), 'high')
    y_imu = filtfilt(b_imu, a_imu, imu_data)
    
    # calculate the fft on filtered data and take out DC
    imu_data_fft = np.abs(np.fft.rfft(y_imu, n = 256)) ** 3
    # mic_data_fft[:1] = 0
    
    # add data to the lists
    talking_datas.append(imu_data)
    talking_ffts.append(imu_data_fft)
    
avg_talking_fft = np.average(talking_ffts, axis = 0)

# print()

def live_talking_detection():
    global stop_thread
    while(stop_thread):
        imu_buf = ble_itf.imu_buf
        # if len(imu_buf['ax'][-256:]) != len(imu_buf['ay'][-256:]) or len(imu_buf['ay'][-256:]) != len(imu_buf['az'][-256:]) or len(imu_buf['ax'][-256:]) != len(imu_buf['az'][-256:]):
        #     continue
        try:
            ax_y_z = np.array(imu_buf['a_xyz'][-256:])     #np.array(imu_buf['ax'][-256:]) + np.array(imu_buf['ay'][-256:]) + np.array(imu_buf['az'][-256:])
            if len(ax_y_z) == 0:
                continue
            b_imu, a_imu = butter(3, 35 / (fs_imu/2), 'high')
            y_imu = filtfilt(b_imu, a_imu, ax_y_z)
            live_fft = np.abs(np.fft.rfft(y_imu, n = 256)) ** 3
            correlation = np.correlate(avg_talking_fft, live_fft)[0]
            correlation = correlation / (10 ** 16)
            if correlation > 3:
                print('user talking', correlation)
            else:
                print('not user talking', correlation)
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
print('imu data_rate: ', len(ble_itf.mic_buf['a_xyz']) / 10)
print('mic data_rate: ', len(ble_itf.mic_buf) / 10)

# if __name__ == '__main__':
#     main()
