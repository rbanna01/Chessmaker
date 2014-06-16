using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace ChessMaker.Controllers
{
    public class VariantsController : ControllerBase
    {
        public ActionResult Index()
        {
            var variants = Entities().Variants.Where(v => v.PublicVersionID.HasValue).ToList();
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
            if (!ModelState.IsValid)
                return RedirectToAction("New");

            UserService users = GetService<UserService>();
            VariantService variants = GetService<VariantService>();

            var user = users.GetByName(User.Identity.Name);
            var variant = variants.CreateNewVariant(user, model);
            return RedirectToAction("Basics", new { variant.ID });
        }

        [Authorize]
        public ActionResult Basics(int id)
        {
            var variant = Entities().Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            var model = new VariantBasicsModel(variant);
            return View(model);
        }
    }
}
