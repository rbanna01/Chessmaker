using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Data.Entity;
using System.Globalization;
using System.Web.Security;

namespace ChessMaker.Models
{
    public enum GameMode
    {
        Public,
        Private,
        Local,
        AI,
    }

    public class NewGameModel
    {
        public List<VariantSelectionModel> Variants { get; set; }
        public List<AIDifficultyModel> Difficulties { get; set; }
        public bool AllowOnlinePlay { get; set; }
    }

    public class VariantSelectionModel
    {
        public VariantSelectionModel(VariantVersion version)
        {
            Name = version.Variant.Name;
            VariantID = version.VariantID;
            VersionID = version.ID;
            NumPlayers = version.Variant.PlayerCount;
        }

        public VariantSelectionModel(VariantVersion version, string customName, bool isPrivate)
            : this(version)
        {
            Name = customName;
            IsPrivate = isPrivate;
        }

        public VariantSelectionModel() { }

        public string Name { get; set; }
        public int VariantID { get; set; }
        public int VersionID { get; set; }
        public bool IsPrivate { get; set; }

        //public string Author { get; set; }
        //public string Description { get; set; }

        public int NumPlayers { get; set; }
    }

    public class AIDifficultyModel
    {
        public int ID { get; set; }
        public string Name { get; set; }
    }

    public class GamePlayModel
    {
        public GamePlayModel(VariantVersion version, GameMode mode)
        {
            if (version.Variant.PublicVersionID.HasValue && version.ID == version.Variant.PublicVersionID)
                Name = version.Variant.Name;
            else
                Name = string.Format("{0} v{1}", version.Variant.Name, version.Number);

            Mode = mode;
            SchemaDefinition = version.Definition;
        }

        public string Name { get; set; }
        public GameMode Mode { get; set; }
        public AIDifficultyModel AI { get; set; }
        public string SchemaDefinition { get; private set; }
    }
}
