import asyncio
import struct
import numpy as np
import threading
from time import sleep
import tkinter as tk
from bleak import *

chars_uuid = {
	'2713': 'imu',
	'2714': 'mic',
	'2715': 'tmp',
	'2716': 'clr',
}

global notification_en
notification_en = 0

global mic_buf, imu_buf, clr_buf
mic_buf= []

imu_buf = {
	'a_xyz' : []
	#'ax' : [],
	#'ay' : [],
	#'az' : [],
	'wx' : [],
	'wy' : [],
	'wz' : [],
	'mx' : [],
	'my' : [],
	'mz' : [],
}

global color_label

def IMU_handler(name, data):
	# print(name, struct.unpack('<f', data)[0])
	global imu_buf
	# print(struct.unpack('<'+'f'*9, data))
	# print(len(data))
	new_data = list(np.frombuffer(data, dtype=float)) #struct.unpack('<'+'f'*9*6, data)
	# for i, key in enumerate(list(imu_buf.keys())[:3]):
	# 	imu_buf[key].extend(new_data[i::3])
	imu_buf['a_xyz'].extend(new_data)
	# print(imu_buf['ax'][-1])

def CLR_handler(name, data):
	global clr_buf
	# print(name, np.frombuffer(data, dtype=np.float32).astype(int))
	config_gui_color(np.frombuffer(data, dtype=np.float32).astype(int)[2:5])
	# print(np.frombuffer(data, dtype=np.float32).astype(int)[2:5])
	# print(struct.unpack('<i', data))
	# clr_buf.append(struct.unpack('<f', data)[0])

def MIC_handler(name, data):
	# print(name, len(np.frombuffer(data, dtype=np.int16)))
	global mic_buf
	# mic_buf.append(np.frombuffer(data, dtype=np.int16)[0])
	# print(len(data))
	mic_buf = list(mic_buf)
	mic_buf += list(np.frombuffer(data, dtype=np.int16))

dispatcher = {
	'imu': IMU_handler,
	'mic': MIC_handler,
	'tmp': IMU_handler,
	'clr': CLR_handler
}

global handler_name2handle

class ble_itf:
	def __init__(self, name, uuid2name):
		self.name = name
		self.uuid2name = uuid2name
		self.name2handle = dict()
		self.device = None
		self.device_list = None
		self.client = None
		
		global handler_name2handle
		handler_name2handle = self.name2handle
		
	def notification_dispatcher(sender, data):
		def get_char_name(handle):
			global handler_name2handle
			for name, item in handler_name2handle.items():
				if item[0] == handle:
					return name

		char_name = get_char_name(sender)
		dispatcher[char_name](char_name, data)
		
		
	async def __discover_devices(self):
		print('Looking for devices...')
		self.device_list = await BleakScanner.discover()
		return self.device_list
	
	async def connect(self):
		print('Initializing...')
		await self.__discover_devices()
		for device in self.device_list:
			if device.name == self.name:
				print('Found ', self.name, ' at address', device.address)
				self.device = device
				break
		if self.device:
			self.client = BleakClient(self.device.address)
			print('Connecting...')
			await self.client.connect()
			print('Connected!')
			self.__setup_characteristics()
		return self.is_connected()
	
	async def disconnect(self):
		print('Disconnecting...')
		await self.client.disconnect()
		print('Disconnected!')
		return self.is_connected()
	
	def is_connected(self):
		return self.client.is_connected
	
	def __setup_characteristics(self):
		for char in self.client.services.services[10].characteristics:
			for key in self.uuid2name:
				if key in char.uuid:
					self.name2handle[self.uuid2name[key]] = (char.handle, char)
					
	def get_char_obj(self, char_name):
		return self.name2handle[char_name][1]
	
	def get_char_uuid(self, char_name):
		return self.name2handle[char_name][1].uuid
					
	async def start_notification(self, char_name, handler = notification_dispatcher):
		await self.client.start_notify(self.get_char_uuid(char_name), handler)
		global notification_en
		notification_en = 1

	async def read_from_char(self, char_name):
		return await self.client.read_gatt_char(self.get_char_uuid(char_name))
		
	async def stop_notification(self, char_name):
		await self.client.stop_notify(self.get_char_uuid(char_name))
		global notification_en
		notification_en = 0
		
	async def start_all_notification(self):
		for name in name2handle:
			await self.start_notification(name)
			
	async def stopt_all_notification(self):
		for name in name2handle:
			await self.stop_notification(name)

def data_rate_checker():
	global notification_en
	global imu_buf, mic_buf
	while True:
		if notification_en:
			print('notification enabled!')
			while notification_en:
				imu_buf, mic_buf = [], []
				sleep(10)
				print('imu_datarate: ', len(imu_buf) / 5, ' Hz')
				print('mic_datarate: ', len(mic_buf) / 5, ' Hz')
			break

async def main():
	ble = ble_itf('NANO_IMU', chars_uuid)
	await ble.connect()
	await ble.start_notification('mic')
	await ble.start_notification('imu')
	# while(True):
	# 	print(await ble.read_from_char('mic'))
	await asyncio.sleep(20.0)
	await ble.stop_notification('mic')
	await ble.stop_notification('imu')
	await ble.disconnect()

def gui():
    global color_label
    window = tk.Tk()
    color_frame = tk.Frame(master = window, borderwidth = 0, bg = 'red')
    color_frame.pack(fill=tk.BOTH, side=tk.LEFT, expand=True)
    color_label = tk.Label(master = color_frame, text = 'color', font = ("courier new", 10), width = 50, bg = 'red', anchor = "w")
    color_label.pack(fill=tk.BOTH, side=tk.LEFT, expand=True)
    window.mainloop()

def config_gui_color(color_arr):
	if len(color_arr) != 3:
		return

	global color_label
	color_str = "#"
	for i in color_arr:
		color_str += hex(i)[-2:]
	color_label.config(bg = color_str)

async def call():
	await main()

# async def call():
if __name__ == '__main__':
	# data_rate_thread = threading.Thread(target = data_rate_checker, name = 'datarate')
	# data_rate_thread.start()
	# gui_thread = threading.Thread(target = gui, name = 'gui_thread')
	# gui_thread.start()
	# await main()
	
	loop = asyncio.get_event_loop()
	loop.run_until_complete(main())
	print(len(mic_buf) / 10)

	# data_rate_thread.join()
	# gui_thread.join()