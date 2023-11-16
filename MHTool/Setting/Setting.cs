using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace MHTool.Setting
{
    public abstract class Setting : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        public void Update(string propertyName) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));

        public void Update(string propertyName, object updateObject)
        {
            Update(propertyName);
            Config.Set(propertyName, updateObject?.ToString());
        }

        public ConfigNode Config { get; private set; }

        public Setting(ConfigNode config)
        {
            Config = config;
        }
    }
}
