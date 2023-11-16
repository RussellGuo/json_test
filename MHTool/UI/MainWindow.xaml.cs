using Cryptography.Encryption;
using ISPCore.Chip;
using ISPCore.Connect;
using ISPCore.FileResolver;
using ISPCore.Packet;
using ISPCore.Util;
using MHTool.Connector;
using MHTool.Setting;
using Microsoft.Win32;
using net.meano.Util;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Xml.Linq;


namespace MHTool.UI
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        #region Initialize

        #region Language
        public ResourceDictionary Lang;


        private void LanguageButton_Click(object sender, RoutedEventArgs e)
        {
            if (LanguageList.Visibility == Visibility.Visible || sender == LanguageList)
            {
                LanguageList.Visibility = Visibility.Collapsed;
                ConnectorGroupBox.Height = 262;
                ConnectorGroupBox.Margin = new Thickness(10, 0, 0, 0);
            }
            else
            {
                ConnectorGroupBox.Height = 232;
                LanguageList.Visibility = Visibility.Visible;
                ConnectorGroupBox.Margin = new Thickness(10, 30, 0, 0);
            }
        }

        private void LanguageSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string languageConfig = "System";
            object selectItem = ((ComboBoxItem)LanguageList.SelectedItem).Content;
            if(selectItem == Lang["Language.zh-cn"])
            {
                languageConfig = "zh-cn";
            }
            else if (selectItem == Lang["Language.en-us"])
            {
                languageConfig = "en-us";
            }
            GlobalConfig.Set("Language", languageConfig);

            UpdateLanguage(GlobalConfig.Get("Language"));
            LanguageButton_Click(LanguageList, null);
        }

        private void UpdateLanguage(string language)
        {
            if (language != "System")
            {
                Thread.CurrentThread.CurrentCulture = new CultureInfo(language);
                Thread.CurrentThread.CurrentUICulture = new CultureInfo(language);
            }
            else
            {
                Thread.CurrentThread.CurrentCulture = new CultureInfo(SystemLanguage);
            }

            string systemLanguage = CultureInfo.CurrentCulture.Name;

            language = systemLanguage.ToLower() switch
            {
                "zh-cn" => "zh-cn",
                "en-us" => "en-us",
                _ => "en-us",
            };

            Application.Current.Resources.MergedDictionaries[0] = new ResourceDictionary()
            {
                Source = new Uri("pack://application:,,,/Language/" + language + ".xaml", UriKind.RelativeOrAbsolute)
            };
            Lang = Application.Current.Resources;
            Logger.Lang = Lang;
        }

        private string SystemLanguage;

        private void InitializeLanguage()
        {
            SystemLanguage = CultureInfo.CurrentCulture.Name;
            Lang = Application.Current.Resources;

            string languageConfig = GlobalConfig.ConfiguredGet("Language");

            languageConfig = languageConfig switch
            {
                "zh-cn" => "zh-cn",
                "en-us" => "en-us",
                _ => "System",
            };

            LanguageList.Text = (string)Lang["Language." + languageConfig];
        }
        #endregion


        public string StaticTitle;

        public ConfigNode GlobalConfig = new("Config");
        public MainWindow()
        {
            InitializeComponent();

            InitializeLog();
            InitializeLanguage();
            InitializeConnector();
            InitializeFirmware();
            InitializeFlash();
            InitializeSelfCheck();
            InitializeAutoTask();

#if DEBUG
            Title = $"{Title} V{App.BuildVersion(true)}";
#else
            Title = $"{Title} V{App.BuildVersion(false)}";
#endif


            StaticTitle = Title;

            ChipConnected(false);
        }

        private void WindowClosed(object sender, EventArgs e)
        {
            GlobalConfig.Save();
        }

        private void HexTextBoxPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (e.Source is not TextBox textbox)
            {
                return;
            }
            string previewText = textbox.Text.Insert(textbox.SelectionStart, e.Text);

            Regex re = new("^(0x)?[0-9A-Fa-f]{0,8}$");
            e.Handled = !re.IsMatch(previewText);
        }

        #endregion

        #region SelfCheck

        private ObservableCollection<SelfCheckUnit> SelfCheckItemList;
        public void InitializeSelfCheck()
        {
            SelfCheckItemList = new ObservableCollection<SelfCheckUnit>();
            SelfCheckItemControl.ItemsSource = SelfCheckItemList;
        }

        private void SelfCheckButton_Click(object sender, RoutedEventArgs e)
        {
            if (Connector == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return;
            }
            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return;
            }
            WPacketCryptoCheck CryptoMap = new((ushort)SelfCheckUnit.GenerateCheckMap(SelfCheckItemList));
            Connector.SendCommand(Header.CryptoCheck, CryptoMap);
            Connector.SendReady();
        }

        private void BootCheckButton_Click(object sender, RoutedEventArgs e)
        {
            if (Connector == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return;
            }
            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return;
            }
            Connector.SendCommand(Header.BootCheck, null);
            Connector.SendReady();
        }


        private void SelfCheckAllButton_Click(object sender, RoutedEventArgs e)
        {
            SelfCheckUnit.CheckAll(SelfCheckItemList);
        }

        private void SelfCheckInverseButton_Click(object sender, RoutedEventArgs e)
        {
            SelfCheckUnit.CheckInverse(SelfCheckItemList);
        }

        private void ClearCheckButton_Click(object sender, RoutedEventArgs e)
        {
            SelfCheckUnit.ResetColor(SelfCheckItemList);
        }
        #endregion

        #region Logger
        public void InitializeLog()
        {
            Logger.RegisterSave();
            Logger.Info("Version: {0}", App.BuildVersion(true));
            Logger.Register(LoggerUIEvent);
        }


        public void LoggerUIEvent(LogUnit log)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                SolidColorBrush messagecolor = new();
                messagecolor = log.Type switch
                {
                    LogType.Debug => Brushes.Purple,
                    LogType.Error => Brushes.Red,
                    LogType.Fatal => Brushes.DarkRed,
                    LogType.Warn => Brushes.DarkGoldenrod,
                    LogType.Info => Brushes.Black,
                    _ => Brushes.DarkGray,
                };
                Run logmessage =
                    new($"[{DateTime.Now.ToLongTimeString()} {Enum.GetName(typeof(LogType), log.Type).ToUpper()}]: {log.Message}")
                    {
                        Foreground = messagecolor
                    };

                if (log.Message.Contains(Logger.T("Auto")))
                {
                    bool isSuccess;
                    if ((isSuccess = log.Message.Contains(Logger.T("succeeded"))) || log.Message.Contains(Logger.T("failed")))
                    {
                        logmessage = new(log.Message)
                        {
                            Foreground = isSuccess ? Brushes.LimeGreen : Brushes.Red,
                            FontSize = 35,
                            FontStretch = FontStretches.UltraExpanded,
                        };
                    }
                }
                if (LogList.Items.Count > 200)
                {
                    LogList.Items.Clear();
                }
                LogList.Items.Add(logmessage);
                LogList.SelectedIndex = LogList.Items.Count - 1;
                LogList.ScrollIntoView(LogList.SelectedItems[0]);
            }));
        }
        private void LogList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (MessageBox.Show(Logger.T("Clear the log?"), "", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
                LogList.Items.Clear();
        }

        public void ActionProgressBarSetColor(SolidColorBrush color) {
            ActionProgressBar.Foreground = color;
        }

        public void ActionProgressBarChange(double start, double ratio, double percent) => ActionProgressBarChange(start, ratio, percent, "");

        public void ActionProgressBarChange(double start, double ratio, double percent, string info)
        {
            double totalPercent = start + (percent * ratio);
            ActionProgressBar.Value = ActionProgressBar.Maximum * totalPercent;
            ActionLabel.Content = string.Format(Logger.T(info), percent);
            if (info != "")
            {
                ActionLabel.Margin = new Thickness(0, 0, 0, 0);
                double progressWidth = ActionProgressBar.ActualWidth * totalPercent;
                ActionLabel.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));

                if (progressWidth > ActionLabel.DesiredSize.Width)
                {
                    ActionLabel.HorizontalAlignment = HorizontalAlignment.Right;
                    ActionLabel.Margin = new Thickness(0, 0, ActionProgressBar.ActualWidth - progressWidth, 0);
                    ActionLabel.Foreground = Brushes.WhiteSmoke;
                }
                else
                {
                    ActionLabel.HorizontalAlignment = HorizontalAlignment.Left;
                    ActionLabel.Margin = new Thickness(progressWidth, 0, 0, 0);
                    ActionLabel.Foreground = Brushes.Black;
                }
            }
        }

        public void ActionProgressBarShow(bool IsShow) => ActionGrid.Visibility = IsShow ? Visibility.Visible : Visibility.Collapsed;
        #endregion

        #region Firmware
        //public WPacketFirmware Firmware;
        public FirmwareSetting FirmwareOption;
        public OpenFileDialog FirmwareOpenFile = null;
        public OpenFileDialog FirmwareKeyOpenFile = null;

        public void InitializeFirmware()
        {
            FirmwareOption = new(GlobalConfig.GetNode("Firmware"));
            FirmwareTab.DataContext = FirmwareOption;
            
            FirmwareOpenFile = new OpenFileDialog
            {
                Title = "请选择固件文件",
                InitialDirectory = "",
                Filter =
                    Lang["Firmware.Filter.HexFile"] + "|*.hex|" +
                    Lang["Firmware.Filter.BinFile"] + "|*.bin|" +
                    Lang["Firmware.Filter.SigFile"] + "|*.sig|" +
                    Lang["Firmware.Filter.UsigFile"] + "|*.usig",
                ValidateNames = true,
                CheckPathExists = true,
                CheckFileExists = true,
                Multiselect = false
            };

            FirmwareKeyOpenFile = new OpenFileDialog
            {
                Title = "选择私钥文件",
                InitialDirectory = "",
                Filter =
                    "RSA密钥文件(*.rsa)" + "|*.rsa|" +
                    "SM2密钥文件(*.sm2)" + "|*.sm2",
                ValidateNames = true,
                CheckPathExists = true,
                CheckFileExists = true,
                Multiselect = false,
            };
        }

        private DateTime DownloadStartTime;
        public void Download()
        {
            if (Connector == null || ConnectedChip == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return;
            }

            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return;
            }


            FirmwareFile firmware = FirmwareOption.Firmware;

            bool isFirmwareNeedSign = ConnectedChip.Stage == StageEnum.Secure;
            if (isFirmwareNeedSign && !firmware.IsSignedHeaderValid)
            {
                Logger.Error("Signed firmware not available, load aborted!");
                return;
            }
            else if(!isFirmwareNeedSign && !firmware.IsHeaderValid)
            {
                Logger.Error("Firmware not available, load aborted!");
                return;
            }
            {
                FirmwareOption.LineKey = null;
                FirmwareOption.LineIV = null;
            }

            WPacketEraseFlash wef = null;
            WPacketEraseFlash wefHeader = null;
            uint externalFlashSize = 0;
            if (firmware.Section == FirmwareFile.SectionType.Flash)
            {
                if (!ConnectedChip.IsInternalFlash)
                {
                    if (FlashOptions.MCUFlashOption == null)
                    {
                        if (!ConnectedChip.IsBondingFlash)
                        {
                            FlashList.SelectedIndex = -1;
                            Logger.Warn("No current flash parameters, please configure and save!");
                        }
                        else
                        {
                            Logger.Error("Chip storage problem, load aborted!");
                        }
                        return;
                    }
                    externalFlashSize = (uint)FlashOptions.MCUFlashOption.FlashSizeValue * 1024 * 128;
                    ConnectedChip.FlashFwMaxAddress = ConnectedChip.FlashStartAddresss + externalFlashSize - 1;
                }
                else
                {
                    ConnectedChip.FlashFwMaxAddress = ConnectedChip.FlashStartAddresss + ConnectedChip.InternalFlashSize - 1;
                }

                if (firmware.Address < ConnectedChip.FlashFwMinAddress || firmware.EndAddress > ConnectedChip.FlashFwMaxAddress)
                {
                    Logger.Error(
                        "The firmware region [ 0x{0:X08} ~ 0x{1:X08} ] is not in the available {2} firmware region [ 0x{3:X08} ~ 0x{4:X08} ], load aborted!",
                        firmware.Address,
                        firmware.EndAddress,
                        "Flash",
                        ConnectedChip.FlashFwMinAddress,
                        ConnectedChip.FlashFwMaxAddress
                        );
                    return;
                }

                Logger.Info(
                    "Load firmware to: [ 0x{0:X08} ~ 0x{1:X08} ] with a total of {2} bytes.",
                    firmware.Address,
                    firmware.EndAddress,
                    firmware.Length
                    );

                if (firmware.IsEraserFullChip)
                {
                    Logger.Info("Erase all sectors.");
                    wef = new WPacketEraseFlash(0, externalFlashSize > 0x01000000 ? (externalFlashSize / ConnectedChip.FlashSectorSize) : 0xFFFFFFFF, ConnectedChip.FlashSectorSize);
                }
                else
                {
                    uint eraseStartAddress = (firmware.Address - ConnectedChip.FlashStartAddresss) & (~(ConnectedChip.FlashSectorSize - 1));
                    uint eraseEndAddress = ((firmware.EndAddress - ConnectedChip.FlashStartAddresss) & (~(ConnectedChip.FlashSectorSize - 1))) + ((firmware.EndAddress % ConnectedChip.FlashSectorSize == 0) ? 0 : ConnectedChip.FlashSectorSize);
                    uint eraseSectors = (eraseEndAddress - eraseStartAddress) / ConnectedChip.FlashSectorSize;
                    Logger.Info("Erase requires sectors [ 0x{0:X08} ~ 0x{1:X08} ] with a total of {2} sectors.", eraseStartAddress, eraseEndAddress, eraseSectors);
                    wefHeader = new WPacketEraseFlash(ConnectedChip.FlashFwHeaderOffset, 1, ConnectedChip.FlashSectorSize);
                    wef = new WPacketEraseFlash(eraseStartAddress, eraseSectors, ConnectedChip.FlashSectorSize);
                }
            }
            else if (firmware.Section == FirmwareFile.SectionType.RAM)
            {
                if (firmware.Address < ConnectedChip.RamFwMinAddress || firmware.EndAddress >= ConnectedChip.RamFwMaxAddress)
                {
                    Logger.Error(
                        "The firmware region [ 0x{0:X08} ~ 0x{1:X08} ] is not in the available {2} firmware region [ 0x{3:X08} ~ 0x{4:X08} ], load aborted!",
                        firmware.Address,
                        firmware.EndAddress,
                        "RAM",
                        ConnectedChip.RamFwMinAddress,
                        ConnectedChip.RamFwMaxAddress
                        );
                    return;
                }
                Logger.Info(
                    "Load firmware to: [ 0x{0:X08} ~ 0x{1:X08} ] with a total of {2} bytes.",
                    firmware.Address,
                    firmware.EndAddress,
                    firmware.Length
                    );
                    wef = new WPacketEraseFlash(0, 0xFFFFFFFF, ConnectedChip.FlashSectorSize);
            }
            else
            {
                Logger.Error(
                    "The firmware region [ 0x{0:X08} ~ 0x{1:X08} ] is not in RAM or Flash region, load aborted!",
                    firmware.Address,
                    firmware.EndAddress
                    );
                return;
            }

            byte[] firmwareHeader = isFirmwareNeedSign ? firmware.SignedHeader : firmware.Header;
            Logger.Debug($"Firmware header: {Hex.GetString(firmwareHeader)}");
            if (FirmwareOption.IsLineKeyValid)
            {
                if (firmware.Key.IsRSAKey)
                {
                    RijndaelManaged aes = new()
                    {
                        Key = FirmwareOption.LineKey,
                        IV = FirmwareOption.LineIV,
                        Mode = CipherMode.CBC,
                        Padding = PaddingMode.Zeros,
                    };
                    firmwareHeader = aes.CreateEncryptor().TransformFinalBlock(firmwareHeader.ToArray(), 0, firmwareHeader.Count());
                    Logger.Debug("Enc aes header:" + Hex.GetString(firmwareHeader));
                }
                else
                {
                    SM4Utils sm4 = new()
                    {
                        hexString = true,
                        secretKey = Hex.GetString(FirmwareOption.LineKey),
                        iv = Hex.GetString(FirmwareOption.LineIV)
                    };
                    firmwareHeader = Hex.GetBytes(sm4.Encrypt_CBC(firmwareHeader));
                    Logger.Debug("Enc sm4 header:" + Hex.GetString(firmwareHeader));
                }
            }
            // 固件头发送命令
            Connector.SendCommand(Header.FWHeader, firmwareHeader);

            if (wefHeader != null)
                Connector.SendCommand(Header.EraserFlash, wefHeader.ToBytes());

            // 擦除命令
            if (wef != null)
            {
                int eraseTimeout = (int)((wef.Sector == 0xFFFFFFFF ? (ConnectedChip.FlashFwMaxAddress + 1 - ConnectedChip.FlashStartAddresss) : (wef.Sector * wef.SectorSize)) / 1024 * 12 + 2000); // 12ms / 1KB
                Logger.Debug("Estimated erase time {0} ms", eraseTimeout);
                Connector.SendCommand(Header.EraserFlash, wef.ToBytes(), eraseTimeout);
            }

            // 下载命令
            List<WPacketDownload> firmwareCommandList = FirmwareOption.CommandList;

            if (!(firmwareCommandList?.Count > 0))
            {
                Logger.Error("Firmware command list is empty!");
                return;
            }
            foreach(WPacketDownload command in firmwareCommandList)
            {
                if (command != firmwareCommandList.Last())
                {
                    Connector.SendCommand(Header.FWData, command);
                }
                else
                {
                    int fwTimeout = (int)(firmwareCommandList.Count * FirmwareOption.MaxTransferSize / 4096 * 4 + 2000); // 4ms / 4KB
                    Connector.SendCommand(Header.FWData, command, fwTimeout);
                }
            }


            // USB串口处理
            if (ConnectedChip.IsSupportVCP && (Connector is SerialConnector serial) && serial.IsLegacyVCP)
            {
                Connector.SendCommand(Header.VCPCompleted, null);
            }
            Connector.SendReady();
            DownloadStartTime = DateTime.Now;
            DownloadButtonLabel.SetResourceReference(ContentProperty, "ui.Firmware.LoadButton.Loading");
            ActionProgressBarSetColor(Brushes.LimeGreen);
            ActionProgressBarShow(true);
        }

        public void DownloadButton_Click(object sender, RoutedEventArgs e)
        {
            if (DownloadButtonLabel.Content == Lang["ui.Firmware.LoadButton.Load"])
            {
                Download();
            }
            else
            {
                if (Connector != null)
                    Connector.Disconnect();
                Logger.Info("Manually interrupt load!");
                DownloadButtonLabel.SetResourceReference(ContentProperty, "ui.Firmware.LoadButton.Load");
            }
        }

        private void FirmwareSelectButton_Click(object sender, RoutedEventArgs e)
        {
            FirmwareOpenFile.InitialDirectory = FirmwareOption.Firmware.FileDirectory;
            FirmwareOption.FirmwarePath = FirmwareOpenFile.ShowDialog() == true ? FirmwareOpenFile.FileName : null;
        }

        private void FirmwareKeySelectButton_Click(object sender, RoutedEventArgs e)
        {
            FirmwareKeyOpenFile.InitialDirectory = FirmwareOption.Firmware.Key.FileDirectory;
            FirmwareOption.KeyPath = FirmwareKeyOpenFile.ShowDialog() == true ? FirmwareKeyOpenFile.FileName : null;
        }

        #endregion

        #region Connector
        public IConnector Connector;
        public ChipProperty ConnectedChip => Connector?.ConnectedChip;
        public SerialConnector Serialcon;
        public USBConnector MSDConnector;
        public DeviceListener DeviceMessage;
        public void InitializeConnector()
        {
            DeviceMessage = new DeviceListener();
            SourceInitialized += DeviceMessage.RegisterListener;

            Serialcon = new SerialConnector(ConnectorType.SerialConnector);
            Serialcon.Event += new ConnectorHandler(ConnectorCallBack);
            DeviceMessage.Event += Serialcon.SerialChangedEvent;

            MSDConnector = new USBConnector(ConnectorType.USBConnector);
            MSDConnector.Event += new ConnectorHandler(ConnectorCallBack);
            DeviceMessage.Event += MSDConnector.USBChangedEvent;

            UpdatePort(this, null);
            BaudrateList.Text = GlobalConfig.ConfiguredGetInt("Baudrate", 115200).ToString();
        }

        private void UpdatePort(object sender, EventArgs e)
        {
            bool IsPortChanged = false;
            string CurrentPort = PortList.Text;
            string[] serialPorts = SerialPort.GetPortNames();
            List<string> avaliablePortsList = serialPorts.ToList();
            avaliablePortsList = avaliablePortsList.Where((t, i) => serialPorts.Take(i + 1).Count(s => s == t) <= 1).ToList();
            avaliablePortsList.Sort();
            foreach (DriveInfo chipDrive in USBConnector.GetChipDrives())
            {
                avaliablePortsList.Insert(0, chipDrive.Name);
            }

            if (avaliablePortsList.Count() == PortList.Items.Count)
            {
                foreach (string spname in avaliablePortsList)
                {
                    if (!PortList.Items.Contains(spname))
                    {
                        IsPortChanged = true;
                        break;
                    }
                }
            }
            else
            {
                IsPortChanged = true;
            }
            if (IsPortChanged)
            {
                PortList.Items.Clear();
                foreach (string spname in avaliablePortsList)
                {
                    PortList.Items.Add(spname);
                }
            }
            if (PortList.Items.Contains(GlobalConfig.ConfiguredGet("Port")))
            {
                // 首先选择上一次连接成功的端口
                PortList.Text = GlobalConfig.Get("Port");
            }
            else if (PortList.Items.Contains(CurrentPort))
            {
                // 再选择列表更新之前的端口
                PortList.Text = CurrentPort;
            }
            else
            {
                // 最后选择第一个可用端口
                PortList.SelectedIndex = 0;
            }

            if (IsPortChanged)
            {
                Logger.Info("There are {0} port(s) available!", PortList.Items.Count);
            }
        }

        private void PortSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string selectedPort = (e.AddedItems.Count != 0 ? e.AddedItems[0].ToString() : "").Trim();
            bool isSerialPort = selectedPort.StartsWith("COM");

            BaudrateGrid.Visibility = isSerialPort ? Visibility.Visible : Visibility.Collapsed;

            if (isSerialPort)
            {
                DeviceInfo deviceInfo = DeviceInfo.GetSerialInfo(selectedPort);
                if (deviceInfo != null)
                {
                    PortInfoTextBlock.Text = string.Format(Logger.T(@"DESC: {0}\nMFR: {1}"), deviceInfo.BusDescription ?? deviceInfo.Description, deviceInfo.Manufacturer);
                    return;
                }
            }
            else
            {
                DriveInfo driveInfo = USBConnector.GetChipDriveInfo(selectedPort);
                if (driveInfo != null)
                {
                    bool isDiskMode = USBConnector.GetDriveMode(driveInfo) == USBConnector.ModeEnum.MHDisk;
                    PortInfoTextBlock.Text = string.Format(
                        Logger.T(@"Letter: {0}\nFormat: {1}\nLabel: {2}\nMode: {3}"),
                        driveInfo.Name.Replace(":\\", ""),
                        isDiskMode ? "RAW" : driveInfo.DriveFormat,
                        isDiskMode ? "Empty" : driveInfo.VolumeLabel,
                        isDiskMode ? "Physical disk mode" : "Logical volume mode"
                        );
                    return;
                }
            }

            PortInfoTextBlock.Text = "";
        }

        private string UpdateBaudrate(string baudrate)
        {
            if (!int.TryParse(baudrate, out int selectedBaudrate))
            {
                BaudrateList.Text = "115200";
                return "115200";
            }
            if (!GlobalConfig.Get("Baudrate").Equals(selectedBaudrate.ToString()))
            {
                GlobalConfig.Set("Baudrate", selectedBaudrate.ToString());
            }
            return GlobalConfig.Get("Baudrate");
        }

        private void BaudrateTextChanged(object sender, TextChangedEventArgs e) => UpdateBaudrate(BaudrateList.Text);

        public void ChipConnectButton_Click(object sender, RoutedEventArgs e)
        {
            object device;
            IConnector connector;
            string portName = PortList.Text.Trim();
            if (portName?.StartsWith("COM") == true)
            {
                device = new SerialPort(portName, int.Parse(BaudrateList.Text));
                connector = Serialcon;
            }
            else
            {
                device = portName;
                connector = MSDConnector;
            }
            if (ChipConnectButtonLabel.Content == Lang["ui.Connect.ConnectButton.Connect"])
            {
                ChipConnectButtonLabel.SetResourceReference(ContentProperty, "ui.Connect.ConnectButton.Connecting");
                ActionProgressBarSetColor(Brushes.DeepSkyBlue);
                ActionProgressBarShow(true);
                connector.Connect(device);
            }
            else if (ChipConnectButtonLabel.Content == Lang["ui.Connect.ConnectButton.Connecting"])
            {
                connector?.Disconnect();
            }
            else
            {
                Connector?.Disconnect();
            }
        }

        private void CancelCheckBoxClickBubble(object sender, RoutedEventArgs e) => e.Handled = true;

        #endregion

        #region Protection

        private ProtectionSetting ProtectionOption;

        private void InitializeProtection(ChipProperty chip)
        {
            ProtectionOption = new(GlobalConfig.GetNode(chip.Name).GetNode("Protection"));
            ProtectGroupBox.DataContext = ProtectionOption;
        }

        private void ReadProtectionButton_Click(object sender, RoutedEventArgs e)
        {
            if (Connector == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return;
            }
            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return;
            }
            List<byte> byteList = new();
            byteList.AddRange(BitConverter.GetBytes(ProtectionOption.ReadProtectionStartUint));
            byteList.AddRange(BitConverter.GetBytes(ProtectionOption.ReadProtectionEndUint));
            Connector.SendCommand(Header.SetReadProtection, byteList.ToArray());
            Connector.SendReady();
        }

        private void WriteProtectionButton_Click(object sender, RoutedEventArgs e)
        {
            if (sender is not Button button)
            {
                return;
            }

            if (button.Content == Lang["ui.SecurityTab.Protection.GenerateKey"])
            {
                ProtectionOption?.GenerateWriteProtectionKey();
                return;
            }

            if (Connector == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return;
            }
            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return;
            }

            List<byte> byteList = new();
            if (button.Content == Lang["ui.SecurityTab.Protection.SetWriteProtection"])
            {
                byteList.AddRange(BitConverter.GetBytes(ProtectionOption.WriteProtectionStartUint));
                byteList.AddRange(BitConverter.GetBytes(ProtectionOption.WriteProtectionEndUint));
                byteList.AddRange(BitConverter.GetBytes(ProtectionOption.WriteProtectionKeyUint));
                Connector.SendCommand(Header.SetWriteProtection, byteList.ToArray());
                Connector.SendReady();
            }
            else if (button.Content == Lang["ui.SecurityTab.Protection.ClearWriteProtection"])
            {
                byteList.AddRange(BitConverter.GetBytes(ProtectionOption.WriteProtectionKeyUint));
                Connector.SendCommand(Header.ClearWriteProtection, byteList.ToArray());
                Connector.SendReady();
            }
        }
        #endregion


        #region DeviceStageUpdate
        public DeviceStageUpdateSetting DeviceStageUpdate;
        public OpenFileDialog DeviceKeyOpenFile;
        ObservableCollection<SelfCheckUnit> DeviceStageCheckItemList;

        private void InitializeDeviceStage(ChipProperty connectedChip)
        {
            DeviceStageUpdate = new DeviceStageUpdateSetting(this, connectedChip, GlobalConfig.GetNode("DeviceUpdate"));
            DeviceStageUpdate.LoadSettings();

            DeviceStageUpdateTab.DataContext = DeviceStageUpdate;

            DeviceStageCheckItemList = new ObservableCollection<SelfCheckUnit>();
            DeviceCheckMap.ItemsSource = DeviceStageCheckItemList;

            SelfCheckUnit.InitializeCheckBitMap(DeviceStageCheckItemList, ConnectedChip.CheckBitMap, DeviceStageUpdate.DeviceStageCheckMapResult);
            DeviceCheckCheckBox_Click(null, null);

            DeviceKeyOpenFile = new OpenFileDialog
            {
                Title = "选择设备公钥文件",
                InitialDirectory = "",
                Filter = "RSA公钥文件(*.rsa)" + "|*.rsa|" + "SM2公钥文件(*.sm2)" + "|*.sm2",
                ValidateNames = true,
                CheckPathExists = true,
                CheckFileExists = true,
                Multiselect = false
            };
            DeviceKeyOpenFile.FileOk += new CancelEventHandler(DeviceKeyOpenFileOK);

            ////////////////////////////////////////////////////////////////////

            bool IsStrictMode = false;
            FlashEncryptCheckBox.Visibility = IsStrictMode ? Visibility.Collapsed : Visibility.Visible;
            DeviceStageUpdate.DeviceFlashTrueRandom = IsStrictMode;
            DeviceStageUpdate.DeviceFlashEncrypt = IsStrictMode;
        }

        private void DeviceSNClick(object sender, RoutedEventArgs e) => DeviceStageUpdate.SNFormat.Show();

        private void DeviceCheckCheckBox_Click(object sender, RoutedEventArgs e)
        {
            DeviceStageUpdate.DeviceStageCheckMap = SelfCheckUnit.GenerateCheckMap(DeviceStageCheckItemList).ToString("X04");
        }

        private void DeviceKeyOpenFileOK(object sender, CancelEventArgs e) => DeviceKeyLabel.ToolTip = DeviceKeyOpenFile.FileName;

        private void DeviceKeyButton_Click(object sender, RoutedEventArgs e) => DeviceKeyOpenFile.ShowDialog(this);

        public bool DeviceUpdate()
        {
            if (Connector == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return false;
            }

            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return false;
            }

            if (!DeviceStageUpdate.SNFormat.IsSNReady)
            {
                Logger.Warn("SN format has not been configured!");
                return false;
            }

            if (!DeviceStageUpdate.IsKeyValid)
            {
                Logger.Warn("Key not found!");
                return false;
            }

            if (!DeviceStageUpdate.IsRSAKey && !ConnectedChip.IsSupportSMKey)
            {
                Logger.Warn("SMKey is not supported!");
                return false;
            }

            WPacketChipUpdate deviceStageUpdatePacket;
            deviceStageUpdatePacket = new WPacketChipUpdate(
                ConnectedChip,
                false,
                DeviceStageUpdate.SNFormat.SNByte,
                DeviceStageUpdate.DeviceTimeout,
                DeviceStageUpdate.DeviceStageCheckMapResult,
                DeviceStageUpdate.DeviceKeyIndex,
                DeviceStageUpdate.IsRSAKey ? DeviceStageUpdate.KeyE : DeviceStageUpdate.KeyX,
                DeviceStageUpdate.IsRSAKey ? DeviceStageUpdate.KeyN : DeviceStageUpdate.KeyY,
                DeviceStageUpdate.DeviceFlashEncrypt == true,
                DeviceStageUpdate.DeviceFlashTrueRandom == true,
                DeviceStageUpdate.IsOTPTrueRandom == true,
                FlashKeyTextBox.Text,
                OTPKeyTextBox.Text,
                !DeviceStageUpdate.IsRSAKey
            )
            {
                AuthKey = Hex.GetBytes(DeviceStageUpdate.AuthKey),
                AuthIV = Hex.GetBytes(DeviceStageUpdate.AuthIV)
            };

            DeviceStageUpdate.IsUpdating = true;
            Logger.Info("Device updating...");

            {
                Connector.SendCommand(ConnectedChip.IsSupportDeviceSN ? Header.ChipUpdate : Header.ChipUpdateNoSN, deviceStageUpdatePacket);
                Connector.SendReady();
            }

            return true;
        }

        public void DeviceSNUpdateButton_Click(object sender, RoutedEventArgs re)
        {
            DeviceUpdate();
        }
        #endregion

        #region Flash
        public FlashSetting FlashOptions;
        public void InitializeFlash()
        {
            FlashOptions = new(GlobalConfig.GetNode("Flash"));
            FlashOptions.LoadFlashSettings();

            FlashOptionTab.DataContext = FlashOptions;

            FlashList.ItemsSource = FlashOptions.Flashes;
            // 设置列表项目绑定
            Binding bindFlashText = new();
            bindFlashText.Source = FlashOptions;
            bindFlashText.Path = new PropertyPath(nameof(FlashSetting.Flashes));
            bindFlashText.Mode = BindingMode.TwoWay;
            FlashList.SetBinding(ItemsControl.ItemsSourceProperty, bindFlashText);

            // 绑定FlashName到下拉选择列表
            Binding bindFlashListText = new();
            bindFlashListText.Source = FlashOptions;
            bindFlashListText.Path = new PropertyPath(nameof(FlashSetting.CurrentFlashName));
            bindFlashListText.Mode = BindingMode.TwoWay;
            FlashList.SetBinding(ComboBox.TextProperty, bindFlashListText);
            FlashList.SelectedIndex = 0;

            // Flash控制字绑定数据
            Binding bindFlashDataGridItemSource = new();
            bindFlashDataGridItemSource.Source = FlashOptions;
            bindFlashDataGridItemSource.Path = new PropertyPath(nameof(FlashSetting.CurrentFlashCollection));
            bindFlashDataGridItemSource.Mode = BindingMode.OneWay;
            FlashDataGrid.SetBinding(ItemsControl.ItemsSourceProperty, bindFlashDataGridItemSource);
            FlashDataGrid.MinRowHeight = 35;
            FlashDataGrid.RowHeaderWidth = 50;
            FlashDataGrid.IsReadOnly = true;
            FlashDataGrid.IsEnabled = false;

            // Flash厂商编号绑定
            Binding bindFlashFlashManuShowTextBoxText = new();
            bindFlashFlashManuShowTextBoxText.Source = FlashOptions;
            bindFlashFlashManuShowTextBoxText.Path = new PropertyPath(nameof(FlashSetting.CurrentFlashManu));
            FlashManuShowTextBox.SetBinding(TextBox.TextProperty, bindFlashFlashManuShowTextBoxText);

            // FlashID绑定
            Binding bindFlashFlashIDShowTextBoxText = new();
            bindFlashFlashIDShowTextBoxText.Source = FlashOptions;
            bindFlashFlashIDShowTextBoxText.Path = new PropertyPath(nameof(FlashSetting.CurrentFlashID));
            FlashIDShowTextBox.SetBinding(TextBox.TextProperty, bindFlashFlashIDShowTextBoxText);

            // Flash Size绑定
            Binding bindFlashFlashSizeShowTextBoxText = new();
            bindFlashFlashSizeShowTextBoxText.Source = FlashOptions;
            bindFlashFlashSizeShowTextBoxText.Path = new PropertyPath(nameof(FlashSetting.CurrentFlashSize));
            FlashSizeShowTextBox.SetBinding(TextBox.TextProperty, bindFlashFlashSizeShowTextBoxText);
        }

        public void TrigFlashEditStatu(bool isEditing)
        {
            FlashOptions.IsEditing = isEditing;
            FlashList.IsEditable = isEditing;
            FlashDataGrid.IsReadOnly = !isEditing;
            FlashDataGrid.IsEnabled = isEditing;
            FlashManuShowTextBox.IsReadOnly = !isEditing;
            FlashIDShowTextBox.IsReadOnly = !isEditing;
            FlashSizeShowTextBox.IsReadOnly = !isEditing;
            FlashManuShowTextBox.BorderThickness = new Thickness(isEditing ? 1 : 0);
            FlashIDShowTextBox.BorderThickness = new Thickness(isEditing ? 1 : 0);
            FlashSizeShowTextBox.BorderThickness = new Thickness(isEditing ? 1 : 0);
            FlashAddButton.IsEnabled = !isEditing;
            FlashDeleteButton.IsEnabled = !isEditing;
            FlashEditButton.IsEnabled = !isEditing;
            FlashIDButton.IsEnabled = !isEditing;
            FlashSetButton.IsEnabled = !isEditing;
            FlashReloadButton.SetResourceReference(ContentProperty, isEditing ? "ui.Flash.ReloadButton.Cancel" : "ui.Flash.ReloadButton.Normal");
        }

        private void FlashEditButton_Click(object sender, RoutedEventArgs e)
        {
            if (FlashEditButton.Content == Lang["ui.Flash.EditButton.Edit"])
            {
                TrigFlashEditStatu(true);
                FlashEditButton.IsEnabled = true;
                FlashEditButton.SetResourceReference(ContentProperty, "ui.Flash.EditButton.Save");
            }
            else
            {
                string SaveFlashName = FlashOptions.CurrentFlashOption.FlashName;
                if (FlashOptions.CurrentFlashOption.FlashName != FlashOptions.CurrentFlashName)
                {
                    if (FlashOptions.Flashes.Contains(FlashOptions.CurrentFlashOption.FlashName))
                    {
                        Logger.Error("Flash名称重复！请重新键入！");
                        return;
                    }
                }
                TrigFlashEditStatu(false);
                FlashOptions.SaveFlashSettings();
                FlashEditButton.SetResourceReference(ContentProperty, "ui.Flash.EditButton.Edit");
                FlashList.Text = SaveFlashName;
                Logger.Info("Flash配置保存成功！");
                if (Connector != null && Connector.IsChipConnected)
                {
                    Logger.Info("将重新匹配Flash！");
                    FlashIDButton_Click(null, null);
                }
            }
        }

        private void FlashAddButton_Click(object sender, RoutedEventArgs e)
        {
            if (FlashAddButton.Content == Lang["ui.Flash.AddButton.Edit"])
            {
                FlashList.SelectedIndex = -1;
                TrigFlashEditStatu(true);
                FlashAddButton.IsEnabled = true;
                FlashAddButton.SetResourceReference(ContentProperty, "ui.Flash.AddButton.Save");
            }
            else
            {
                string SaveFlashName = FlashOptions.CurrentFlashOption.FlashName;
                if (SaveFlashName != FlashOptions.CurrentFlashName)
                {
                    if (FlashOptions.Flashes.Contains(SaveFlashName))
                    {
                        Logger.Error("Flash名称重复！请重新键入！");
                        return;
                    }
                }
                if (string.IsNullOrEmpty(FlashOptions.CurrentFlashOption.FlashName))
                {
                    Logger.Error("必须键入Flash名称！");
                    return;
                }
                TrigFlashEditStatu(false);
                FlashOptions.AddCurrentSetting();
                FlashOptions.SaveFlashSettings();
                FlashAddButton.SetResourceReference(ContentProperty, "ui.Flash.AddButton.Edit");
                FlashList.Text = SaveFlashName;
                Logger.Info("新的Flash(" + SaveFlashName + ")配置保存成功！");
            }
        }

        private void FlashDeleteButton_Click(object sender, RoutedEventArgs e)
        {
            if (MessageBox.Show("确定删除当前配置？", "删除Flash配置", MessageBoxButton.YesNo).Equals(MessageBoxResult.Yes))
            {
                FlashOptions.DelCurrentSetting();
                FlashOptions.SaveFlashSettings();
                Logger.Info("当前Flash配置已删除！");
            }
        }

        private void FlashDataGrid_LoadingRow(object sender, DataGridRowEventArgs e)
        {
            if (e.Row.GetIndex() == 0)
            {
                e.Row.Header = "Cmd";
            }
            else if (e.Row.GetIndex() == 1)
            {
                e.Row.Header = "Option";
            }
            else
            {
                e.Row.Header = "Data";
            }
        }

        private void FlashReloadButton_Click(object sender, RoutedEventArgs e)
        {
            if (FlashReloadButton.Content == Lang["ui.Flash.ReloadButton.Normal"])
            {
                FlashOptions.LoadFlashSettings();
                Logger.Info("Flash配置重载成功！");
            }
            else
            {
                FlashAddButton.SetResourceReference(ContentProperty, "ui.Flash.AddButton.Edit");
                FlashEditButton.SetResourceReference(ContentProperty, "ui.Flash.EditButton.Edit");
                TrigFlashEditStatu(false);
                FlashOptions.LoadFlashSettings();
                FlashList.SelectedIndex = 0;
            }

        }

        private void FlashIDButton_Click(object sender, RoutedEventArgs e)
        {
            if (Connector == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return;
            }
            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return;
            }
            Connector.SendCommand(Header.FlashID, null);
            Connector.SendReady();
        }

        private void FlashSetButton_Click(object sender, RoutedEventArgs e)
        {
            if (Connector == null)
            {
                Logger.Warn("The chip is not connected yet, please connect the chip before operation!");
                return;
            }
            if (!Connector.CanSendCommand)
            {
                Logger.Warn("The task thread is busy!");
                return;
            }
            if (FlashList.Text == "")
            {
                Logger.Warn("No flash configuration, please select Flash configuration!");
                return;
            }
            WPacketFlashOption wfo = FlashOptions.CurrentFlashOption;

                if (!((bool)FlashOTPCheckBox.IsChecked))
                {
                    wfo.Mask = 0x55555555;
                }
                else
                {
                    if (MessageBox.Show(this, $"写入OTP的Flash参数只能写入{ConnectedChip.FlashOptionCount}次！请谨慎写入！确认写入请选择'是'！", "警告", MessageBoxButton.YesNo, MessageBoxImage.Warning) != MessageBoxResult.Yes)
                    {
                        Logger.Warn("用户取消了向OTP中写入Flash参数！");
                        return;
                    }
                    wfo.Mask = 0x00000000;
                }

            wfo.IsFlashOptionDataNeeded = ConnectedChip.IsFlashOptionDataNeeded;
            wfo.IsFlashOptionIDNeeded = ConnectedChip.IsFlashOptionIDNeeded;

            Connector.SendCommand(Header.WriteFlashOption, wfo);
            Connector.SendReady();
        }

        #endregion

        #region AutoTask
        public AutoTaskSetting AutoTaskOptions;

        public void InitializeAutoTask()
        {
            AutoTaskOptions = new AutoTaskSetting(this, GlobalConfig.GetNode("AutoTask"));
            AutoTaskGroupBox.DataContext = AutoTaskOptions;
        }

        private void AutoTaskButton_Click(object sender, RoutedEventArgs e)
        {
            if (AutoTaskButton.Content == Lang["ui.AutoTask.AutoButton.Start"])
            {
                AutoTaskOptions.TaskTimerStart(PortList.Text.Trim());
                AutoTaskButton.SetResourceReference(ContentProperty, "ui.AutoTask.AutoButton.Stop");
            }
            else
            {
                AutoTaskOptions.TaskTimerStop();
                AutoTaskButton.SetResourceReference(ContentProperty, "ui.AutoTask.AutoButton.Start");
            }
        }
        #endregion

    }
}
