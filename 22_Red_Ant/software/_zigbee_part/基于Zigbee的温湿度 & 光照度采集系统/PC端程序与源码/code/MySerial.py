#coding=utf-8

import serial
import serial.tools.list_ports

BaudrateList_g = (2400, 4800, 9600, 14400, 19200, 28800, 38400,
                  57600, 76800, 115200, 230400)

"""
    Class: Serial
"""
class MySerial(serial.Serial):
    def __init__(self):
        # baudrate list
        self.__BaudrateList = BaudrateList_g

        # Create serial
        serial.Serial.__init__(self)

    def MyOpen(self, port=None, baudrate=9600, bytesize=8, parity=serial.PARITY_NONE,
             stopbits=1, timeout=1):
        ret = ''
        try:
            self.port = port
            self.baudrate = baudrate
            self.bytesize = bytesize
            self.parity = parity
            self.stopbits = stopbits
            self.timeout = timeout
            self.open()
        except Exception as errMsg:
            ret = errMsg
        finally:
            return ret

    def MyClose(self):
        msg = 'Close Serial OK!'
        try:
            if self.isOpen():
                self.close()
        except Exception as errMsg:
            msg = errMsg
        finally:
            return msg

    def MyWrite(self, str):
        msg = 'Write OK!'
        try:
            if str != '':
                self.write(str)
        except Exception as errMsg:
            msg = errMsg
        finally:
            return msg

    def MyRead(self):
        msg = ''
        try:
            if self.isOpen():
                readBufLen = self.inWaiting()
                if readBufLen > 0:
                    msg = self.read(readBufLen)
        except Exception as errMsg:
            pass
        finally:
            return msg

    def GetCOMList(self):
        ComList = []
        PortList = list(serial.tools.list_ports.comports())
        for i in range(0, len(PortList)):
            ComList.append((list(PortList[i]))[0])
        return tuple(ComList)

    def GetBaudrateList(self):
        return self.__BaudrateList

    def GetBaudrateStringList(self):
        BaudrateStrList = []
        for n in self.__BaudrateList:
            BaudrateStrList.append(str(n))
        return tuple(BaudrateStrList)

    def GetBaudrateIndex(self, baudrate):
        if baudrate in self.__BaudrateList:
            return self.__BaudrateList.index(baudrate)
        return -1

