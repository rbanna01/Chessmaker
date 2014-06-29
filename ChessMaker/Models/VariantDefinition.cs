using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Xml;

namespace ChessMaker.Models
{
    public class VariantDefinition
    {
        public VariantDefinition(VariantVersion version)
        {
            XmlDocument doc = new XmlDocument();
            if (version.Definition != string.Empty)
                doc.LoadXml(version.Definition);
            else
                doc.LoadXml("<game><board viewBox=\"0 0 100 100\"/></game>");
            Version = version;
            Xml = doc;
        }

        public XmlElement CreateElement(string name) { return Xml.CreateElement(name); }
        public XmlAttribute CreateAttribute(string name) { return Xml.CreateAttribute(name); }
        public XmlAttribute CreateAttribute(string name, string value)
        {
            var attr = Xml.CreateAttribute(name);
            attr.Value = value;
            return attr;
        }

        private VariantVersion Version { get; set; }
        private XmlDocument Xml { get; set; }

        private void Changed()
        {
            Version.Definition = Xml.OuterXml;
        }

        public XmlElement Board
        {
            get
            {
                return Xml.DocumentElement["board"];
            }
            set
            {
                Xml.DocumentElement.ReplaceChild(value, Board);
                Changed();
            }
        }
    }
}