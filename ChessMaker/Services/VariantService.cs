using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace ChessMaker.Services
{
    public class VariantService : ServiceBase
    {
        public List<VariantSelectionModel> ListPlayableVersions(string currentUser, bool includePrivateVariants)
        {
            var variantList = new List<VariantSelectionModel>();

            var publicVariants = Entities.Variants
                .Where(v => v.PublicVersion != null)
                .OrderBy(v => v.Name)
                .Select(v => v.PublicVersion)
                .ToList();

            foreach (var variant in publicVariants)
                variantList.Add(new VariantSelectionModel(variant));

            if (!includePrivateVariants)
                return variantList;

            var privateVariants = Entities.VariantVersions
                .Where(v => v.Variant.CreatedBy.Name == currentUser)
                .OrderBy(v => v.VariantID)
                .ThenBy(v => v.ID)
                .ToList();

            foreach (var version in privateVariants)
            {
                string customName = string.Format("{0} {1}{2}",
                    version.Variant.Name,
                    DescribeVersion(version),
                    version.Variant.PublicVersionID.HasValue && version.Variant.PublicVersionID == version.ID ? " (public)" : string.Empty
                );
                variantList.Add(new VariantSelectionModel(version, customName));
            }

            return variantList;
        }

        public List<VariantListModel> ListVariants(string currentUser)
        {
            var variantList = new List<VariantListModel>();

            var variants = Entities.Variants
                .Where(v => v.PublicVersion != null || v.CreatedBy.Name == currentUser)
                .OrderBy(v => v.Name)
                .ToList();

            foreach (var variant in variants)
                variantList.Add(new VariantListModel(variant));

            return variantList;
        }

        public List<VariantSelectionModel> ListUserVariants(User user, bool isLoggedInUser)
        {
            List<VariantSelectionModel> versionList = new List<VariantSelectionModel>();

            if (isLoggedInUser)
            {// for current user, use the most up-to-date version of each of their variants, and don't require the variant to be public
                foreach (var variant in user.Variants)
                {
                    var version = Entities.VariantVersions.Where(v => v.VariantID == variant.ID).OrderByDescending(v => v.LastModified).FirstOrDefault();
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

        public Variant CreateNewVariant(User user, VariantEditModel basics)
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

        public static string DescribeVersion(VariantVersion version)
        {
            return string.Format("v{0} @ {1}", version.Number, version.LastModified.ToString("d"));
        }

        public VariantVersion CreateNewVersion(VariantVersion version)
        {
            var newestVersion = version.Variant.AllVersions.OrderByDescending(v => v.Number).FirstOrDefault();
            int versionNum = newestVersion == null ? 1 : newestVersion.Number + 1;

            var newVersion = Entities.Entry(version).GetDatabaseValues().ToObject() as VariantVersion;
            newVersion.LastModified = DateTime.Now;
            newVersion.Number = (short)versionNum;
            Entities.VariantVersions.Add(newVersion);
            Entities.SaveChanges();

            return newVersion;
        }

        public bool CanDelete(VariantVersion version)
        {
            // versions can be deleted only if they are associated with no games at all, active or otherwise.
            // public versions can not be deleted.
            return !version.Games.Any()
                && (!version.Variant.PublicVersionID.HasValue || version.Variant.PublicVersionID != version.ID)
                && version.Variant.AllVersions.Count > 1;
        }

        public VariantEditModel CreateEditModel(Variant variant)
        {
            var model = new VariantEditModel(variant);

            var versions = variant.AllVersions.OrderByDescending(x => x.Number);
            foreach (var version in versions)
                model.Versions.Add(new VersionSelectionModel(version, version.ID == variant.PublicVersionID, CanDelete(version)));

            return model;
        }
    }
}