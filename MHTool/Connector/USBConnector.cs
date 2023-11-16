using ISPCore.Connect;
using ISPCore.Packet;
using ISPCore.Util;
using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Linq;
using net.meano.Util;

namespace MHTool.Connector
{
    public class USBConnector : ConnectorBase
    {
        public enum ModeEnum
        {
            None,
            MHDisk,
            MHVolume,
        }

        public USBConnector(ConnectorType contype) : base(contype)
        {

        }

        public ModeEnum Mode { get; private set; }

        private FileStream DriveStream;

        private bool IsDriverAvaliable => (DriveStream != null && DriveStream.CanSeek && DriveStream.CanWrite && DriveStream.CanRead);

        private bool WriteDrivePacket(byte[] writeBytes)
        {
            if (!IsDriverAvaliable) return false;

            if (Mode == ModeEnum.MHDisk)
            {
                byte[] cmdWriteBytes;
                if (writeBytes[0] == 0x02 && writeBytes[1] == (byte)Header.FWData)
                {
                    SCPUPacket.Packet writePacket = SCPUPacket.PacketDecode(writeBytes);
                    WriteStream(0x10000000, writePacket.Data, writePacket.Length);
                    Logger.Debug("USB data written to 0x{0:X08}", BitConverter.ToUInt32(writePacket.Data, 0));

                    int partCmdLength = writeBytes.Length - writePacket.Length;
                    cmdWriteBytes = new byte[16 + partCmdLength];
                    Array.Copy(Encoding.ASCII.GetBytes(S031ConditionCode), 0, cmdWriteBytes, 0, 16);
                    Array.Copy(writeBytes, 0, cmdWriteBytes, 16, partCmdLength - 2);
                    Array.Copy(writeBytes, writeBytes.Length - 2, cmdWriteBytes, 14 + partCmdLength, 2);
                }
                else
                {
                    cmdWriteBytes = new byte[16 + writeBytes.Length];
                    Array.Copy(Encoding.ASCII.GetBytes(S031ConditionCode), 0, cmdWriteBytes, 0, 16);
                    Array.Copy(writeBytes, 0, cmdWriteBytes, 16, writeBytes.Length);
                }

                Logger.Debug("send: " + Hex.GetString(writeBytes, " "));
                return WriteStream(0x08000000, cmdWriteBytes, cmdWriteBytes.Length) != 0;
            }
            else if (Mode == ModeEnum.MHVolume)
            {
                Logger.Debug("send: " + Hex.GetString(writeBytes, " "));
                return WriteStream(0x00000000, writeBytes, writeBytes.Length) != 0;
            }
            return false;
        }

        protected override void WriteThread(object state)
        {
            lock (WriteTimerLock)
            {
                byte[] writeBytes = PACK.ReadyToWrite();
                if (writeBytes.Length > 0 && !WriteDrivePacket(writeBytes))
                {
                    Logger.Warn("Drive write error.");
                    Disconnect();
                }
            }
        }

        private bool ReadDrivePacket()
        {
            if (!IsDriverAvaliable) return false;

            byte[] USBReadBytes = new byte[512];
            if (Mode == ModeEnum.MHDisk)
            {
                if (ReadStream(0x08000000, USBReadBytes, 512) != 512)
                {
                    return false;
                }

            }
            else if (Mode == ModeEnum.MHVolume)
            {
                if (ReadStream(0x00000000, USBReadBytes, 512) != 512)
                {
                    return false;
                }
            }

            foreach (byte readbyte in USBReadBytes)
            {
                if (PACK.DealReadByte(readbyte)) break;
            }
            return true;
        }

        protected override void ReadThread(object state)
        {
            lock (WriteTimerLock)
            {
                if(!ReadDrivePacket())
                {
                    Disconnect();
                }
            }
        }

        public override void Connect(object device)
        {
            DriveInfo connectDrive = GetChipDriveInfo((string)device);
            DriveStream = OpenDrive(connectDrive);
            Mode = GetDriveMode(connectDrive);
            IsUserBootConnection = false;
            if (DriveStream != null && Mode != ModeEnum.None)
            {
                PACK.FlushReadBuffer();
                PACK.FlushWriteBuffer();
                CMD.ClearTask();
                WriteTimer.Change(0, 25);
                ReadTimer.Change(0, 25);
                CreateEvent(EventReason.Connected, null);
            }
            else
            {
                CreateEvent(EventReason.ConnectFailed, "Failed to open drive device! Please try reconnecting.");
                Disconnect();
            }
        }

        public override void Disconnect()
        {
            try
            {
                ChipStage = 0;
                BootVersion = string.Empty;
                ChipSN = string.Empty;
                DeviceSN = string.Empty;
                ReadTimer.Change(-1, -1);
                WriteTimer.Change(-1, -1);
                DriveStream?.Close();
                DriveStream = null;
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
            }
        }

        protected override void ReadEndCallback()
        {
            if(Mode == ModeEnum.MHDisk)
            {
                lock (WriteTimerLock)
                {
                    byte[] USBWriteBytes = new byte[512];
                    Array.Copy(Encoding.ASCII.GetBytes(S031ConditionCode), 0, USBWriteBytes, 0, 16);
                    USBWriteBytes[16] = 0;
                    WriteStream(0x08000000, USBWriteBytes, 512);
                    Thread.Sleep(15);
                }
            }
        }

        private const string S031ConditionCode = "MH1903 ROM BOOT\0";

        public const uint FILE_ATTRIBUTE_READONLY = 0x00000001;
        public const uint FILE_ATTRIBUTE_HIDDEN = 0x00000002;
        public const uint FILE_ATTRIBUTE_SYSTEM = 0x00000004;
        public const uint FILE_ATTRIBUTE_DIRECTORY = 0x00000010;
        public const uint FILE_ATTRIBUTE_ARCHIVE = 0x00000020;
        public const uint FILE_ATTRIBUTE_DEVICE = 0x00000040;
        public const uint FILE_ATTRIBUTE_NORMAL = 0x00000080;
        public const uint FILE_ATTRIBUTE_TEMPORARY = 0x00000100;
        public const uint FILE_ATTRIBUTE_SPARSE_FILE = 0x00000200;
        public const uint FILE_ATTRIBUTE_REPARSE_POINT = 0x00000400;
        public const uint FILE_ATTRIBUTE_COMPRESSED = 0x00000800;
        public const uint FILE_ATTRIBUTE_OFFLINE = 0x00001000;
        public const uint FILE_ATTRIBUTE_NOT_CONTENT_INDEXED = 0x00002000;
        public const uint FILE_ATTRIBUTE_ENCRYPTED = 0x00004000;

        public const uint GENERIC_READ = 0x80000000;
        public const uint GENERIC_WRITE = 0x40000000;
        public const uint FILE_SHARE_READ = 0x00000001;
        public const uint FILE_SHARE_WRITE = 0x00000002;

        public const uint CREATE_NEW = 1;
        public const uint CREATE_ALWAYS = 2;
        public const uint OPEN_EXISTING = 3;

        public const uint FILE_FLAG_NO_BUFFERING = 0x20000000;
        public const uint FILE_FLAG_WRITE_THROUGH = 0x80000000;

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern SafeFileHandle CreateFile(
            string lpFileName,
            uint dwDesiredAccess,
            uint dwShareMode,
            IntPtr lpSecurityAttributes,
            uint dwCreationDisposition,
            uint dwFlagsAndAttributes,
            IntPtr hTemplateFile
        );

        public int ReadStream(int seekOffset, byte[] buffer, int count)
        {
            if (DriveStream == null || !DriveStream.CanSeek)
                return 0;
            try
            {
                DriveStream.Seek(seekOffset, SeekOrigin.Begin);
                return DriveStream.Read(buffer, 0, count);
            }
            catch
            {
                return 0;
            }
        }

        public int WriteStream(int seekOffset, byte[] buffer, int count)
        {
            if (DriveStream == null || !DriveStream.CanSeek)
                return 0;
            try
            {
                DriveStream.Seek(seekOffset, SeekOrigin.Begin);
                for (int bufferOffset = 0; bufferOffset < count; bufferOffset += 512)
                {
                    byte[] blockBuffer;
                    int writeOffset = bufferOffset;
                    if (count - bufferOffset >= 512)
                        blockBuffer = buffer;
                    else
                    {
                        blockBuffer = new byte[512];
                        Array.Copy(buffer, bufferOffset, blockBuffer, 0, count - bufferOffset);
                        writeOffset = 0;
                    }
                    DriveStream.Write(blockBuffer, writeOffset, 512);
                }
                return count;
            }
            catch
            {
                return 0;
            }
        }

        public static FileStream OpenDrive(DriveInfo drive)
        {
            ModeEnum driveMode = GetDriveMode(drive);
            string handlePath;
            switch (driveMode)
            {
                case ModeEnum.MHDisk:
                    handlePath = @"\\?\" + drive.Name.Replace(@"\", "");
                    break;
                case ModeEnum.MHVolume:
                    handlePath = drive.Name + "STREAM.RAW";
                    break;
                default:
                    return null;
            }

            try
            {
                SafeFileHandle initHandle = CreateFile(
                    handlePath,
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    IntPtr.Zero,
                    OPEN_EXISTING,
                    FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING,
                    IntPtr.Zero
                );

                if (initHandle.IsInvalid)
                    return null;

                FileStream tryStream = new(initHandle, FileAccess.ReadWrite, 512);
                // FileStream tryStream = new FileStream(device, FileMode.Open, FileAccess.ReadWrite, FileShare.None, 4096, FileOptions.WriteThrough | (FileOptions)0x20000000);

                if (tryStream == null || !tryStream.CanSeek || !tryStream.CanRead || !tryStream.CanWrite)
                    return null;

                if (driveMode == ModeEnum.MHDisk)
                {
                    tryStream.Seek(0x08000000, SeekOrigin.Begin);
                    byte[] ReadHeader = new byte[512];
                    if (tryStream.Read(ReadHeader, 0, 512) == 512 && Encoding.ASCII.GetString(ReadHeader).StartsWith(S031ConditionCode))
                    {
                        return tryStream;
                    }
                    else
                    {
                        tryStream.Close();
                        return null;
                    }
                }
                else
                {
                    return tryStream;
                }
            }
            catch (Exception ex)
            {
                Logger.Debug(ex.Message);
                return null;
            }
        }

        internal void USBChangedEvent(DeviceListenerArgs e)
        {
            if (e.DeviceType == DeviceListener.DBT_DEVTYP_DEVICEINTERFACE)
                CreateEvent(EventReason.USBChanged, null);
        }

        public static DriveInfo GetChipDriveInfo(string driveName)
        {
            DriveInfo[] drives = DriveInfo.GetDrives();
            foreach(DriveInfo drive in drives)
            {
                if (drive.Name.StartsWith(driveName))
                    return drive;
            }
            return null;
        }

        public static ModeEnum GetDriveMode(DriveInfo drive)
        {
            if(drive == null)
                return ModeEnum.None;

            if (!drive.DriveType.Equals(DriveType.Removable))
                return ModeEnum.None;

            string driveFormat = "";
            string volumeLabel = "";
            try
            {
                driveFormat = drive.DriveFormat;
                volumeLabel = drive.VolumeLabel;
            }
            catch
            {
            }

            if (driveFormat == "" && volumeLabel == "")
            {
                return ModeEnum.MHDisk;
            }
            else if (driveFormat == "FAT" && volumeLabel == "MH-DiskBoot")
            {
                if (File.Exists(drive.Name + "STREAM.RAW"))
                    return ModeEnum.MHVolume;
            }
            return ModeEnum.None;
        }

        public static DriveInfo[] GetChipDrives()
        {
            List<DriveInfo> chipDriveList = new();
            foreach (DriveInfo drive in DriveInfo.GetDrives())
            {
                if (GetDriveMode(drive) != ModeEnum.None)
                {
                    chipDriveList.Add(drive);
                }
            }
            return chipDriveList.ToArray();
        }
    }
}
