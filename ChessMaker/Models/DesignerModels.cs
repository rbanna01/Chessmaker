using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace ChessMaker.Models
{
    public class BoardShapeModel
    {
        public BoardShapeModel(VariantVersion version)
        {
            VariantName = version.Variant.Name;
            SvgData = version.Definition;
        }

        public string VariantName { get; set; }
        public string SvgData { get; set; }
    }
}