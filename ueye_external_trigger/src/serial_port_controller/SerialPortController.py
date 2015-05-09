#!/usr/bin/env python
"""
File: SerialPortController.py
Author: Yifei Zhang
Email: njzhangyifei@gmail.com
Github: https://github.com/njzhangyifei
Description: theaded approach for seiral port monitor
"""

import threading
import serial
import Queue

class SerialPortController(threading.Thread):
    """SerialPortController
    """
    def __init__(self, \
            data_q, error_q, \
            port_name, port_baud, \
            port_stopbits=serial.STOPBITS_ONE, \
            port_parity=serial.PARITY_NONE, \
            port_timeout=0.1):
        threading.Thread.__init__(self)
        self._serial_port = None
        self._serial_arg = dict( \
                port=port_name, \
                baudrate=port_baud, \
                stopbits=port_stopbits, \
                parity=port_parity, \
                timeout=port_timeout)
        self._data_q = data_q
        self._error_q = error_q
        self._frame_rate = None
        self._current_index = 0xFF
        self._alive = threading.Event()
        self._alive.set()

    def run(self):
        """run
        """
        try:
            if self._serial_port:
                self._serial_port.close()
            self._serial_port = serial.Serial(**self._serial_arg)
        except serial.SerialException, error:
            self._error_q.put(error.message)
            return
        while self._alive.isSet():
            data = ''
            if self._serial_port.inWaiting():
                data = self._serial_port.readline()
                data = data.rstrip('\r\n')
            if not data == '':
                self._data_q.put(data)
        if self._serial_port:
            self._serial_port.close()

    def set_frame_rate(self, frame_rate):
        """set the frame rate by writing to serial port
        :frame_rate : int, the frame rate to be set to
        """
        if self._alive.isSet() and self._serial_port:
            data = bytearray([0xA0, self.generate_index(), frame_rate])
            chksum(data)
            self._serial_port.flushInput()
            self._serial_port.write(data)
            with self._data_q.mutex:
                self._data_q.queue.clear()
                self._serial_port.flushInput()

    def generate_index(self):
        """generate_index.
        :returns: generate index of the data pack
        """
        if self._current_index < 0xFF:
            self._current_index += 1
        elif self._current_index == 0xFF:
            self._current_index = 0xF0
        return self._current_index

    def join(self, timeout=None):
        self._alive.clear()

        threading.Thread.join(self, timeout)

def chksum(data):
    """chksum
    @type data: bytearray
    """
    data_chksum = 0
    for element in data:
        data_chksum ^= element
    data.append(data_chksum)

