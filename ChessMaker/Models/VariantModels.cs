using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace ChessMaker.Models
{
    public class VariantEditModel
    {
        public VariantEditModel(Variant v)
        {
            VariantID = v.ID;
            Name = v.Name;
            NumPlayers = v.PlayerCount;
            Description = v.Description;
            HelpText = v.HelpText;

            Versions = new List<VersionSelectionModel>();
        }

        public VariantEditModel()
        {
            NumPlayers = 2;
        }

        public int? VariantID { get; set; }

        [Required]
        public string Name { get; set; }

        public string Description { get; set; }
        
        [AllowHtml]
        public string HelpText { get; set; }

        [Required]
        [Display(Name = "# of players")]
        [Range(2,8)]
        public int NumPlayers { get; set; }

        public List<VersionSelectionModel> Versions { get; set; }
    }

    public class VersionSelectionModel
    {
        public VersionSelectionModel(VariantVersion version, bool isPublic, bool canDelete)
        {
            Name = VariantService.DescribeVersion(version);
            VersionID = version.ID;
            IsPublic = isPublic;
            CanDelete = canDelete;
        }

        public string Name { get; set; }
        public int VersionID { get; set; }
        public bool IsPublic { get; set; }
        public bool CanDelete { get; set; }
    }

    public class VariantListModel
    {
        public VariantListModel(Variant variant)
        {
            Name = variant.Name;
            ID = variant.ID;
            IsPublic = variant.PublicVersionID.HasValue;
        }

        public string Name { get; set; }
        public int ID { get; set; }
        public bool IsPublic { get; set; }
    }
}