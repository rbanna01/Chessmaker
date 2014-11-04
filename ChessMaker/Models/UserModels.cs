using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace ChessMaker.Models
{
    public class UserVariantsModel
    {
        public string UserName { get; set; }
        public bool IsCurrentUser { get; set; }
        public List<VariantSelectionModel> Variants { get; set; }
    }

    public class ThemeModel
    {
        public string BoardLight { get; set; }
        public string BoardMid { get; set; }
        public string BoardDark { get; set; }

        public string BoardLightSelected { get; set; }
        public string BoardMidSelected { get; set; }
        public string BoardDarkSelected { get; set; }

        public string PieceLight { get; set; }
        public string PieceDark { get; set; }
    }
}