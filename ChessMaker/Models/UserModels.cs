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
        public string Light { get; set; }
        public string Mid { get; set; }
        public string Dark { get; set; }

        public string LightSelected { get; set; }
        public string MidSelected { get; set; }
        public string DarkSelected { get; set; }
    }
}