using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Data.Entity;
using System.Globalization;
using System.Web.Security;
using System.Xml;

namespace ChessMaker.Models
{
    public enum GameMode
    {
        Public,
        Private,
        Offline,
        AI,
        AI_vs_AI,
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

    public class PlayerModel
    {
        public bool IsLocal { get; set; }
        public virtual bool IsAI { get { return false; } }
    }

    public class AIDifficultyModel : PlayerModel
    {
        public int ID { get; set; }
        public string DisplayName { get; set; }
        public string InternalName { get; set; }
        public override bool IsAI { get { return true; } }
    }

    public class GamePlayModel
    {
        public GamePlayModel(VariantVersion version, GameMode mode)
        {
            Tag = version.Variant.Tag;

            if (version.Variant.PublicVersionID.HasValue && version.ID == version.Variant.PublicVersionID)
                Name = version.Variant.Name;
            else
            {
                Name = string.Format("{0} v{1}", version.Variant.Name, version.Number);
                Version = version.Number;
            }

            Mode = mode;
        }

        public string Name { get; private set; }
        public string Tag { get; private set; }
        public int? Version { get; private set; }

        public GameMode Mode { get; set; }
        public PlayerModel[] Players { get; set; }
    }
}
