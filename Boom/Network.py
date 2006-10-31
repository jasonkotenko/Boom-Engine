#!/usr/bin/env python

"""
	Boom Network
	===========
		This module contains all the necessary low-level network interfacing. It
		handles sending/receiving, keepalive, connection establishment/destruction,
		message time-stamping, message encoding/decoding, latency control, and
		specification of reliable versus unreliable data.
	
		License
		-------
		Copyright (C) 2006 Jens Taylor

		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 2 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
"""

import Log
Log.info("Initializing network subsystem...")

import Interface

import socket
import select
import struct

"""
	This variable controls how the data is packed depending on which pre-defined
	message is created. Index 0 is reserved for the message header.
"""
NETWORK_MESSAGE_TYPES = [">HHHHH"]
NETWORK_HEADER_LENGTH = struct.calcsize(NETWORK_MESSAGE_TYPES[0])

class Message:
	"""
		The timestamp is listed as two integers: a number of seconds
			and a number of milliseconds
		The length is a simple integer of the length of the data payload
		The type is one of the predefined values for types indicating
			what the data will be interpreted as.
		The flags contain pertinent information, such as whether the data
			should be sent reliably.
	"""
	def __init__(self, time_float = 0.0, message_length = 0, 
					message_type = 0, flags = 0, data = []):
		self.time_s = int(time_float)
		self.time_ms = int((time_float - self.time_s) * 1000)
		self.length = message_length
		self.mtype = message_type
		self.flags = flags
		self.data = data

	def pack(self):
		return self.pack_header() + self.pack_data()

	def unpack(self, payload):
		head = self.unpack_header(payload[:NETWORK_HEADER_LENGTH])
		self.time_s, self.time_ms, self.length, self.mtype, self.flags = head
		self.data = list(self.unpack_data(payload[NETWORK_HEADER_LENGTH:])

	def pack_header(self):
		return struct.pack(NETWORK_MESSAGE_TYPES[0], self.time_s, self.time_ms,
							self.length, self.mtype, self.flags)

	def unpack_header(self, header):
		return struct.unpack(NETWORK_MESSAGE_TYPES[0], header)

	def pack_data(self):
		return struct.pack(NETWORK_MESSAGE_TYPES[self.mtype], *data)

	def unpack_data(self, d):
		return struct.unpack(NETWORK_MESSAGE_TYPES[self.mtype], d)

	def time_float(self, ts = None, tms = 0):
		if not ts:
			ts = self.time_s
			tms = self.time_ms
		return ts + (tms / 1000)

	def time_int(self, tf = None):
		if not tf:
			ret_ints = [self.time_s, self.time_ms]
		else:
			ts = int(tf)
			ret_ints = [ts, int((tf - ts) / 1000)]

class MessageBuffer:
	def __init__(self):
		self.pos = 0
		self.data = []

	def write(self, data):
		"""
			data is of Message type
		"""
		self.data.append(data.pack())
		return true

	def read(self):
		"""
			Read the next chunk of data and return a Message
		"""
		#unpacked = struct.unpack(NETWORK_MESSAGE_TYPES[0], 
		#							self.data[self.pos][:NETWORK_HEADER_LENGTH])
		#message = Message(unpacked*)
		message = Message()
		message.unpack(self.data[self.pos])
		del self.data[self.pos]
		return message

	def flush(self):
		"""
			Clears the entire buffer returning all the raw network data
		"""
		total = ""
		for mess in self.data:
			total += mess
		self.clear()
		return total

	def clear(self):
		"""
			Simply clears all the buffer data
		"""
		self.data = []
		return true

class Network:
	def __init__(self):
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		# Option to allow broadcast messages to be sent.
		self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

	def send(to, message):
		pass

	def recv(from = None):
		pass
