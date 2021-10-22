import numpy as np
from scipy import signal
import random
import serial
import threading
import time

from itertools import count
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk

import ble_itf

class ble_imu:
	def __init__(self, port = 'COM4', baud = 115200):
		# init serial port stuff
		self.ser = serial.Serial()
		self.ser.port = port
		self.ser.baudrate = baud
		self.ser.timeout = 10
		
		# constants
		self.START = '55'
		self.TIME = '50'
		self.ACC = '51'
		self.OMEGA = '52'
		self.ANGLE = '53'
		self.MAG = '54'
		self.DATA_STAT = '55'
		self.QUAT = '59'
		
		# data dictionaries
		self.acc_dict = {
			'ax': 0.0,
			'ay': 0.0,
			'az': 0.0
		}
		self.omega_dict = {
			'wx': 0.0,
			'wy': 0.0,
			'wz': 0.0
		}
		self.angle_dict = {
			'roll': 0.0,
			'pitch': 0.0,
			'yaw': 0.0
		}
		self.mag_dict = {
			'mx': 0.0,
			'my': 0.0,
			'mz': 0.0
		}
		self.units_dict = {
			self.ACC : 'Acceleration ($m/s^2$)',
			self.OMEGA : 'Angular Velocity ($rad/s$)',
			self.ANGLE : 'Angle ($deg$)',
			self.MAG : 'Magnetic Field ($\mu$$T$)'
		}
		self.temp = 0.0
		self.dict_sel = {
			self.ACC : self.acc_dict,
			self.OMEGA : self.omega_dict,
			self.ANGLE : self.angle_dict,
			self.MAG : self.mag_dict
		}

		# init threadding variables
		self.read_flag = False
		self.data_thread = threading.Thread(target = self.gen_data, name = 'gen_data')
		self.alarm_thread = threading.Thread(target = self.alarm, name = 'alarm')
		self.data_rate_thread = threading.Thread(target = self.data_rate_tracker, name = 'data_rate')

		# others
		self.frame_counter = 0
		self.data_rate = 0

	def start(self):
		# self.data_thread.start()
		# self.alarm_thread.start()

		index = count()
		self.fig = plt.figure()
		root = tk.Tk()
		# label = tk.Label(root,text="IMU data").grid(column=0, row=1)

		control_panel = tk.Frame(master = root).pack(side = tk.RIGHT, fill = tk.BOTH, expand = True)#.grid(column=0, row=0)
		canvas = FigureCanvasTkAgg(self.fig, master=root)
		canvas.get_tk_widget().pack(side = tk.RIGHT, fill = tk.BOTH, expand = True)#.grid(column=0,row=1)

		x = np.linspace(0, 10, 50)
		self.line_1 = np.zeros(50)
		self.line_2 = np.zeros(50)
		self.line_3 = np.zeros(50)

		self.plot_dataset = self.acc_dict
		self.curr_dataset = self.ACC

		def tk_connect():
			if self.ser.is_open:
				disconnect_serial()
				return

			self.data_thread.start()
			while not self.ser.is_open:
				# print(self.data_thread.is_alive())
				if not self.data_thread.is_alive():
					self.connect_button.configure(text = "Connection FAILED\nTry Again", bg = 'LightSalmon')
					self.data_thread = threading.Thread(target = self.gen_data, name = 'gen_data')
					return
				continue
			self.data_rate_thread.start()
			self.connect_button.configure(text = "Connected\nport: " + self.ser.port + "\nbaud: " + str(self.ser.baudrate), bg = 'light green')
			self.acc_button.configure(bg = 'LightSkyBlue')

		def disconnect_serial():
			self.read_flag = False

			while self.data_thread.is_alive() or self.data_rate_thread.is_alive():
				continue

			self.data_thread = threading.Thread(target = self.gen_data, name = 'gen_data')
			self.data_rate_thread = threading.Thread(target = self.data_rate_tracker, name = 'data_rate')

			self.connect_button.configure(text = 'Disconnected', bg = 'LightSalmon')
			for i in list(self.button_dict.keys()):
				self.button_dict[i].configure(bg = 'white')
			self.acc_dict = {
				'ax': 0.0,
				'ay': 0.0,
				'az': 0.0
			}
			self.omega_dict = {
				'wx': 0.0,
				'wy': 0.0,
				'wz': 0.0
			}
			self.angle_dict = {
				'roll': 0.0,
				'pitch': 0.0,
				'yaw': 0.0
			}
			self.mag_dict = {
				'mx': 0.0,
				'my': 0.0,
				'mz': 0.0
			}
			self.temp = 0.0
			self.line_1 = np.zeros(50)
			self.line_2 = np.zeros(50)
			self.line_3 = np.zeros(50)
			return

		def tk_quit():
			self.ser.close()
			self.read_flag = False
			root.quit()
			root.destroy()

		def switch_data(sel):
			if sel == self.curr_dataset or not self.ser.is_open:
				return

			self.line_1 = np.zeros(50)
			self.line_2 = np.zeros(50)
			self.line_3 = np.zeros(50)

			self.plot_dataset = self.dict_sel[sel]
			self.button_dict[self.curr_dataset].configure(bg = 'white')
			self.button_dict[sel].configure(bg = 'LightSkyBlue')
			self.curr_dataset = sel

		def animate(i):
			var = next(index)

			if not self.ser.is_open and self.data_thread.is_alive():
				disconnect_serial()

			self.frame_counter += 1

			self.line_1[:-1] = self.line_1[1:]
			self.line_1[-1] = self.plot_dataset[list(self.plot_dataset.keys())[0]] if self.ser.is_open else 0

			self.line_2[:-1] = self.line_2[1:]
			self.line_2[-1] = self.plot_dataset[list(self.plot_dataset.keys())[1]] if self.ser.is_open else 0

			self.line_3[:-1] = self.line_3[1:]
			self.line_3[-1] = self.plot_dataset[list(self.plot_dataset.keys())[2]] if self.ser.is_open else 0

			y_lim = (np.max(np.array([np.abs(self.line_1), np.abs(self.line_2), np.abs(self.line_3)])) + 5) * 1.05

			plt.cla()
			plt.xlim(0,10)
			plt.ylim(-y_lim,y_lim)
			plt.plot(x, self.line_1, label = list(self.plot_dataset.keys())[0])
			plt.plot(x, self.line_2, label = list(self.plot_dataset.keys())[1])
			plt.plot(x, self.line_3, label = list(self.plot_dataset.keys())[2])
			plt.legend(loc = 'upper right')
			plt.title(self.units_dict[self.curr_dataset].split(' (')[0] + ' vs Time')
			plt.xlabel('time')
			plt.ylabel(self.units_dict[self.curr_dataset])
			plt.annotate(str(np.round(self.line_1[-1], 1)), xy = (x[-1], self.line_1[-1]), textcoords = 'data')
			plt.annotate(str(np.round(self.line_2[-1], 1)), xy = (x[-1], self.line_2[-1]), textcoords = 'data')
			plt.annotate(str(np.round(self.line_3[-1], 1)), xy = (x[-1], self.line_3[-1]), textcoords = 'data')
			plt.text(0.02, 0.95, 'Data Rate: ' + str(self.data_rate) + 'Hz', fontsize=12, transform=plt.gcf().transFigure)
			# plt.gcf().text()

		self.connect_button = tk.Button(master=control_panel, text="Disconnected", command = tk_connect, width = 20, bg = 'LightSalmon')
		self.connect_button.pack(side = tk.TOP, fill = tk.BOTH, expand = True)
		self.acc_button = tk.Button(master=control_panel, text="Acceleration", command = lambda: switch_data(self.ACC), width = 20)
		self.acc_button.pack(side = tk.TOP, fill = tk.BOTH, expand = True)
		self.omega_button = tk.Button(master=control_panel, text="Angular Velocity", command = lambda: switch_data(self.OMEGA), width = 20)
		self.omega_button.pack(side = tk.TOP, fill = tk.BOTH, expand = True)
		self.angle_button = tk.Button(master=control_panel, text="Angle", command = lambda: switch_data(self.ANGLE), width = 20)
		self.angle_button.pack(side = tk.TOP, fill = tk.BOTH, expand = True)
		self.mag_button = tk.Button(master=control_panel, text="Magnetic Field", command = lambda: switch_data(self.MAG), width = 20)
		self.mag_button.pack(side = tk.TOP, fill = tk.BOTH, expand = True)
		self.quit_button = tk.Button(master=control_panel, text="Quit", command = tk_quit, width = 20)
		self.quit_button.pack(side = tk.TOP, fill = tk.BOTH, expand = True)
		self.button_dict = {
			self.ACC: self.acc_button, 
			self.OMEGA: self.omega_button, 
			self.ANGLE: self.angle_button, 
			self.MAG: self.mag_button
		}

		ani = FuncAnimation(plt.gcf(), animate, frames = index, interval = 1)
		plt.tight_layout()
		tk.mainloop()
		return

	def alarm(self, t = 100):
		time.sleep(t)
		self.read_flag = False
		return
	
	def gen_data(self):
		try:
			self.ser.open()
		except:
			self.ser.close()

		if self.ser.is_open == True:
			print("ready")
		else:
			# print("failed")
			return
		self.read_flag = True
		# threading.Thread(target=alarm, name='alarm').start()
		while self.read_flag and self.ser.is_open:
#		  self.temp_data = self.ser.read()
			# print(self.read_flag, self.ser.is_open)
			try:
				if(self.ser.read().hex() == self.START):
					self.update_data(self.ser.read().hex())
			except:
				# print("except")
				continue
		# print("terminate")
		self.ser.close()

	def data_rate_tracker(self):
		while(self.ser.is_open):
			time.sleep(1)
			self.data_rate = self.frame_counter
			self.frame_counter = 0
			# self.fig.gcf().text(0.02, 0.95, 'Data Rate: ' + str(self.data_rate) + 'Hz', fontsize=12)
		
	def update_data(self, sel):
		def acc_formula(high, low):
			return np.int16((high << 8) | low) / 32768.0 * 16 * 9.8
		def omega_formula(high, low):
			return np.int16((high << 8) | low) / 32768.0 * 2000
		def angle_formula(high, low):
			return np.int16((high << 8) | low) / 32768.0 * 180
		def mag_formula(high, low):
			return np.int16((high << 8) | low)
		def temp_formula(high, low):
			return np.int16((high << 8) | low) / 100.0
		
		formula_dict = {
			self.ACC : acc_formula,
			self.OMEGA : omega_formula,
			self.ANGLE : angle_formula,
			self.MAG : mag_formula
		}
		
		dict2use = self.dict_sel[sel]
		formula2use = formula_dict[sel]
		data = self.split_data(self.ser.read(9).hex())

		for count, key in enumerate(list(dict2use.keys())):
			dict2use[key] = formula2use(data[count * 2 + 1], data[count * 2])

		print(list(dict2use.keys())[0], dict2use[list(dict2use.keys())[0]], list(dict2use.keys())[1], dict2use[list(dict2use.keys())[1]], list(dict2use.keys())[2], dict2use[list(dict2use.keys())[2]],)
		
		self.temp = temp_formula(data[7], data[6])
		
	def split_data(self, data, n = 2):
		return [int('0x' + data[i:i+n], 16) for i in range(0, len(data), n)]

if __name__ == '__main__':
	imu = ble_imu()
	imu.start()