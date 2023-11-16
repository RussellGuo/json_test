using ISPCore.Chip;
using ISPCore.FileResolver;
using ISPCore.Util;
using MHTool.UI;
using net.meano.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Xml.Linq;

namespace MHTool.Setting
{
    public class DeviceStageUpdateSetting : Setting
    {
        public const string SettingDir = @"./Setting";
        public const string SettingPath = SettingDir + @"/ChipUpdate.xml";

        public SNFormatSetting SNFormat { get; private set; }

        public ChipProperty Chip { get; set; }

        public DeviceStageUpdateSetting(Window owner, ChipProperty chip, ConfigNode config) : base(config)
        {
            Chip = chip;
            SNFormat = new SNFormatSetting(owner, "DeviceSN", SaveSettings, Chip.DeviceSNSize);
            DeviceStageCheckMap = Config.ConfiguredGet(nameof(DeviceStageCheckMap), "0");
            KeyPath = Config.ConfiguredGet(nameof(KeyPath));
            FlashKey = Config.ConfiguredGet(nameof(FlashKey));
            OTPKey = Config.ConfiguredGet(nameof(OTPKey));
            AuthKey = Config.ConfiguredGet(nameof(AuthKey));
            AuthIV = Config.ConfiguredGet(nameof(AuthIV));
            DeviceFlashEncrypt = Config.ConfiguredGetBoolean(nameof(DeviceFlashEncrypt), true);
            DeviceFlashTrueRandom = Config.ConfiguredGetBoolean(nameof(DeviceFlashTrueRandom), true);
            IsOTPTrueRandom = Config.ConfiguredGetBoolean(nameof(IsOTPTrueRandom), false);
        }


        public ushort DeviceStageCheckMapResult { get; private set; } = 0;

        public string DeviceStageCheckMap
        {
            get
            {
                return DeviceStageCheckMapResult.ToString("X04");
            }
            set
            {
                try
                {
                    DeviceStageCheckMapResult = ushort.Parse(value, NumberStyles.HexNumber);
                    Update(nameof(DeviceStageCheckMap), DeviceStageCheckMap);
                }
                catch
                {

                }
            }
        }

        private readonly KeyFile KeyFile = new();

        public string KeyPath
        {
            get => KeyFile.FileName;
            set
            {
                try
                {
                    KeyFile.ImportKeyFile(value);
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
                    Update(nameof(KeyPath), KeyFile.FilePath);
                    Update(nameof(KeyFileName));
                    Update(nameof(FlashKey), FlashKey);
                    Update(nameof(OTPKey), OTPKey);
                    Update(nameof(AuthKey), AuthKey);
                    Update(nameof(AuthIV), AuthIV);
                }
            }
        }

        public string KeyFileName => KeyFile.FileName;

        public string DeviceTimeoutText
        {
            get => DeviceTimeout.ToString();
            set
            {
                if (!(ushort.TryParse(value, NumberStyles.Integer, null, out ushort buf) && buf >= 0 && buf <= 0xFFFF))
                {
                    Logger.Warn("格式不正确，应为0~65535的数字！");
                    return;
                }
                DeviceTimeout = buf;
            }
        }
        public ushort DeviceTimeout = 100;

        public uint DeviceKeyIndex => KeyFile.KeyIndex;

        public bool IsKeyValid => KeyFile.IsValid;

        public bool IsRSAKey => KeyFile.IsRSAKey;

        public byte[] KeyE => KeyFile.RSAKey.Exponent;

        public byte[] KeyN => KeyFile.RSAKey.Modulus;

        public byte[] KeyX => Hex.GetBytes(KeyFile.SM2Key.PublicKeyXString);
        public byte[] KeyY => Hex.GetBytes(KeyFile.SM2Key.PublicKeyYString);

        public Visibility FlashVisibility => Chip.IsInternalFlash ? Visibility.Collapsed : Visibility.Visible;

        public Visibility DeviceCheckVisibility => Chip.IsDeviceCheckNeeded ? Visibility.Visible : Visibility.Collapsed;

        public bool FlashEncryptEnable => DeviceFlashEncrypt == true;

        public bool DeviceFlashEncrypt
        {
            get => DeviceFlashEncryptValue;
            set
            {
                DeviceFlashEncryptValue = value;
                if (!DeviceFlashEncryptValue)
                {
                    DeviceFlashTrueRandom = true;
                }
                Update(nameof(FlashEncryptEnable));
                Update(nameof(DeviceFlashEncrypt), DeviceFlashEncrypt);
            }
        }
        private bool DeviceFlashEncryptValue = true;

        public bool DeviceFlashTrueRandom
        {
            get => DeviceTrueRandomValue | !DeviceFlashEncryptValue;
            set
            {
                DeviceTrueRandomValue = value;
                Update(nameof(FlashKeyVisibility));
                Update(nameof(DeviceFlashTrueRandom), DeviceFlashTrueRandom);
            }
        }
        private bool DeviceTrueRandomValue = true;

        public Visibility FlashKeyVisibility => (Chip.IsInternalFlash || DeviceFlashTrueRandom == true) ? Visibility.Collapsed : Visibility.Visible;

        public string FlashKey
        {
            get => FlashKeyValue;
            set
            {
                FlashKeyValue = value;
                Update(nameof(FlashKey));
            }
        }
        private string FlashKeyValue = "000102030405060708090A0B0C0D0E0F";

        public bool? IsOTPTrueRandom
        {
            get => IsOTPTrueRandomValue;
            set
            {
                IsOTPTrueRandomValue = (bool)value;
                Update(nameof(IsOTPTrueRandom), IsOTPTrueRandom);
                Update(nameof(OTPKeyInputVisibility));
            }
        }
        private bool IsOTPTrueRandomValue = false;

        public Visibility OTPKeyInputVisibility => (!Chip.IsOTPKeyNeeded || IsOTPTrueRandom == true) ? Visibility.Collapsed : Visibility.Visible;

        public Visibility OTPKeyVisibility => Chip.IsOTPKeyNeeded ? Visibility.Visible : Visibility.Collapsed;

        public string OTPKey
        {
            get => Hex.GetString(KeyFile.OTPKey);
            set
            {
                KeyFile.OTPKey = Hex.GetBytes(value);
                Update(nameof(OTPKey), OTPKey);
            }
        }

        public Visibility AuthKeyVisibility => Chip.IsAuthKeyNeeded ? Visibility.Visible : Visibility.Collapsed;

        public string AuthKey
        {
            get => Hex.GetString(KeyFile.AuthKey);
            set => KeyFile.AuthKey = Hex.GetBytes(value);
        }


        public string AuthIV
        {
            get => Hex.GetString(KeyFile.AuthIV);
            set => KeyFile.AuthIV = Hex.GetBytes(value);
        }

        public bool IsUpdateAllowed
        {
            get => isUpdateAllowed;
            set {
                isUpdateAllowed = value;
                Update(nameof(IsUpdateEnabled));
            }
        }
        private bool isUpdateAllowed = false;

        public bool IsUpdating
        {
            get => isUpdating;
            set
            {
                isUpdating = value;
                Update(nameof(IsUpdateEnabled));
            }
        }
        private bool isUpdating = false;

        public bool IsUpdateEnabled => IsUpdateAllowed && !IsUpdating;

        public void LoadDefault()
        {
            SNFormat.Clear();
            if (SNFormat.MaxSize == 16)
            {
                SNFormat.SNElements.Add(new SNFormatSetting.FixedUnit("10808080"));
                SNFormat.SNElements.Add(new SNFormatSetting.RandomUnit(4));
                SNFormat.SNElements.Add(new SNFormatSetting.FixedUnit("20190808"));
                SNFormat.SNElements.Add(new SNFormatSetting.IncrementUnit("00000001", 1));
            }
            else if (SNFormat.MaxSize == 4)
            {
                SNFormat.SNElements.Add(new SNFormatSetting.IncrementUnit("D0000001", 1));
            }
        }

        public void LoadSettings()
        {
            try
            {
                XElement xRoot = XElement.Load(SettingPath);
                XElement xChip = xRoot.Element(Chip.Name);
                SNFormat.XmlRecord = xChip.Element(SNFormat.FormatName);
                Update("");
            }
            catch (Exception ex)
            {
                Logger.Debug(ex.Message);
                LoadDefault();
                SaveSettings();
            }
        }

        public void SaveRecord(string chipSN, string deviceSN, bool result, string error)
        {
            if (result) SNFormat.GenNewSN();

            XElement xRoot = XElement.Load(SettingPath);
            XElement xChip = xRoot.Element(Chip.Name);

            XElement xSNRecord = xChip.Element(SNFormat.FormatName + ".Record");
            if (xSNRecord == null)
                xChip.Add(xSNRecord = new XElement(SNFormat.FormatName + ".Record"));

            xSNRecord.Add(
                new XElement(
                    "Record",
                    new XAttribute("Time", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")),
                    new XAttribute("Result", result),
                    new XAttribute("ChipSN", chipSN),
                    string.IsNullOrEmpty(deviceSN) ? null : new XAttribute("DeviceSN", deviceSN),
                    string.IsNullOrEmpty(error) ? null : new XAttribute("Error", error)
                ));
            xRoot.Save(SettingPath);
        }

        public void SaveSettings()
        {
            if (!File.Exists(SettingPath))
            {

                if (!Directory.Exists(SettingDir))
                {
                    Directory.CreateDirectory(SettingDir);
                }
                XDocument xdoc = new(new XElement("ChipUpdateSetting", null));
                xdoc.Save(SettingPath);
            }
            XElement xRoot = XElement.Load(SettingPath);
            XElement xChip = xRoot.Element(Chip.Name);
            if (xChip == null)
                xRoot.Add(xChip = new XElement(Chip.Name));

            XElement xSN = xChip.Element(SNFormat.FormatName);
            if (xSN != null)
                xSN.Remove();
            xChip.Add(SNFormat.XmlRecord);

            xRoot.Save(SettingPath);
        }
    }
}
