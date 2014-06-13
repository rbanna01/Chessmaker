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
}