using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Xml;

namespace ChessMaker.Models
{
    public class BoardShapeModel
    {
        public BoardShapeModel(VariantVersion version, XmlDocument boardSvg)
        {
            VariantName = version.Variant.Name;
            SvgData = boardSvg.OuterXml;
        }

        public string VariantName { get; set; }
        public string SvgData { get; set; }
    }
}