using ISPCore.Connect;
using ISPCore.Packet;
using ISPCore.Util;
using net.meano.Util;
using System;
using System.IO.Ports;

namespace MHTool.Connector
{
    public class SerialConnector : ConnectorBase
    {
        // 私有方法及对象
        public SerialPort serialport;
        public bool IsLegacyVCP { get; private set; } = false;

        public SerialConnector(ConnectorType type)
            : base(type)
        {
        }

        public byte[] ReadBuffer = new byte[4096];
        // 串口读取线程
        private void SerialReadThread(object sender, SerialDataReceivedEventArgs e)
        {
            // 串口读取事件 每接收一个字节触发一次
            try
            {
                int ReadCount = serialport.BytesToRead;
                ReadCount = Math.Min(ReadCount, ReadBuffer.Length);
                serialport.Read(ReadBuffer, 0, ReadCount);
                for (int i = 0; i < ReadCount; i++)
                {
                    if (PACK.DealReadByte(ReadBuffer[i]))
                    {
                        PacketTimerThread(this);
                    }
                }
            }
            catch (Exception ex)
            {
                Logger.Error(ex.Message);
            }
        }

        // 串口发送线程
        protected override void WriteThread(object state)
        {
            lock (WriteTimerLock)
            {
                byte[] WriteBytes = PACK.ReadyToWrite();
                try
                {
                    if (serialport != null && serialport.IsOpen && WriteBytes.Length > 0)
                    {
                        serialport.Write(WriteBytes, 0, WriteBytes.Length);
                        Logger.Debug("send: " + Hex.GetString(WriteBytes, " "));
                    }
                }
                catch(Exception ex)
                {
                    Logger.Error(ex.Message);
                    Disconnect();
                }
            }
        }

        protected override void ReadThread(object state)
        {
            ;
        }

        public const string MH_USB_ID = "USB\\VID_0103&PID_6061\\00000000050C";

        public override void Connect(object device)
        {
            if (device is not SerialPort serial)
            {
                CreateEvent(EventReason.ConnectFailed, "Device type error!");
                return;
            }

            IsUserBootConnection = false;

            IsLegacyVCP = false;
            serialport = serial;
            DeviceInfo SerialInfo = DeviceInfo.GetSerialInfo(serialport.PortName);
            if (
                SerialInfo != null &&
                SerialInfo.Name.Equals(serialport.PortName) &&
                SerialInfo.ID.Equals(MH_USB_ID) &&
                (SerialInfo.BusDescription?.Equals("MEGAHUNT1901 Virtual ComPort in FS Mode") == true ||
                SerialInfo.BusDescription?.Equals("MEGAHUNT1902 Boot Download in FS Mode") == true)
            )
            {
                Logger.Info("The connection is base on VCP protcol");
                IsLegacyVCP = true;
            }
            try
            {
                serialport.DtrEnable = true;
                serialport.DataBits = 8;
                serialport.StopBits = StopBits.One;
                serialport.Parity = Parity.None;
                serialport.ReadTimeout = 50;
                serialport.ReceivedBytesThreshold = 1;
                serialport.DataReceived +=
                    new SerialDataReceivedEventHandler(SerialReadThread);
                PACK.FlushReadBuffer();
                PACK.FlushWriteBuffer();
                CMD.ClearTask();
                serialport.Open();
                WriteTimer.Change(15, 25);
                ReadTimer.Change(0, 25);
                Logger.Info("Connecting, please reset device");
                CreateEvent(EventReason.Connected, null);
                return;
            }
            catch (Exception ex)
            {
                Disconnect();
                CreateEvent(EventReason.ConnectFailed, ex.Message);
            }
        }

        public override void Disconnect() {
            try
            {
                ChipStage = 0;
                BootVersion = string.Empty;
                ChipSN = string.Empty;
                DeviceSN = string.Empty;
                ReadTimer.Change(-1, -1);
                WriteTimer.Change(-1, -1);
                serialport.Close();
                CreateEvent(EventReason.Disconnected, null);
            }
            catch (Exception ex)
            {
                CreateEvent(EventReason.Disconnected, ex.Message);
            }
            finally
            {
                PACK.FlushReadBuffer();
                PACK.FlushWriteBuffer();
                CMD.ClearTask();
                serialport = null;
            }
        }

        internal void SerialChangedEvent(DeviceListenerArgs e)
        {
            if(e.DeviceType == DeviceListener.DBT_DEVTYP_PORT)
            {
                if(e.DeviceAction == DeviceListener.DBT_DEVICEREMOVECOMPLETE && serialport!=null && !serialport.IsOpen)
                {
                    Logger.Warn("The serial port disconnected! Chip connection is about to close!");
                    Disconnect();
                }
            }
            CreateEvent(EventReason.SerialChanged, null);
        }
    }
}