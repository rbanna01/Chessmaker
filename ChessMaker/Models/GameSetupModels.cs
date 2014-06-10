using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Data.Entity;
using System.Globalization;
using System.Web.Security;

namespace ChessMaker.Models
{
    public class GameSetupModel
    {
        public List<VariantSelectionModel> Variants { get; set; }
        public bool SelectAI { get; set; }
        public bool PromptPlayerSelection { get; set; }
        public string Heading { get; set; }
        public string ConfirmText { get; set; }
        public string SubmitAction { get; set; }
    }

    public class VariantSelectionModel
    {
        public string Name { get; set; }
        public int VersionID { get; set; }

        //public string Author { get; set; }
        //public string Description { get; set; }

        //[Display(Name = "Players")]
        //public int NumPlayers { get; set; }
    }
}
