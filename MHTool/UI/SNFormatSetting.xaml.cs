using ISPCore.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using System.Xml.Linq;

namespace MHTool.UI
{
    public partial class SNFormatSetting : Window, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public string FormatName { get; private set; }

        public int[] SNSize;

        public void Info(string message)
        {
            MessageBox.Show(this, message, "提示", MessageBoxButton.OK, MessageBoxImage.Information, MessageBoxResult.Yes);
        }

        public void Warn(string message)
        {
            MessageBox.Show(this, message, "警告", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.Yes);
        }

        public void Error(string message)
        {
            MessageBox.Show(this, message, "错误", MessageBoxButton.OK, MessageBoxImage.Error, MessageBoxResult.Yes);
        }

        public delegate void SaveDelegate();

        private readonly SaveDelegate SaveHandle;

        public SNFormatSetting(Window owner, string formatName, SaveDelegate saveHandle, int[] snSize)
        {
            Owner = owner;
            SaveHandle = saveHandle;
            FormatName = formatName;
            SNSize = snSize;
            InitializeComponent();
            DataContext = this;
        }

        public void UpdateProperty(string PropertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged.Invoke(this, new PropertyChangedEventArgs(PropertyName));
                SaveHandle?.Invoke();
            }
        }

        public Visibility VisibilityState => MaxSize != 0 ? Visibility.Visible : Visibility.Collapsed;

        private void FixedButtonClick(object sender, RoutedEventArgs e) => AddFixElements(FixedText);

        private void RandomButtonClick(object sender, RoutedEventArgs e) => AddRandomElements(RandomText);

        private void AutoincrementButtonClick(object sender, RoutedEventArgs e) => AddIncElements(AutoincrementText);

        private void DeleteButtonClick(object sender, RoutedEventArgs e) =>  DelElement();


        public string FixedText { get; set; }

        public string RandomText { get; set; }

        public string AutoincrementText { get; set; }

        private void OnClosing(object sender, CancelEventArgs e)
        {
            e.Cancel = true;
            Hide();
            Owner.Activate();
        }

        public string SNText
        {
            get
            {
                if (IsSNReady)
                {
                    StringBuilder buf = new();
                    foreach (SNElement sne in SNElements)
                    {
                        buf.Append(sne.ElementString);
                    }
                    return buf.ToString();
                }
                else
                {
                    return "SN格式未设置完成";
                }
            }
            set
            {
                UpdateProperty(nameof(SNText));
            }
        }

        public string SNPreviewText
        {
            get
            {
                StringBuilder buf = new();
                foreach (SNElement sne in SNElements)
                {
                    buf.Append(sne.PreviewDisplay);
                }
                return buf.ToString();
            }
        }

        public string SNFormatText
        {
            get
            {
                StringBuilder buf = new();
                foreach (SNElement sne in SNElements)
                {
                    buf.Append(sne.FormatDisplay);
                    buf.Append(',');
                }
                return buf.ToString();
            }
        }

        public byte[] SNByte
        {
            get
            {
                List<byte> bytelist = new();
                foreach (SNElement sne in SNElements)
                {
                    bytelist.AddRange(sne.ElementByte);
                }
                return bytelist.ToArray();
            }
        }

        public int SNCurrentSize
        {
            get
            {
                int tmp = 0;
                foreach (SNElement sne in SNElements)
                {
                    tmp += sne.ByteLength;
                }
                return tmp;
            }
        }

        public int MaxSize
        {
            get
            {
                if (SNSize == null)
                    return 0;
                int maxSNSize = 0;
                foreach (int snSize in SNSize)
                {
                    maxSNSize = Math.Max(snSize, maxSNSize);
                }
                return maxSNSize;
            }
        }

        public void GenNewSN()
        {
            foreach (SNElement sne in SNElements)
            {
                sne.GenElement();
            }
            UpdateProperty("SNText");
        }

        public bool IsSNReady
        {
            get
            {
                if (MaxSize == 0)
                    return true;
                foreach (int snSize in SNSize)
                {
                    if (snSize == SNCurrentSize)
                        return true;
                }
                return false;
            }
        }

        public List<SNElement> SNElements
        {
            get
            {
                return SNElementsValue;
            }
        }

        public XElement XmlRecord
        {
            get
            {
                XElement SNFormatRecord = new(FormatName);
                foreach(SNElement ele in SNElements)
                {
                    SNFormatRecord.Add(
                        new XElement("SNUnit",
                            new XAttribute("Format", ele.SingleElementFormat),
                            new XAttribute("Data", ele.PreviewDisplay))
                        );
                }
                return SNFormatRecord;
            }
            set
            {
                SNElements.Clear();
                IEnumerable<XElement> elements = from ele in value.Elements("SNUnit") select ele;
                foreach (var ele in elements)
                {
                    switch (ele.Attribute("Format").Value) {
                        case "F":
                            SNElements.Add(new FixedUnit(ele.Attribute("Data").Value));
                            break;
                        case "R":
                            SNElements.Add(new RandomUnit(ele.Attribute("Data").Value));
                            break;
                        case "I":
                            SNElements.Add(new IncrementUnit(ele.Attribute("Data").Value, 1));
                            break;
                    }
                }
                if(SNCurrentSize > MaxSize)
                {
                    SNElements.Clear();
                    SaveHandle();
                    Error("SN记录出错，请重新配置！");
                }
            }
        }

        public void AddFixElements(string FixBytes)
        {
            if(!string.IsNullOrEmpty(FixBytes) && (FixBytes.Length%2 == 0) && Regex.IsMatch(FixBytes, "^[0-9A-Fa-f]+$"))
            {
                if((SNCurrentSize + FixBytes.Length/2) <= MaxSize)
                {
                    SNElements.Add(new FixedUnit(FixBytes));
                    UpdateProperty("");
                    if(IsSNReady)
                    {
                        Info("SN设置完成！");
                    }
                    else
                    {
                        Info("固定值添加成功，但SN长度不够，还需添加其他单元！");
                    }
                }
                else
                {
                    Warn("固定值添加失败，添加后SN长度超出SN规定长度" + MaxSize);
                }
                
            }
            else
            {
                Warn("固定值添加失败，格式/长度错误！");
            }
        }

        public void AddRandomElements(string RandomCountString)
        {
            if (string.IsNullOrEmpty(RandomCountString) || !int.TryParse(RandomCountString, out int RandomCount) || RandomCount <= 0)
            {
                Warn("数字格式不正确!");
                return;
            }
            if ((SNCurrentSize + RandomCount) <= MaxSize)
            {
                SNElements.Add(new RandomUnit(RandomCount));
                UpdateProperty("");
                if (IsSNReady)
                {
                    Info("SN设置完成！");
                }
                else
                {
                    Info("随机值添加成功，但SN长度不够，还需添加其他单元！");
                }
            }
            else
            {
                Warn("随机值添加失败，添加后SN长度超出SN规定长度" + MaxSize);
            }
            SaveHandle();
        }

        public void AddIncElements(string IncDefault)
        {
            if (!string.IsNullOrEmpty(IncDefault) && (IncDefault.Length % 2 == 0) && Regex.IsMatch(IncDefault, "^[0-9A-Fa-f]+$"))
            {
                if ((SNCurrentSize + IncDefault.Length / 2) <= MaxSize)
                {
                    SNElements.Add(new IncrementUnit(IncDefault, 1));
                    UpdateProperty("");
                    if (IsSNReady)
                    {
                        Info("SN设置完成！");
                    }
                    else
                    {
                        Info("自增值添加成功，但SN长度不够，还需添加其他单元！");
                    }
                }
                else
                {
                    Warn("自增值添加失败，添加后SN长度超出SN规定长度" + MaxSize);
                }

            }
            else
            {
                Warn("自增值添加失败，格式/长度错误！");
            }
        }

        public void DelElement()
        {
            if (SNElements.Count > 0)
            {
                SNElements.RemoveAt(SNElements.Count - 1);
                UpdateProperty("");
                Info("删掉一个SN单元！");
            }
            else
            {
                Warn("SN已经清空！无法再删除！");
            }
        }

        public void Clear()
        {
            SNElements.Clear();
            UpdateProperty("");
        }

        public List<SNElement> SNElementsValue = new();

        public abstract class SNElement
        {
            public readonly char SingleElementFormat;
            public readonly int SingleElementCount;
            public readonly bool HasDefaultValue;
            public int RepeatCount = 1;
            public BigInteger ElementData
            {
                get
                {
                    return BigInteger.Parse(ElementString, NumberStyles.HexNumber);
                }
                set
                {
                    ElementString = value.ToString("X0" + (ByteLength*2).ToString());
                }
            }
            public SNElement(char format, int count, bool hasdefault)
            {
                SingleElementFormat = format;
                SingleElementCount = count;
                HasDefaultValue = hasdefault;
            }

            public abstract string PreviewDisplay
            {
                 get;
            }

            public string FormatDisplay
            {
                get
                {
                    return new string(SingleElementFormat, RepeatCount);
                }
            }

            public int ByteLength
            {
                get
                {
                    return SingleElementCount * RepeatCount;
                }
            }

            public byte[] ElementByte
            {
                get
                {
                    if(ElementByteValue==null || ElementByteValue.Length != ByteLength)
                    {
                        ElementByteValue = new byte[ByteLength];
                    }
                    return ElementByteValue;
                }
                set
                {
                    ElementByteValue = value;
                }
            }

            private byte[] ElementByteValue;

            public string ElementString
            {
                get
                {
                    return Hex.GetString(ElementByte);
                }
                set
                {
                    if (!string.IsNullOrEmpty(value))
                    {
                        ElementByte = new byte[value.Length / 2];
                        for (int i= 0;i<ElementByte.Length;i++)
                        {
                            ElementByte[i] = byte.Parse(value.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber);
                        }
                    }
                    else
                    {
                        ElementByte = new byte[0];
                    }
                }
            }

            public abstract void GenElement();
        }
        public class RandomUnit : SNElement
        {
            public RandomUnit(string data) : base('R', 1, false)
            {
                RepeatCount = data.Length / 2;
                GenElement();
            }

            public RandomUnit(int count) : base('R', 1, false)
            {
                RepeatCount = count;
                GenElement();
            }

            private static readonly Random Rand = new();

            public override string PreviewDisplay
            {
                get
                {
                    return Hex.GetString(RandomBytes(ByteLength));
                }
            }

            private byte[] RandomBytes(int Length)
            {
                byte[] bytes = new byte[Length];
                Rand.NextBytes(bytes);
                return bytes;
            }

            public override void GenElement()
            {
                ElementByte = RandomBytes(RepeatCount);
            }
        }

        public class FixedUnit : SNElement
        {
            public FixedUnit(string fixeddata) : base('F', 1, true)
            {
                RepeatCount = fixeddata.Length / 2;
                ElementString = fixeddata;
            }

            public override void GenElement()
            {
                
            }

            public override string PreviewDisplay
            {
                get
                {
                    return ElementString;
                        //.Replace("yyyy", DateTime.Now.ToString("yyyy"))
                        //.Replace("yy", DateTime.Now.ToString("yy"))
                        //.Replace("MM", DateTime.Now.ToString("MM"))
                        //.Replace("dd", DateTime.Now.ToString("dd"))
                        //.Replace("HH", DateTime.Now.ToString("HH"))
                        //.Replace("mm", DateTime.Now.ToString("mm"))
                        //.Replace("ss", DateTime.Now.ToString("ss"));
                }
            }
        }

        public class IncrementUnit : SNElement
        {
            private int GenCount = 0;
            private readonly int AddCount = 1;
            public IncrementUnit(string initdata, int addcount): base('I', 1, true)
            {
                RepeatCount = initdata.Length / 2;
                ElementString = initdata;
                AddCount = addcount;
            }

            public override void GenElement()
            {
                ElementData += AddCount;
                GenCount++;
            }

            public override string PreviewDisplay
            {
                get
                {
                    return ElementString;
                }
            }
        }

    }
}
