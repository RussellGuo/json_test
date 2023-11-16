using MHTool.UI;
using net.meano.Util;
using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;

namespace MHTool.Setting
{
    public class AutoTaskSetting : Setting
    {

        public bool IsEnable;

        public bool IsRunning => IsEnable && AutoCurrent.Status == AutoTaskStatuEnum.Running;

        public DispatcherTimer AutoTaskTimer;
        public MainWindow MW;
        public AutoTaskSetting(MainWindow m, ConfigNode config) : base(config)
        {
            MW = m;
            AutoTaskTimer = new DispatcherTimer();
            AutoTaskTimer.Interval = TimeSpan.FromMilliseconds(100);
            AutoTaskTimer.Tick += new EventHandler(AutoTaskTimerThread);

            AutoReConnect = Config.ConfiguredGetBoolean(nameof(AutoReConnect), true);
            AutoDeviceUpdate = Config.ConfiguredGetBoolean(nameof(AutoDeviceUpdate), false);
            AutoDownload = Config.ConfiguredGetBoolean(nameof(AutoDownload), true);
            AutoWaitTimeInt = Config.ConfiguredGetInt(nameof(AutoWaitTimeInt), 10);
        }

        public void TaskTimerStart(string autoDevice)
        {
            AutoDevice = autoDevice;
            if (AutoDevice != null)
                Logger.Info("将自动连接设备: " + AutoDevice);
            IsEnable = true;
            AutoCurrent.Task = AutoTaskEnum.Connect;
            AutoCurrent.Status = AutoTaskStatuEnum.Start;
            AutoTaskTimer.Start();
        }

        private string AutoDevice;

        public void TaskTimerStop()
        {
            AutoCurrent.Task = AutoTaskEnum.TaskEnd;
        }

        public bool AutoDeviceUpdate
        {
            get => isDeviceUpdateNeeded;
            set
            {
                isDeviceUpdateNeeded = value;
                Update(nameof(AutoDeviceUpdate), isDeviceUpdateNeeded);
            }
        }
        private bool isDeviceUpdateNeeded = false;

        public bool AutoDownload
        {
            get => isDownloadNeeded;
            set
            {
                isDownloadNeeded = value;
                Update(nameof(AutoDownload), isDownloadNeeded);
            }
        }
        private bool isDownloadNeeded = true;

        public bool AutoReConnect
        {
            get => isAutoReConnect;
            set
            {
                isAutoReConnect = value;
                Update(nameof(AutoReConnect), isAutoReConnect);
            }
        }
        private bool isAutoReConnect = true;

        public string AutoWaitTime
        {
            get => AutoWaitTimeInt.ToString() + Logger.T(" s");
            set => Update(nameof(AutoWaitTime));
        }

        public int AutoWaitTimeInt
        {
            get => autoWaitTimeInt;
            set
            {
                autoWaitTimeInt = value;
                Update(nameof(AutoWaitTimeInt), autoWaitTimeInt);
                Update(nameof(AutoWaitTime));
            }
        }
        private int autoWaitTimeInt = 10;

        public void AutoTaskTimerThread(object sender, EventArgs es)
        {
            switch (AutoCurrent.Task)
            {
                case AutoTaskEnum.Connect:
                    if (AutoCurrent.Status == AutoTaskStatuEnum.Start)
                    {
                        if (MW.Connector == null & MW.ChipConnectButtonLabel.Content == MW.Lang["ui.Connect.ConnectButton.Connect"])
                        {
                            if (AutoDevice != null)
                            {
                                MW.PortList.Text = AutoDevice;
                                if (MW.PortList.Text.Trim() != AutoDevice)
                                {
                                    AutoCurrent.Status = AutoTaskStatuEnum.Failed;
                                    Logger.Warn("指定自动连接的设备 " + AutoDevice + " 还未连接到主机.");
                                    return;
                                }
                            }

                            Logger.Info("Connecting chip automatically!");
                            AutoCurrent.Status = AutoTaskStatuEnum.Running;
                            MW.ChipConnectButton_Click(null, null);
                        }
                        else
                        {
                            Logger.Info("The chip has been connected, need to disconnect!");
                            if (MW.Connector != null)
                            {
                                MW.Connector.Disconnect();
                                return;
                            }
                            else
                            {
                                MW.ChipConnectButtonLabel.SetResourceReference(ContentControl.ContentProperty, "ui.Connect.ConnectButton.Connect");
                            }

                        }
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Succeed)
                    {
                        Logger.Info("Connect successfully, go to the next step!");
                        AutoCurrent.NextTask();
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Failed)
                    {
                        if (isAutoReConnect || MessageBox.Show(MW, Logger.T("Connection failed. Do you want to reconnect?"), "警告", MessageBoxButton.YesNo) == MessageBoxResult.Yes)
                        {
                            Logger.Info("Reconnecting...");
                            AutoCurrent.Status = AutoTaskStatuEnum.Start;
                        }
                        else
                        {
                            Logger.Info("Give up reconnection!");
                            AutoCurrent.Task = AutoTaskEnum.TaskEnd;
                        }
                    }
                    else
                    {
                        // Running
                    }
                    break;
                case AutoTaskEnum.WaferUpdate:
                case AutoTaskEnum.ChipUpdate:
                    AutoCurrent.NextTask();
                    break;
                case AutoTaskEnum.DeviceUpdate:
                    if (AutoCurrent.Status == AutoTaskStatuEnum.Start)
                    {
                        if (!isDeviceUpdateNeeded)
                        {
                            AutoCurrent.NextTask();
                            break;
                        }
                        if (MW.ConnectedChip?.Stage == ISPCore.Chip.StageEnum.Secure)
                        {
                            Logger.Warn("The chip is already in the security stage!");
                            AutoCurrent.NextTask();
                            break;
                        }
                        if (!MW.DeviceUpdate())
                        {
                            TaskTimerStop();
                            break;
                        }
                        AutoCurrent.Status = AutoTaskStatuEnum.Running;
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Succeed)
                    {
                        Logger.Info("Auto device update {0}!", Logger.T("succeeded"));
                        AutoCurrent.NextTask();
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Failed)
                    {
                        string failMessage = string.Format(Logger.T("Auto device update {0}!"), Logger.T("failed"));
                        Logger.Info(failMessage);
                        MessageBox.Show(MW, failMessage, Logger.T("failed"));
                        TaskTimerStop();
                    }
                    break;
                case AutoTaskEnum.Download:
                    if (AutoCurrent.Status == AutoTaskStatuEnum.Start)
                    {
                        if (!isDownloadNeeded)
                        {
                            AutoCurrent.NextTask();
                            break;
                        }
                        // TODO
                        if (!MW.FirmwareOption.Firmware.IsHeaderValid)
                        {
                            Logger.Error("固件未准备好，任务中断！");
                            TaskTimerStop();
                        }
                        else if (MW.Connector != null && MW.Connector.CanSendCommand)
                        {
                            MW.DownloadButton_Click(null, null);
                            AutoCurrent.Status = AutoTaskStatuEnum.Running;
                        }
                        else
                        {
                            Logger.Error("连接发生问题，任务中断！");
                            TaskTimerStop();
                        }
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Succeed)
                    {
                        Logger.Info("Auto download {0}!", Logger.T("succeeded"));
                        AutoCurrent.NextTask();
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Failed)
                    {
                        string failMessage = string.Format(Logger.T("Auto download {0}!"), Logger.T("failed"));
                        Logger.Info(failMessage);
                        MessageBox.Show(MW, failMessage, Logger.T("failed"));
                        TaskTimerStop();
                    }
                    break;
                case AutoTaskEnum.Wait:
                    if (AutoCurrent.Status == AutoTaskStatuEnum.Start)
                    {
                        Logger.Info("等待" + AutoWaitTimeInt + "秒钟，" + AutoWaitTimeInt + "秒后将进行下一次任务连接！");
                        AutoCurrent.Status = AutoTaskStatuEnum.Running;
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Succeed)
                    {
                        AutoCurrent.NextTask();
                    }
                    else if (AutoCurrent.Status == AutoTaskStatuEnum.Failed)
                    {
                        // Never Failed
                    }
                    else
                    {
                        // Running
                        if (AutoCurrent.TimeCount >= (2 + AutoWaitTimeInt * 10))
                        {
                            AutoCurrent.Status = AutoTaskStatuEnum.Succeed;
                        }
                        AutoCurrent.TimeCount++;
                    }
                    break;
                case AutoTaskEnum.TaskEnd:
                    if (MW.ChipConnectButtonLabel.Content != MW.Lang["ui.Connect.ConnectButton.Connect"])
                    {
                        MW.ChipConnectButton_Click(null, null);
                    }
                    AutoTaskTimer.Stop();
                    MW.AutoTaskButton.SetResourceReference(ContentControl.ContentProperty, "ui.AutoTask.AutoButton.Start");
                    Logger.Info("自动任务已停止！");
                    IsEnable = false;
                    break;
            }
        }

        public AutoTaskStatu AutoCurrent = new();

        public class AutoTaskStatu
        {
            public AutoTaskEnum Task;
            public AutoTaskStatuEnum Status;
            public int TimeCount;
            public AutoTaskStatu()
            {
                Task = AutoTaskEnum.Connect;
                Status = AutoTaskStatuEnum.Start;
                TimeCount = 0;
            }
            public void NextTask()
            {
                Task = (++Task) == AutoTaskEnum.TaskEnd ? AutoTaskEnum.Connect : Task;
                Status = AutoTaskStatuEnum.Start;
                TimeCount = 0;
            }

            public void Succeed()
            {
                Status = AutoTaskStatuEnum.Succeed;
            }
            public void Failed()
            {
                Status = AutoTaskStatuEnum.Failed;
            }
        }

        public enum AutoTaskEnum
        {
            Connect = 0,
            WaferUpdate = 1,
            ChipUpdate = 2,
            DeviceUpdate = 3,
            Download = 4,
            Wait,
            TaskEnd,
            TaskCount
        }

        public enum AutoTaskStatuEnum
        {
            Start,
            Running,
            Succeed,
            Failed
        }
    }
}
