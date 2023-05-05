from AppUI import *
import threading
from time import ctime,sleep
import sys
import os

APPUIRunning = wx.App()
FrameRunning = AppFrame(None, -1, 'TempHumiADC  V1.0.0')
if os.path.exists('./icon/default.ico'):
    FrameRunning.SetIcon(wx.Icon('./icon/default.ico', wx.BITMAP_TYPE_ICO))
else:
    print("Icon Not FOUND!")

exitFlag = False
mutex = threading.Lock()

def main(args):
    global exitFlag
    global mutex

    while True:
        sleep(0.3)

        # UI is exit?
        mutex.acquire()
        if exitFlag:
            mutex.release()
            break
        mutex.release()

        # Try to read data
        try:
            FrameRunning.SerialRx()
            FrameRunning.ReflashCOMList()
        except Exception:
            print(sys.exc_info())
            continue
            #break

def UIRunning():
    global exitFlag
    global mutex

    # UI Running...
    FrameRunning.ShowUI()
    APPUIRunning.MainLoop()

    # Set UI exit-flag as True
    mutex.acquire()
    exitFlag = True
    mutex.release()

if __name__ == '__main__':
    t = threading.Thread(target=main, args=('main',))
    t.setDaemon(True)
    t.start()

    UIRunning()

    t.join()


