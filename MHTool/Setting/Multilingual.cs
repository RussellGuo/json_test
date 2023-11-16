using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace MHTool.Setting
{
    public class Multilingual
    {
        public static void Switch(ResourceDictionary resource)
        {
            if (Lang.MergedDictionaries.Contains(resource))
            {
                Lang.MergedDictionaries.Add(resource);
            }
        }

        public static ResourceDictionary Lang = new();

    }


}
