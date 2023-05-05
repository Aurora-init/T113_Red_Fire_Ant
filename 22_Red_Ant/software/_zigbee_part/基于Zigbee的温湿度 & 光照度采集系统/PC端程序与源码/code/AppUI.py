#coding=utf-8
from MySerial import *
import wx
from wx.lib.embeddedimage import PyEmbeddedImage

"""
    Class: AppFrame
"""
class AppFrame(wx.Frame):
    def __init__(self, parent, id, title):
        # Init Frame
        wx.Frame.__init__(self, parent, id, title, size=wx.Size(570, 310),
                style=wx.DEFAULT_FRAME_STYLE ^ (wx.RESIZE_BORDER | wx.MAXIMIZE_BOX))

        # Init Serial
        self.serialFD = MySerial()

        # font
        fontList = wx.FontEnumerator().GetFacenames()
        if u'微软雅黑' in fontList:
            self.font = wx.Font(11, wx.DEFAULT, wx.NORMAL, wx.NORMAL, faceName=u'微软雅黑')
            self.font13 = wx.Font(13, wx.DEFAULT, wx.NORMAL, wx.NORMAL, faceName=u'微软雅黑')
        else:
            self.font = wx.Font(11, wx.DEFAULT, wx.NORMAL, wx.NORMAL)

        # menu
        self.menubar = wx.MenuBar()

        # status bar
        self.statBar = self.CreateStatusBar()
        self.statBar.SetStatusText('Welcome!')
        self.statBar.SetBackgroundColour('LIGHT GREY')

        # Panel
        self.panel = wx.Panel(self)
        self.panel.SetBackgroundColour('LIGHT GREY')

        # Init UI
        self.__InitUI()

        pass

    def __del__(self):
        pass

    def __InitUI(self):

        # Menu
        help = wx.Menu()
        help.Append(300, '&About', 'About')

        self.menubar.Append(help, '&Help')
        self.SetMenuBar(self.menubar)

        # Static box
        SerialCfgSB = wx.StaticBox(self, label=u'串口配置')
        SerialCfgSB.SetFont(self.font)
        SerialCfgSB.SetBackgroundColour('LIGHT GREY')
        SerialCfgSB.SetForegroundColour('red')

        # COM
        COMST = wx.StaticText(self.panel, label=u'端    口 ：')
        COMST.SetFont(self.font)
        self.COMCB = wx.ComboBox(self.panel, id = 80, size=(80, 25), style=wx.CB_READONLY)

        # baudrate
        baudrateST = wx.StaticText(self.panel, label=u'波特率 ：')
        baudrateST.SetFont(self.font)
        self.baudrateCB = wx.ComboBox(self.panel, id = 81, size=(81, 25), style=wx.CB_READONLY)

        # databits, parity, stopbits, flow
        DPSF = u'数据位 ： 8 \n校验位 ： None \n停止位 ： 1 \n流    控 ： None'
        DPSFTest = wx.StaticText(self.panel, label=DPSF)
        DPSFTest.SetFont(self.font)

        # button - connection
        self.SerialConnButton = wx.Button(self.panel, id=1, label=u'连 接')
        self.SerialConnButton.SetFont(self.font13)
        self.SerialConnButton.SetForegroundColour('blue')

        # Picture
        self.images = ['temp.png', 'humi.png', 'light.png']

        self.tempPH = wx.StaticBitmap(self.panel)
        self.humiPH = wx.StaticBitmap(self.panel)
        self.lightPH = wx.StaticBitmap(self.panel)

        self.tempPH.SetBitmap(wx.Bitmap('./ph/' + self.images[0]))
        self.humiPH.SetBitmap(wx.Bitmap('./ph/' + self.images[1]))
        self.lightPH.SetBitmap(wx.Bitmap('./ph/' + self.images[2]))

        # Show Test
        tempL = u'温度：'
        self.tempT = wx.StaticText(self.panel, label=tempL)
        self.tempT.SetFont(self.font)

        humiL = u'湿度：'
        self.humiT = wx.StaticText(self.panel, label=humiL)
        self.humiT.SetFont(self.font)

        lightL = u'光照强度：'
        self.lightT = wx.StaticText(self.panel, label=lightL)
        self.lightT.SetFont(self.font)

        self.tempTest = wx.TextCtrl(self.panel, -1, style=wx.TE_READONLY)
        self.tempTest.SetBackgroundColour('THISTLE')
        self.tempTest.SetFont(self.font)
        self.tempTest.SetSize(wx.Size(70, 25))

        self.humiTest = wx.TextCtrl(self.panel, -1, style=wx.TE_READONLY)
        self.humiTest.SetBackgroundColour('DARK TURQUOISE')
        self.humiTest.SetFont(self.font)
        self.humiTest.SetSize(wx.Size(70, 25))

        self.lightTest = wx.TextCtrl(self.panel, -1, style=wx.TE_READONLY)
        self.lightTest.SetBackgroundColour('GOLDENROD')
        self.lightTest.SetFont(self.font)
        self.lightTest.SetSize(wx.Size(80, 25))

        ##########
        # Layout #
        ##########
        """自动布局法：
        """

        #"""  手动布局法：
        SerialCfgSB.SetPosition(wx.Point(5, 5))
        SerialCfgSB.SetSize(wx.Size(165, 230))

        COMST.SetPosition(wx.Point(15, 30))
        self.COMCB.SetSize(wx.Size(80, 25))
        self.COMCB.SetPosition(wx.Point(84, 30))

        baudrateST.SetPosition(wx.Point(15, 62))
        self.baudrateCB.SetSize(wx.Size(80, 25))
        self.baudrateCB.SetPosition(wx.Point(84, 60))

        DPSFTest.SetPosition(wx.Point(15, 100))

        self.SerialConnButton.SetSize(wx.Size(120, 40))
        self.SerialConnButton.SetPosition(wx.Point(10, 190))

        self.tempPH.SetPosition(wx.Point(180, 18))
        self.humiPH.SetPosition(wx.Point(180, 138))
        self.lightPH.SetPosition(wx.Point(375, 80))

        self.tempT.SetPosition(wx.Point(275, 45))
        self.tempTest.SetPosition(wx.Point(275, 70))

        self.humiT.SetPosition(wx.Point(275, 170))
        self.humiTest.SetPosition(wx.Point(275, 195))

        self.lightT.SetPosition(wx.Point(482, 110))
        self.lightTest.SetPosition(wx.Point(482, 135))

        #"""

        ###########
        # Setting #
        ###########
        self.__InitSetting()

    def __InitSetting(self):
        # COM
        self.COMList = self.serialFD.GetCOMList()
        self.COMCB.SetItems(self.COMList)
        self.COMCB.SetSelection(0)
        self.ComListID = 0

        # Baudrate
        self.baudrateCB.Append(self.serialFD.GetBaudrateStringList())
        self.baudrateCB.SetSelection(self.serialFD.GetBaudrateIndex(9600))
        self.BDListID = self.baudrateCB.GetCurrentSelection()

        # Bind
        self.COMCB.Bind(wx.EVT_COMBOBOX, self.__ComListSelection, id=80)
        self.baudrateCB.Bind(wx.EVT_COMBOBOX, self.__BDSelection, id=81)
        self.SerialConnButton.Bind(wx.EVT_BUTTON, self.__SerialConnButtonHandler, id=1)
        self.Bind(wx.EVT_TOOL, self.__HELP, id=300)

    def ReflashCOMList(self):
        newCOMList = self.serialFD.GetCOMList()
        currentSel = self.COMCB.GetCurrentSelection()
        selStr = ''
        if not currentSel < 0:
            selStr = self.COMList[currentSel]

        if newCOMList != self.COMList:
            self.COMList = newCOMList
            self.COMCB.SetItems(self.COMList)
            if self.SerialConnButton.GetLabel() == u'连 接':
                self.COMCB.SetSelection(0)
            elif selStr in self.COMList:
                self.COMCB.SetStringSelection(selStr)
            else:
                self.__SerialConnButtonHandler(None)
                self.COMCB.SetSelection(0)

    def __ComListSelection(self, evt):
        if self.SerialConnButton.GetLabel() == u'连 接':
            pass
        elif self.ComListID != self.COMCB.GetCurrentSelection():
            self.statBar.SetStatusText('更改COM口需要断开串口连接后重新连接才能生效！')

        self.ComListID = self.COMCB.GetCurrentSelection()

    def __BDSelection(self, evt):
        if self.SerialConnButton.GetLabel() == u'连 接':
            pass
        elif self.BDListID != self.baudrateCB.GetCurrentSelection():
            self.statBar.SetStatusText('更改波特率需要断开串口连接后重新连接才能生效！')

        self.BDListID = self.baudrateCB.GetCurrentSelection()

    def __SerialConnButtonHandler(self, evt):
        if self.SerialConnButton.GetLabel() == u'连 接':
            # COM Current Selection
            if self.COMCB.GetCurrentSelection() < 0:
                self.statBar.SetStatusText('未选中串口！')
                return False

            retMsg = self.serialFD.MyOpen(port=self.COMCB.GetValue(),
                                          baudrate=int(self.baudrateCB.GetValue()))
            if retMsg == '':
                self.statBar.SetStatusText('串口已打开！')
                self.SerialConnButton.SetLabel(u'断开连接')
                self.SerialConnButton.SetForegroundColour('red')
            else:
                self.statBar.SetStatusText('连接失败：'+str(retMsg))
        else:
            # Close
            self.serialFD.MyClose()
            self.statBar.SetStatusText('串口已关闭！')
            self.SerialConnButton.SetLabel(u'连 接')
            self.SerialConnButton.SetForegroundColour('blue')

    def __HELP(self, evt):
        dlg = wx.MessageDialog(None, u"温湿度及光照强度采集!", u"Help", wx.YES_NO | wx.ICON_QUESTION)
        if dlg.ShowModal() == wx.ID_YES:
            pass
        dlg.Destroy()

    def SerialRx(self):
        rx = self.serialFD.MyRead()
        if rx != '':
            rxstr = str(rx)
            # Temp
            t = rxstr.find('T')
            if t != -1:
                h = rxstr.find('H')
                if h != -1:
                    sub_str = rxstr[t+1:h]
                    self.tempTest.Clear()
                    self.tempTest.WriteText(sub_str + u' ℃')
            # Humi
            h = rxstr.find('H')
            if h != -1:
                l = rxstr.find('L')
                if l != -1:
                    sub_str = rxstr[h+1:l]
                    self.humiTest.Clear()
                    self.humiTest.WriteText(sub_str + ' %')
            # Light
            l = rxstr.find('L')
            if l != -1:
                e = rxstr.find('#')
                if e != -1:
                    sub_str = rxstr[l+1:e]
                    self.lightTest.Clear()
                    self.lightTest.WriteText(sub_str)

    def ShowUI(self):
        self.Center()
        self.Show()
        return True