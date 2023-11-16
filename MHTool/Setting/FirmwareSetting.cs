using Cryptography.Encryption;
using ISPCore.FileResolver;
using ISPCore.Packet;
using ISPCore.Util;
using net.meano.Util;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Security.Cryptography;
using System.Timers;
using System.Windows;

namespace MHTool.Setting
{
    public class FirmwareSetting : Setting
    {
        public FirmwareSetting(ConfigNode config) : base(config)
        {
            FirmwareWatcher = new FileSystemWatcher
            {
                NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName | NotifyFilters.DirectoryName | NotifyFilters.Attributes
            };
            FirmwareWatcher.BeginInit();
            FirmwareWatcher.Created += new FileSystemEventHandler(WatchChanged);
            FirmwareWatcher.Renamed += new RenamedEventHandler(WatchChanged);
            FirmwareWatcher.Changed += new FileSystemEventHandler(WatchChanged);
            FirmwareWatcher.Deleted += new FileSystemEventHandler(WatchChanged);
            FirmwareWatcher.EndInit();
            Address = Config.ConfiguredGetInt(nameof(Address), 0x01001000).ToString("X08");
            Version = Config.ConfiguredGetInt(nameof(Version), 65535).ToString();
            HashOption = Config.ConfiguredGetInt(nameof(HashOption), 0);
            IsEraserFullChip = Config.ConfiguredGetBoolean(nameof(IsEraserFullChip), false);
            IsExportPackage = Config.ConfiguredGetBoolean(nameof(IsExportPackage), false);
            KeyPath = Config.ConfiguredGet(nameof(KeyPath));
            FirmwarePath = Config.ConfiguredGet(nameof(FirmwarePath));

            EraseStartAddress = Config.ConfiguredGet(nameof(EraseStartAddress), "01000000");
            EraseEndAddress = Config.ConfiguredGet(nameof(EraseEndAddress), "01001000");

            WatchUpdateTimer = new Timer(800)
            {
                AutoReset = false,
            };
            WatchUpdateTimer.Elapsed +=
               new ElapsedEventHandler(
                   (object sender, ElapsedEventArgs e) =>
                   {
                       FirmwarePath = FirmwareWatchPath;
                   }
               );
        }

        #region Watcher
        private readonly FileSystemWatcher FirmwareWatcher;
        private readonly Timer WatchUpdateTimer = null; 
        private string FirmwareWatchPath = null;
        private void WatchChanged(object sender, FileSystemEventArgs e)
        {
            WatchUpdateTimer.Stop();
            switch (e.ChangeType)
            {
                case WatcherChangeTypes.Created:
                case WatcherChangeTypes.Changed:
                case WatcherChangeTypes.Renamed:
                    FirmwareWatchPath = e.FullPath;
                    break;
                case WatcherChangeTypes.Deleted:
                    FirmwareWatchPath = null;
                    break;
            }
            WatchUpdateTimer.Start();
        }
        #endregion

        #region Firmware
        public bool IsLineKeyValid => LineKey != null && LineIV != null;

        public byte[] LineKey;
        public byte[] LineIV;

        public byte[] SM2ID
        {
            get => Firmware.Key.SM2ID;
            set
            {
                if(value != Firmware.Key.SM2ID)
                {
                    Firmware.Key.SM2ID = value;
                    UpdateFile(UpdateSource.Key);
                }
            }
        }

        public enum UpdateSource
        {
            Path,
            Address,
            Version,
            HashOption,
            Key,
            MaxTransferSize,
            LineKey,
            Header,
        }

        public bool UpdateFile(UpdateSource source)
        {
            try
            {
                bool isHeaderUpdated = false, isSignatureUpdated = false;
                if (Firmware.Length > 0)
                {
                    isHeaderUpdated = Firmware.UpdateHeader();
                }
                if (Firmware.IsHeaderValid && Firmware.Key.IsValid)
                {
                    isSignatureUpdated = Firmware.UpdateSignedHeader();
                }
                if (isHeaderUpdated || isSignatureUpdated)
                {
                    Logger.Info("Firmware updated{0}.", isSignatureUpdated ? "(signed)" : "");
                    ExportFile();
                }
                if (source == UpdateSource.Path || source == UpdateSource.Address || source == UpdateSource.LineKey || source == UpdateSource.MaxTransferSize)
                    UpdateCommand();
            }
            catch (Exception ex)
            {
                Logger.Warn(ex.Message);
                return false;
            }
            return true;
        }

        private void ExportFile()
        {
            if (!IsExportPackage)
                return;
            List<string> exportPackageList = new();
            if (Firmware.IsHeaderValid)
            {
                exportPackageList.Add($"[ 调试固件: {Path.GetFileName(Firmware.ExportFile(Firmware.Header))} ] ");
            }
            if (Firmware.IsSignedHeaderValid)
            {
                exportPackageList.Add($"[ 签名固件: {Path.GetFileName(Firmware.ExportFile(Firmware.SignedHeader))} ] ");
            }
            if (exportPackageList.Count > 0)
            {
                string exportInfo = "整合 ";
                foreach(string exportPackage in exportPackageList)
                {
                    exportInfo += exportPackage;
                }
                exportInfo += "导出成功.";
                Logger.Info(exportInfo);
            }
        }

        private uint maxTransferSize = 4096;
        public uint MaxTransferSize
        {
            get => maxTransferSize;
            set
            {
                if (value != maxTransferSize)
                {
                    maxTransferSize = value;
                    UpdateFile(UpdateSource.MaxTransferSize);
                }
            }
        }

        public List<WPacketDownload> CommandList { get; private set; }

        public void UpdateCommand()
        {
            byte[] firmwareBinBytes = Firmware.BinFileByte;
            CommandList = new List<WPacketDownload>();
            if (firmwareBinBytes == null)
                return;

            if (IsLineKeyValid)
            {
                if (Firmware.Key.IsRSAKey)
                {
                    RijndaelManaged aes = new()
                    {
                        Key = LineKey,
                        IV = LineIV,
                        Mode = CipherMode.CBC,
                        Padding = PaddingMode.Zeros,
                    };
                    firmwareBinBytes = aes.CreateEncryptor().TransformFinalBlock(firmwareBinBytes, 0, firmwareBinBytes.Length);
                    Logger.Info("Enc aes bin data generated!");
                }
                else
                {
                    SM4Utils sm4 = new()
                    {
                        hexString = true,
                        secretKey = Hex.GetString(LineKey),
                        iv = Hex.GetString(LineIV)
                    };
                    firmwareBinBytes = Hex.GetBytes(sm4.Encrypt_CBC(firmwareBinBytes));
                    Logger.Info("Enc sm4 bin data generated!");
                }
            }

            // 下载数据生成命令
            for (uint offset = 0; offset < firmwareBinBytes.Length;)
            {
                uint length = Math.Min(maxTransferSize, (uint)(firmwareBinBytes.Length - offset));
                byte[] data = new byte[length];
                Array.Copy(firmwareBinBytes, offset, data, 0, length);
                CommandList.Add(new WPacketDownload(Firmware.Address + offset, data));
                offset += length;
            }
        }

        public readonly FirmwareFile Firmware = new();

        public string FirmwarePath
        {
            get => Firmware.FilePath;
            set
            {
                try
                {
                    FirmwareWatcher.EnableRaisingEvents = false;
                    Firmware.Import(value);
                    FirmwareWatcher.Path = Firmware.FileDirectory;
                    FirmwareWatcher.Filter = Firmware.OriginFileName;
                }
                catch (FileNotFoundException ex)
                {
                    Logger.Debug(ex.Message);
                }
                catch (Exception ex)
                {
                    Logger.Error(ex.Message);
                }
                finally
                {
                    if(FirmwareWatcher.Path != "")
                        FirmwareWatcher.EnableRaisingEvents = true;
                }

                Update(nameof(FirmwarePath), FirmwarePath);
                Update(nameof(Address), Address);
                Update(nameof(Version), Version);
                Update(nameof(HashOption), HashOption);
                Update(nameof(IsEraserFullChip), IsEraserFullChip);

                Update(nameof(FirmwareFileName));
                Update(nameof(IsBinFirmware));
                Update(nameof(IsPartFirmware));

                UpdateFile(UpdateSource.Path);
            }
        }
        public string FirmwareFileName => Firmware.OriginFileName;
        public string FirmwareType => Firmware.OriginFileType;
        public string Address
        {
            get => $"0x{Firmware.Address:X08}";
            set
            {
                if(uint.TryParse(value.Replace("0x", ""), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out uint newAddress))
                {
                    Firmware.Address = newAddress;
                }
                else
                {
                    throw new Exception("请输入正确的数据格式.");
                }
                Update(nameof(Address), $"0x{Firmware.Address:X08}");
                UpdateFile(UpdateSource.Address);
            }
        }

        public Visibility IsBinFirmware => Firmware.IsBinFirmware ? Visibility.Visible : Visibility.Collapsed;
        public Visibility IsPartFirmware => Firmware.IsPartFirmware ? Visibility.Visible : Visibility.Collapsed;
        public string Version
        {
            get => Firmware.Version.ToString();
            set
            {
                if (!(uint.TryParse(value, NumberStyles.Integer, null, out uint newVersion) && newVersion >= 0 && newVersion <= 0xFFFF))
                {
                    Logger.Warn("格式不正确，应为0~65535的数字！");
                    return;
                }
                Firmware.Version = newVersion;
                Update(nameof(Version), Firmware.Version);
                UpdateFile(UpdateSource.Version);
            }
        }

        public int HashOption
        {
            get => Firmware.HashOption == 2 ? 0 : 1;
            set
            {
                uint newHashOption = value == 0 ? (uint)2 : 4;
                Firmware.HashOption = newHashOption;
                Update(nameof(HashOption), HashOption);
                UpdateFile(UpdateSource.HashOption);
            }
        }


        public bool IsEraserFullChip
        {
            get => Firmware.IsEraserFullChip;
            set
            {
                Firmware.IsEraserFullChip = value;
                Update(nameof(IsEraserFullChip), Firmware.IsEraserFullChip);
                ExportFile();
            }
        }

        public bool IsExportPackage
        {
            get => isExportPackage;
            set
            {
                isExportPackage = value;
                Update(nameof(IsExportPackage), isExportPackage);
                ExportFile();
            }
        }
        private bool isExportPackage;

        #endregion

        #region FirmwareKey
        public string KeyPath
        {
            get => Firmware.KeyPath;
            set
            {
                try
                {
                    Firmware.ImportKey(value);
                    if (Firmware.Key.IsValid && !Firmware.Key.IsPrivateKey)
                    {
                        Logger.Warn("The imported key is not a private key. The firmware needs a private key file!");
                        Firmware.ClearKey();
                    }
                }
                catch (FileNotFoundException ex)
                {
                    Logger.Debug(ex.Message);
                }
                catch (Exception ex)
                {
                    Logger.Error(ex.Message);
                }
                Update(nameof(KeyPath), KeyPath);
                Update(nameof(KeyFileName));
                UpdateFile(UpdateSource.Key);
            }
        }
        public string KeyFileName => Firmware.KeyFileName;
        #endregion

        #region EraseFlash
        public string EraseStartAddress {
            get => eraseStartAddress;
            set
            {
                eraseStartAddress = value;
                Update(nameof(EraseStartAddress), eraseStartAddress);
            }
        }
        private string eraseStartAddress;

        public string EraseEndAddress
        {
            get => eraseEndAddress;
            set
            {
                eraseEndAddress = value;
                Update(nameof(EraseEndAddress), eraseEndAddress);
            }
        }
        private string eraseEndAddress;
        #endregion
    }
}
