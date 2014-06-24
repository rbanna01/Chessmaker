using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace ChessMaker.Models
{
    public class VariantEditModel
    {
        public VariantEditModel(Variant v)
        {
            VariantID = v.ID;
            Name = v.Name;
            NumPlayers = v.PlayerCount;

            Versions = new List<VersionSelectionModel>();
            var versions = v.AllVersions.OrderByDescending(x => x.Number);
            foreach (var version in versions)
                Versions.Add(new VersionSelectionModel(version, version.ID == v.PublicVersionID));
        }

        public VariantEditModel()
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

        public List<VersionSelectionModel> Versions { get; set; }
    }

    public class VersionSelectionModel
    {
        public VersionSelectionModel(VariantVersion version, bool isPublic)
        {
            Name = VariantService.DescribeVersion(version);
            VersionID = version.ID;
            IsPublic = isPublic;
        }

        public string Name { get; set; }
        public int VersionID { get; set; }
        public bool IsPublic { get; set; }
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