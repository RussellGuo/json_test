using ISPCore.Chip;
using ISPCore.FileResolver;
using ISPCore.Packet;
using ISPCore.Util;
using MHTool.UI;
using net.meano.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows;
using System.Xml.Linq;

namespace MHTool.Setting
{
    public class ChipStageUpdateSetting : INotifyPropertyChanged
    {
        public const string SettingDir = @"./Setting";
        public const string SettingPath = SettingDir + @"/ChipUpdate.xml";
        public event PropertyChangedEventHandler PropertyChanged;

        public ChipProperty Chip { get; set; }

        public ChipStageUpdateSetting(Window owner, ChipProperty chip)
        {
            Chip = chip;
            SNFormat = new SNFormatSetting(owner, "ChipSN", SaveSettings, Chip.ChipSNSize);
        }

        public SNFormatSetting SNFormat { get; private set; }


        public bool IsCustomDeviceSNOpenValue = false;

        public void Update(string PropertyName) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(PropertyName));

        #region 芯片阶段升级到设备阶段
        public ushort ChipStageCheckMapResult { get; private set; }
        public string ChipStageCheckMap
        {
            get
            {
                return ChipStageCheckMapResult.ToString("X04");
            }
            set
            {
                try
                {
                    ChipStageCheckMapResult = ushort.Parse(value, NumberStyles.HexNumber);
                    Update(nameof(ChipStageCheckMap));
                }
                catch
                {

                }
            }
        }

        public Visibility WaferUpdateVisibility => Chip.IsWaferUpdateNeeded ? Visibility.Visible : Visibility.Collapsed;

        public Visibility ChipKeyVisibility => Chip.IsChipKeyNeeded ? Visibility.Visible : Visibility.Collapsed;

        public readonly KeyFile KeyFile = new();

        public string KeyPath
        {
            get => KeyFile.FilePath;
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
                    Update(nameof(KeyPath));
                    Update(nameof(KeyFileName));
                    Update(nameof(AuthKey));
                    Update(nameof(AuthIV));
                }
            }
        }

        public string KeyFileName => KeyFile.FileName;

        public string ChipTimeoutText
        {
            get
            {
                return ChipTimeout.ToString();
            }
            set
            {
                if (!(ushort.TryParse(value, NumberStyles.Integer, null, out ushort newTimeout) && newTimeout >= 0 && newTimeout <= 0xFFFF))
                {
                    Logger.Warn("格式不正确，应为0~65535的数字！");
                    return;
                }
                ChipTimeout = newTimeout;
            }
        }
        public ushort ChipTimeout = 100;
        public uint KeyInfo => KeyFile.KeyIndex;

        public bool IsKeyValid => KeyFile.IsValid;

        public bool IsRSAKey => KeyFile.IsRSAKey;

        public byte[] KeyE => KeyFile.RSAKey.Exponent;

        public byte[] KeyN => KeyFile.RSAKey.Modulus;

        public byte[] KeyX => KeyFile.SM2Key?.PublicKeyXByte;

        public byte[] KeyY => KeyFile.SM2Key?.PublicKeyYByte;

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

        #endregion

        public void LoadDefault() 
        {
            SNFormat.Clear();
            if (SNFormat.MaxSize == 16)
            {
                SNFormat.SNElements.Add(new SNFormatSetting.FixedUnit("10606060"));
                SNFormat.SNElements.Add(new SNFormatSetting.RandomUnit(4));
                SNFormat.SNElements.Add(new SNFormatSetting.FixedUnit("20190808"));
                SNFormat.SNElements.Add(new SNFormatSetting.IncrementUnit("00000001", 1));
            }
            else if (SNFormat.MaxSize == 4)
            {
                SNFormat.SNElements.Add(new SNFormatSetting.IncrementUnit("C0000001", 1));
            }
        }

        public void LoadSettings()
        {
            try
            {
                XElement xRoot = XElement.Load(SettingPath);
                XElement xChip = xRoot.Element(Chip.Name);
                SNFormat.XmlRecord = xChip.Element(SNFormat.FormatName);
                if (PropertyChanged != null)
                {
                    PropertyChanged.Invoke(this, new PropertyChangedEventArgs(""));
                }
            }
            catch (Exception ex)
            {
                Logger.Debug(ex.Message);
                LoadDefault();
                SaveSettings();
            }
        }

        public void SaveRecord(string chipSN, bool result, string error)
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
                    error != null ? new XAttribute("Error", error) : null
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
