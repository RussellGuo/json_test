using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;

namespace MHTool.Setting
{
    public class ProtectionSetting : Setting
    {
        public ProtectionSetting(ConfigNode config) : base(config)
        {
            ReadProtectionStart = Config.ConfiguredGetInt(nameof(ReadProtectionStart), 0x00000000).ToString("X08");
            ReadProtectionEnd = Config.ConfiguredGetInt(nameof(ReadProtectionEnd), 0x00000000).ToString("X08");
            WriteProtectionStart = Config.ConfiguredGetInt(nameof(WriteProtectionStart), 0x00000000).ToString("X08");
            WriteProtectionEnd = Config.ConfiguredGetInt(nameof(WriteProtectionEnd), 0x00000000).ToString("X08");
            WriteProtectionKey = Config.ConfiguredGetInt(nameof(WriteProtectionKey), new Random().Next(int.MinValue, int.MaxValue)).ToString("X08");
        }

        public uint ReadProtectionStartUint { get; private set; }
        public string ReadProtectionStart
        {
            get => $"0x{ReadProtectionStartUint:X08}";
            set
            {
                ReadProtectionStartUint = uint.Parse(value.Replace("0x", ""), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                Update(nameof(ReadProtectionStart), $"0x{ReadProtectionStartUint:X08}");
            }
        }

        public uint ReadProtectionEndUint { get; private set; }
        public string ReadProtectionEnd
        {
            get => $"0x{ReadProtectionEndUint:X08}";
            set
            {
                ReadProtectionEndUint = uint.Parse(value.Replace("0x", ""), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                Update(nameof(ReadProtectionEnd), $"0x{ReadProtectionEndUint:X08}");
            }
        }

        public uint WriteProtectionStartUint { get; private set; }
        public string WriteProtectionStart
        {
            get => $"0x{WriteProtectionStartUint:X08}";
            set
            {
                WriteProtectionStartUint = uint.Parse(value.Replace("0x", ""), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                Update(nameof(WriteProtectionStart), $"0x{WriteProtectionStartUint:X08}");
            }
        }

        public uint WriteProtectionEndUint { get; private set; }
        public string WriteProtectionEnd
        {
            get => $"0x{WriteProtectionEndUint:X08}";
            set
            {
                WriteProtectionEndUint = uint.Parse(value.Replace("0x", ""), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                Update(nameof(WriteProtectionEnd), $"0x{WriteProtectionEndUint:X08}");
            }
        }

        public void GenerateWriteProtectionKey()
        {
            WriteProtectionKeyUint = (uint)new Random().Next(int.MinValue, int.MaxValue);
            Update(nameof(WriteProtectionKey), $"0x{WriteProtectionKeyUint:X08}");
        }

        public uint WriteProtectionKeyUint { get; private set; }
        public string WriteProtectionKey
        {
            get => $"0x{WriteProtectionKeyUint:X08}";
            set
            {
                WriteProtectionKeyUint = uint.Parse(value.Replace("0x", ""), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
                Update(nameof(WriteProtectionKey), $"0x{WriteProtectionKeyUint:X08}");
            }
        }
    }
}
