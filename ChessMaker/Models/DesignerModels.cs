using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace ChessMaker.Models
{
    public class VariantOverviewModel
    {
        public VariantOverviewModel(Variant v)
        {
            VariantID = v.ID;
            Name = v.Name;
            NumPlayers = v.PlayerCount;
        }

        public VariantOverviewModel()
        {
            NumPlayers = 2;
        }

        public int? VariantID { get; set; }

        [Required]
        public string Name { get; set; }

        [Required]
        [Display(Name = "# of players")]
        [Range(2,8)]
        public int NumPlayers { get; set; }
    }
}