using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using ISPCore.Packet;
using static ISPCore.Packet.WPacketFlashOption;

namespace MHTool.Setting
{
    public class FlashSetting : Setting
    {
        public const string SettingDir = @"./Setting";
        public const string SettingPath = SettingDir + @"/Flash.xml";
        public Dictionary<string, WPacketFlashOption> FlashTable = new()
        {
            ["GD25Q128C"] = new()
            {
                FlashName = "GD25Q128C",
                FlashManu = "C8",
                FlashID = "4018",
                FlashSize = "128",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x31, 0x70, 0x0002),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["GD25Q64C"] = new()
            {
                FlashName = "GD25Q64C",
                FlashManu = "C8",
                FlashID = "4017",
                FlashSize = "64",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x31, 0x70, 0x0002),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["GD25Q32C"] = new()
            {
                FlashName = "GD25Q32C",
                FlashManu = "C8",
                FlashID = "4016",
                FlashSize = "32",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x31, 0x70, 0x0002),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["GD25Q80C"] = new()
            {
                FlashName = "GD25Q80C",
                FlashManu = "C8",
                FlashID = "4014",
                FlashSize = "8",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x31, 0x70, 0x0002),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["GD25Q41B"] = new()
            {
                FlashName = "GD25Q41B",
                FlashManu = "C8",
                FlashID = "4013",
                FlashSize = "4",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x31, 0x70, 0x0002),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["GD25Q21B"] = new()
            {
                FlashName = "GD25Q21B",
                FlashManu = "C8",
                FlashID = "4012",
                FlashSize = "2",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x31, 0x70, 0x0002),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["W25Q40"] = new()
            {
                FlashName = "W25Q40",
                FlashManu = "EF",
                FlashID = "4013",
                FlashSize = "8",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["W25Q80"] = new()
            {
                FlashName = "W25Q80",
                FlashManu = "EF",
                FlashID = "4014",
                FlashSize = "8",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["W25Q16"] = new()
            {
                FlashName = "W25Q16",
                FlashManu = "EF",
                FlashID = "4015",
                FlashSize = "16",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["W25Q32"] = new()
            {
                FlashName = "W25Q32",
                FlashManu = "EF",
                FlashID = "4016",
                FlashSize = "32",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["W25Q64"] = new()
            {
                FlashName = "W25Q64",
                FlashManu = "EF",
                FlashID = "4017",
                FlashSize = "64",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["W25Q128"] = new()
            {
                FlashName = "W25Q128",
                FlashManu = "EF",
                FlashID = "4018",
                FlashSize = "128",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["T25S40"] = new()
            {
                FlashName = "T25S40",
                FlashManu = "1C",
                FlashID = "3113",
                FlashSize = "4",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x00, 0x80, 0x0200),
                burst = new flashcmd(0x00, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["P25Q40H"] = new()
            {
                FlashName = "P25Q40H",
                FlashManu = "85",
                FlashID = "6013",
                FlashSize = "4",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["P25Q80H"] = new()
            {
                FlashName = "P25Q80H",
                FlashManu = "85",
                FlashID = "6014",
                FlashSize = "8",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["P25Q32H"] = new()
            {
                FlashName = "P25Q32H",
                FlashManu = "85",
                FlashID = "6016",
                FlashSize = "32",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["ZB25VQ40"] = new()
            {
                FlashName = "ZB25VQ40",
                FlashManu = "5E",
                FlashID = "6013",
                FlashSize = "4",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["ZB25VQ80"] = new()
            {
                FlashName = "ZB25VQ80",
                FlashManu = "5E",
                FlashID = "6014",
                FlashSize = "8",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["ZB25VQ16"] = new()
            {
                FlashName = "ZB25VQ16",
                FlashManu = "5E",
                FlashID = "6015",
                FlashSize = "16",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["ZB25VQ32"] = new()
            {
                FlashName = "ZB25VQ32",
                FlashManu = "5E",
                FlashID = "4016",
                FlashSize = "32",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
            ["ZB25VQ64"] = new()
            {
                FlashName = "ZB25VQ64",
                FlashManu = "5E",
                FlashID = "4017",
                FlashSize = "64",
                Id = new flashcmd(0x9f, 0x30, 0x0000),
                QE = new flashcmd(0x01, 0x80, 0x0200),
                burst = new flashcmd(0x77, 0x42, 0x0040),
                prg = new flashcmd(0x32, 0xd1, 0x0000),
                se = new flashcmd(0x20, 0x90, 0x0000),
                ce = new flashcmd(0xc7, 0x00, 0x0000),
                Wakeup = new flashcmd(0xab, 0x00, 0x0000),
                Cacheread = new flashcmd(0xeb, 0xb2, 0x0000)
            },
        };

        public bool IsEditing = false;
        public List<string> Flashes
        {
            get
            {
                List<string> Keys = new();
                foreach (string key in FlashTable.Keys)
                {
                    Keys.Add(key);
                }
                return Keys;
            }
            set
            {
                Flashes = value;
            }
        }

        public string CurrentFlashName
        {
            get
            {
                return CurrentFlashNameValue;
            }
            set
            {
                if (!IsEditing)
                {
                    CurrentFlashNameValue = value;
                    Update("");
                }
                else
                {
                    CurrentFlashOption.FlashName = value;
                }
            }
        }
        public string CurrentFlashNameValue;

        public WPacketFlashOption BufferFlashOption = new();
        public WPacketFlashOption MCUFlashOption = new();

        public FlashSetting(ConfigNode config) : base(config)
        {
            IsWriteOTP = Config.ConfiguredGetBoolean(nameof(IsWriteOTP), false);
        }

        public WPacketFlashOption CurrentFlashOption
        {
            get
            {
                if (Flashes.Contains(CurrentFlashName))
                    return (WPacketFlashOption)FlashTable[CurrentFlashName];
                else
                    return BufferFlashOption;
            }
        }

        public ObservableCollection<OptionRow> CurrentFlashCollection
        {
            get
            {
                return CurrentFlashOption.FlashCollection;
            }
        }

        public string CurrentFlashManu
        {
            get
            {
                return CurrentFlashOption.FlashManu;
            }
            set
            {
                CurrentFlashOption.FlashManu = value;
            }
        }

        public string CurrentFlashID
        {
            get
            {
                return CurrentFlashOption.FlashID;
            }
            set
            {
                CurrentFlashOption.FlashID = value;
            }
        }

        public string CurrentFlashSize
        {
            get => $"{CurrentFlashOption.FlashSize} Mbit";
            set => CurrentFlashOption.FlashSize = value.Replace("Mbit", "");
        }

        public bool IsWriteOTP {
            get => isWriteOTP;
            set
            {
                isWriteOTP = value;
                Update(nameof(IsWriteOTP), isWriteOTP);
            }
        }
        private bool isWriteOTP;

        public void LoadFlashSettings()
        {
            try
            {
                XElement xe = XElement.Load(SettingPath);
                IEnumerable<XElement> elements = from ele in xe.Elements("Flash") select ele;
                // List<string> flashNames = new List<string>();
                foreach (var ele in elements)
                {
                    WPacketFlashOption wfo = new()
                    {
                        XmlRecord = ele
                    };
                    if (FlashTable.ContainsKey(wfo.FlashName))
                    {
                        FlashTable.Remove(wfo.FlashName);
                    }
                    FlashTable.Add(wfo.FlashName, wfo);
                    // flashNames.Add(wfo.FlashName);
                }

                //foreach(WPacketFlashOption flashOption in FlashTable.Values)
                //{
                //    if (!flashNames.Contains(flashOption.FlashName))
                //    {
                //        SaveFlashSettings();
                //        break;
                //    }
                //}

                Update("");
            }
            catch (Exception)
            {
                if (File.Exists(SettingPath))
                {
                    SaveFlashSettings();
                }
            }
        }

        public void SaveFlashSettings()
        {
            if (!File.Exists(SettingPath))
            {
                if (!Directory.Exists(SettingDir))
                {
                    Directory.CreateDirectory(SettingDir);
                }
                XDocument xdoc = new(new XElement("Flashes", null));
                xdoc.Save(SettingPath);
            }
            XElement xe = XElement.Load(SettingPath);
            xe.RemoveAll();
            foreach (WPacketFlashOption flashOption in FlashTable.Values)
            {
                xe.Add(flashOption.XmlRecord);
            }
            xe.Save(SettingPath);
            return;
        }

        public void AddCurrentSetting()
        {
            FlashTable.Add(CurrentFlashOption.FlashName, CurrentFlashOption);
        }

        public void DelCurrentSetting()
        {
            FlashTable.Remove(CurrentFlashOption.FlashName);
        }
    }
}
