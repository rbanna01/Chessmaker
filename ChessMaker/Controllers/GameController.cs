using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace ChessMaker.Controllers
{
    public class GameController : Controller
    {
        Entities entities = new Entities();

        public ActionResult Host()
        {
            return View("SetupOnline", GetVariants(true));
        }

        public ActionResult Find()
        {
            return View("FindOnline", GetVariants(false));
        }

        public ActionResult Offline(int? id)
        {
            if (id == null)
                return View("SetupOffline", GetVariants(true));

            var variant = entities.Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            return View("PlayOffline", variant.PublicVersion);
        }

        public ActionResult AI(int? id)
        {
            if (id == null)
                return View("SetupAI", GetVariants(true));

            var variant = entities.Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            return View("PlayAI", variant.PublicVersion);
        }

        public ActionResult Index(int id)
        {
            var game = entities.Games.Find(id);
            if (game == null)
                return HttpNotFound();

            return View("PlayOnline", game);
        }

        private IEnumerable<Tuple<string, int>> GetVariants(bool includePrivate)
        {
            var variants = entities.Variants
                .Where(v => v.PublicVersion != null)
                .OrderBy(v => v.Name);

            var publicVariants = variants.ToList().Select(v => new Tuple<string, int>(v.Name, v.PublicVersionID.Value));

            if (!includePrivate)
                return publicVariants;

            int currentUserID = 1;
            
            var variants2 = entities.VariantVersions
                .Where(v => v.Variant.CreatedByID == currentUserID)
                .OrderBy(v => v.VariantID)
                .ThenBy(v => v.ID);

            var myVariants = variants2.ToList()
                .Select(v => new Tuple<string, int>(
                    string.Format("{0} @ {1}{2}",
                        v.Variant.Name,
                        v.LastModified.ToString("d"),
                        v.Variant.PublicVersionID.HasValue && v.Variant.PublicVersionID == v.ID ? " (public)" : string.Empty
                    ), v.ID));

            return publicVariants.Union(myVariants);
        }
    }
}
