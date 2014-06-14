using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace ChessMaker.Models
{
    public class VariantBasicsModel
    {
        public VariantBasicsModel(Variant v)
        {
            Name = v.Name;
            NumPlayers = v.PlayerCount;
        }

        public VariantBasicsModel()
        {
            NumPlayers = 2;
        }

        [Required]
        public string Name { get; set; }

        [Required]
        [Display(Name = "# of players")]
        [Range(2,8)]
        public int NumPlayers { get; set; }
    }
}