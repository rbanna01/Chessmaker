using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace ChessMaker.Services
{
    public class VariantService : ServiceBase
    {
        public List<VariantSelectionModel> ListVariants(string currentUser, bool includePrivateVariants)
        {
            var variantList = new List<VariantSelectionModel>();

            var publicVariants = Entities.Variants
                .Where(v => v.PublicVersion != null)
                .OrderBy(v => v.Name)
                .Select(v => v.PublicVersion);

            foreach (var variant in publicVariants)
                variantList.Add(new VariantSelectionModel(variant));

            if (!includePrivateVariants)
                return variantList;

            var privateVariants = Entities.VariantVersions
                .Where(v => v.Variant.CreatedBy.Name == currentUser)
                .OrderBy(v => v.VariantID)
                .ThenBy(v => v.ID);

            foreach (var version in privateVariants)
            {
                string customName = string.Format("{0} v{1} @ {2}{3}",
                    version.Variant.Name,
                    version.Number,
                    version.LastModified.ToString("d"),
                    version.Variant.PublicVersionID.HasValue && version.Variant.PublicVersionID == version.ID ? " (public)" : string.Empty
                );
                variantList.Add(new VariantSelectionModel(version, customName));
            }

            return variantList;
        }

        public List<VariantSelectionModel> ListUserVariants(User user, bool isLoggedInUser)
        {
            List<VariantSelectionModel> versionList = new List<VariantSelectionModel>();

            if (isLoggedInUser)
            {// for current user, use the most up-to-date version of each of their variants, and don't require the variant to be public
                foreach (var variant in user.Variants)
                {
                    var version = Entities.VariantVersions.Where(v => v.VariantID == variant.ID).OrderByDescending(v => v.LastModified).SingleOrDefault();
                    if (version != null)
                        versionList.Add(new VariantSelectionModel(version));
                }
            }
            else
            {// if not current user, only look at public variants, and use the public version only
                var variantVersions = user.Variants.Where(v => v.PublicVersionID.HasValue).Select(v => v.PublicVersion);
                foreach (var version in variantVersions)
                    versionList.Add(new VariantSelectionModel(version));
            }

            return versionList;
        }

        public Variant CreateNewVariant(User user, VariantBasicsModel basics)
        {
            var variant = new Variant();
            variant.CreatedByID = user.ID;
            variant.Name = basics.Name;
            variant.PlayerCount = (byte)basics.NumPlayers;

            var version = new VariantVersion();
            version.Number = 1;
            version.LastModified = DateTime.Now;
            version.Variant = variant;
            version.Definition = "<Temp />";

            Entities.Variants.Add(variant);
            Entities.VariantVersions.Add(version);
            Entities.SaveChanges();

            return variant;
        }

        public List<AIDifficultyModel> ListAiDifficulties()
        {
            var list = new List<AIDifficultyModel>();
            list.Add(new AIDifficultyModel() { ID = 1, Name = "Completely random" });
            return list;
        }

        public VariantVersion GetVersionNumber(Variant variant, int versionID)
        {
            return variant.AllVersions.SingleOrDefault(v => v.Number == versionID);
        }
    }
}