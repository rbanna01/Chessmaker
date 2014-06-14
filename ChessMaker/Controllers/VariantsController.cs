using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace ChessMaker.Controllers
{
    public class VariantsController : Controller
    {
        Entities entities = new Entities();

        public ActionResult Index()
        {
            var variants = entities.Variants.Where(v => v.PublicVersionID.HasValue).ToList();
            return View(variants);
        }
        
        [Authorize]
        public ActionResult New()
        {
            var model = new VariantBasicsModel();
            return View("Basics", model);
        }

        [Authorize]
        [HttpPost]
        public ActionResult New([Bind(Include = "Name,NumPlayers")] VariantBasicsModel model)
        {
            if (ModelState.IsValid)
            {
                var user = UserService.GetByName(User.Identity.Name);
                var variant = VariantService.CreateNewVariant(user, model);
                return RedirectToAction("Basics", new { variant.ID });
            }
            return RedirectToAction("New");
        }

        [Authorize]
        public ActionResult Basics(int id)
        {
            var variant = entities.Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            var model = new VariantBasicsModel(variant);
            return View(model);
        }
    }
}
