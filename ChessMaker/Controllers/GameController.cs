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
            return View("SetupOnline", GetPublicVariants());
        }

        public ActionResult Find()
        {
            return View("FindOnline", GetPublicVariants());
        }

        public ActionResult Offline(int? id)
        {
            if (id == null)
                return View("SetupOffline", GetPublicVariants());

            var variant = entities.Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            return View("PlayOffline", variant.PublicVersion);
        }

        public ActionResult AI(int? id)
        {
            if (id == null)
                return View("SetupAI", GetPublicVariants());

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

        private IEnumerable<Variant> GetPublicVariants()
        {
            return entities.Variants.Where(v => v.PublicVersion != null).OrderBy(v => v.Name);
        }
    }
}
