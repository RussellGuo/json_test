using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Media;

namespace MHTool.UI
{
    public class SelfCheckUnit : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public void Update(string propertyName) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        public string Name { get; set; }

        public bool IsChecked { get => isChecked; set { isChecked = value; Update(nameof(IsChecked)); } }
        private bool isChecked;

        public Brush Color
        {
            get
            {
                return color;
            }
            set
            {
                color = value;
                Update(nameof(Color));
            }
        }
        private Brush color = Brushes.Transparent;

        private int Bit = 0;

        public static void InitializeCheckBitMap(ObservableCollection<SelfCheckUnit> selfCheckItems, Dictionary<string, int> checkBitMap, uint checkMap)
        {
            selfCheckItems.Clear();
            foreach(KeyValuePair<string, int> checkBitPair in checkBitMap)
            {
                SelfCheckUnit item = new() { Name = checkBitPair.Key, Bit = checkBitPair.Value, IsChecked = checkBitPair.Key.Contains("Disable") ? true : false };
                if (((uint)(1 << item.Bit) & checkMap) != 0)
                    item.IsChecked = true;
                selfCheckItems.Add(item);
            }
        }

        public static uint GenerateCheckMap(ObservableCollection<SelfCheckUnit> selfCheckItems)
        {
            uint checkMap = 0;
            foreach(SelfCheckUnit item in selfCheckItems)
            {
                checkMap |= item.IsChecked ? (uint)(1 << item.Bit) : 0;
            }
            return checkMap;
        }

        public static bool UpdateCheckMap(ObservableCollection<SelfCheckUnit> selfCheckItems, ushort checkResult)
        {
            bool isSuccess = true;
            foreach (SelfCheckUnit item in selfCheckItems)
            {
                item.Color = item.IsChecked ? ((checkResult & (uint)(1 << item.Bit)) == 0 ? Brushes.LightGreen : Brushes.Red) : Brushes.Transparent;
                if (item.Color == Brushes.Red) isSuccess = false;
            }
            return isSuccess;
        }

        public static void CheckAll(ObservableCollection<SelfCheckUnit> selfCheckItems)
        {
            foreach (SelfCheckUnit item in selfCheckItems)
            {
                item.IsChecked = true;
            }

        }

        public static void CheckInverse(ObservableCollection<SelfCheckUnit> selfCheckItems)
        {
            foreach (SelfCheckUnit item in selfCheckItems)
            {
                item.IsChecked = !item.IsChecked;
            }
        }

        public static void ResetColor(ObservableCollection<SelfCheckUnit> selfCheckItems)
        {
            foreach (SelfCheckUnit item in selfCheckItems)
            {
                item.Color = Brushes.Transparent;
            }
        }
    }
}
