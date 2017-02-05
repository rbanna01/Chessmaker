using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Text;
using System.Web;

namespace ChessMaker.Services
{
    public class VariantService : ServiceBase
    {
        public List<VariantSelectionModel> ListPlayableVersions(string currentUser)
        {
            var variantList = new List<VariantSelectionModel>();

            var publicVariants = Entities.Variants
                .Where(v => v.PublicVersion != null)
                .OrderBy(v => v.Name)
                .Select(v => v.PublicVersion)
                .ToList();

            foreach (var variant in publicVariants)
                variantList.Add(new VariantSelectionModel(variant));

            if (currentUser == null)
                return variantList;

            var privateVariants = Entities.VariantVersions
                .Where(v => v.Variant.CreatedBy.Name == currentUser && (!v.Variant.PublicVersionID.HasValue || v.Variant.PublicVersionID != v.ID))
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
                variantList.Add(new VariantSelectionModel(version, customName, true));
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

        public List<VariantListModel> GetTenNewestVariants()
        {
            var variantsByNewest = Entities.Database.SqlQuery<string>
                      ("select top 10 v.name from variants v"
                            + " join variantversions vv on v.publicversionid = vv.id"
                            + " order by vv.lastmodified desc"
                       );

            List<VariantListModel> output = new List<VariantListModel>();
            List<string> names = new List<string>();
            IEnumerator<string> enumerator = (IEnumerator<string>)variantsByNewest.GetEnumerator();
            
            while (enumerator.MoveNext())
            {
                names.Add(enumerator.Current);
            }

            enumerator.Dispose();

            foreach(string name in names)
            {
                output.Add(new VariantListModel(Entities.Variants.Where(v => v.Name == name).Single()));
            }
            
            return output;
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
        
        public List<AIDifficultyModel> ListAiDifficulties()
        {
            var list = new List<AIDifficultyModel>();
            list.Add(new AIDifficultyModel() { ID = 0, DisplayName = "Completely random", InternalName = "random" });
            list.Add(new AIDifficultyModel() { ID = 1, DisplayName = "Random capture", InternalName = "random capture" });
            list.Add(new AIDifficultyModel() { ID = 2, DisplayName = "AlphaBeta, depth 2", InternalName = "alpha beta 2" });
            list.Add(new AIDifficultyModel() { ID = 3, DisplayName = "AlphaBeta, depth 3", InternalName = "alpha beta 3" });
            list.Add(new AIDifficultyModel() { ID = 4, DisplayName = "AlphaBeta, depth 4", InternalName = "alpha beta 4" });
            list.Add(new AIDifficultyModel() { ID = 5, DisplayName = "AlphaBeta, depth 5", InternalName = "alpha beta 5" });
            list.Add(new AIDifficultyModel() { ID = 6, DisplayName = "AlphaBeta, depth 6", InternalName = "alpha beta 6" });
            return list;
        }

        public Variant GetByName(string name)
        {
            return Entities.Variants.FirstOrDefault(v => v.Name == name);
        }

        public Variant GetByTag(string tag)
        {
            return Entities.Variants.FirstOrDefault(v => v.Tag == tag);
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



        public IEnumerable<string> GetVariantsByPopularity()
        {
            object[] dummy = new object[1];
            var variantsByPopularity = Entities.Database.SqlQuery<string>
                      ("select concat(v.id, ' ', name, ' ', (count(v.name)))"
                                                    + " from games  g"
                                                    + " join variantversions vv on vv.id = g.variantversionid"
                                                    + " join variants v on vv.variantid = v.id"
                                                    + " group by v.name, v.id"
                       );
            List<string> output = new List<string>();
            IEnumerator<string> enumerator = (IEnumerator < string >) variantsByPopularity.GetEnumerator();

            while (enumerator.MoveNext())
                output.Add(enumerator.Current);

            return output;
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

        public string GenerateTagName(string name)
        {
            StringBuilder sb = new StringBuilder();

            bool skipped = false;
            foreach (char c in name.Trim())
            {
                if (char.IsLetterOrDigit(c))
                {
                    if (skipped)
                    {
                        sb.Append(char.ToUpperInvariant(c));
                        skipped = false;
                    }
                    else
                        sb.Append(c);
                }
                else
                    skipped = true;
            }
            return sb.ToString();
        }
    }
}