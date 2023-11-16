using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Linq;

namespace MHTool.Setting
{
    public class ConfigNode
    {
        public string NodeName { get; private set; }

        public ConfigNode ParentNode { get; private set; }

        public string Path => ParentNode?.Path ?? (DirectoryPath + @"\" + NodeName + ".xml");
        public string DirectoryPath => ParentNode?.DirectoryPath ?? directory;
        private string directory;

        private readonly Dictionary<string, ConfigOption> Options = new Dictionary<string, ConfigOption>();
        private readonly Dictionary<string, ConfigNode> Nodes = new Dictionary<string, ConfigNode>();

        public ConfigNode(string rootName) : this(rootName, @"Setting") { }

        public ConfigNode(string rootName, string rootDirectory)
        {
            NodeName = rootName;
            directory = rootDirectory;
            LoadRootNode();
        }

        public ConfigNode(string nodeName, ConfigNode parentNode, IEnumerable<XElement> nodeElement)
        {
            NodeName = nodeName;
            ParentNode = parentNode;
            LoadNode(nodeElement);
        }

        public bool Has(string Name)
        {
            return Options.ContainsKey(Name);
        }

        public bool Set(string name, string value)
        {
            if (Has(name) && Options[name].IsConfigured)
            {
                Options[name].Value = value ?? string.Empty;
                return true;
            }
            return false;
        }

        private string ConfiguredSet(string name, string value)
        {
            Options[name] = new ConfigOption()
            {
                Value = value ?? string.Empty,
                IsConfigured = true
            };
            return Options[name].Value;
        }

        public string Get(string name)
        {
            return Has(name) ? Options[name].Value : null;
        }

        public string ConfiguredGet(string name) => ConfiguredGet(name, string.Empty);

        public string ConfiguredGet(string name, string defaultValue)
        {
            string result = Has(name) ? Options[name].Value : defaultValue;
            ConfiguredSet(name, result);
            return result;
        }

        public int ConfiguredGetInt(string name, int defaultValue)
        {
            if (!int.TryParse(ConfiguredGet(name, defaultValue.ToString()), out int result))
            {
                ConfiguredSet(name, defaultValue.ToString());
                return defaultValue;
            }
            return result;
        }

        public bool ConfiguredGetBoolean(string name, bool defaultValue)
        {
            if(!bool.TryParse(ConfiguredGet(name, defaultValue.ToString()), out bool result))
            {
                ConfiguredSet(name, defaultValue.ToString());
                return defaultValue;
            }
            return result;
        }

        public bool HasNode(string name)
        {
            return Nodes.ContainsKey(name);
        }

        public ConfigNode GetNode(string name)
        {
            if (HasNode(name)) return Nodes[name];
            else
                return AddNode(name);
        }

        public ConfigNode AddNode(string name)
        {
            ConfigNode node = new ConfigNode(name, this, new List<XElement>());
            Nodes.Add(name, node);
            return node;
        }

        public void LoadNode(IEnumerable<XElement> elements)
        {
            foreach (var ele in elements)
            {
                if (ele.Name.ToString().Equals("Option"))
                {
                    string optionName = ele.Attribute("Name").Value;
                    Options[optionName] = new ConfigOption()
                    {
                        Value = ele.Attribute("Value").Value,
                    };
                }
                else if (ele.Name.ToString().Equals("Node"))
                {
                    string nodeName = ele.Attribute("Name").Value;
                    Nodes[nodeName] = new ConfigNode(nodeName, this, ele.Elements());
                }
            }
        }

        private void LoadRootNode()
        {
            try
            {
                Options.Clear();
                Nodes.Clear();
                IEnumerable<XElement> rootElements = XElement.Load(Path).Elements();
                LoadNode(rootElements);
            }
            catch (Exception)
            {
                LoadNode(Save().Elements());
            }
        }

        public XElement Save()
        {
            XElement element = ParentNode?.Save();
            if (element == null)
            {
                if (!Directory.Exists(DirectoryPath))
                {
                    Directory.CreateDirectory(DirectoryPath);
                }
                element = Element();
                element.Save(Path);
            }
            return element;
        }
        public XElement Element()
        {
            XElement element = new XElement("Node", new XAttribute("Name", NodeName));
            foreach (var option in Options)
            {
                element.Add(new XElement(
                    "Option",
                    new XAttribute("Name", option.Key),
                    new XAttribute("Value", option.Value.Value)
                    )
                );
            }
            foreach (var node in Nodes)
            {
                element.Add(node.Value.Element());
            }
            return element;
        }
    }
}
