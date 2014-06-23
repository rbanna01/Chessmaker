using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Net.Http;
using System.Net;

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
            var model = new VariantOverviewModel();
            return View("Overview", model);
        }

        [Authorize]
        [HttpPost]
        public ActionResult New([Bind(Include = "Name,NumPlayers")] VariantOverviewModel model)
        {
            if (!ModelState.IsValid)
                return RedirectToAction("New");

            UserService users = GetService<UserService>();
            VariantService variants = GetService<VariantService>();

            var user = users.GetByName(User.Identity.Name);
            var variant = variants.CreateNewVariant(user, model);
            return RedirectToAction("Overview", new { variant.ID });
        }

        [Authorize]
        [HttpPost]
        public ActionResult Update(int id, [Bind(Include = "Name,NumPlayers")] VariantOverviewModel model)
        {
            var variant = Entities().Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            if (!ModelState.IsValid)
                return RedirectToAction("Overview", new { id = id });
            
            UserService users = GetService<UserService>();
            var user = users.GetByName(User.Identity.Name);

            // check if this user is allowed to edit this variant
            if (variant.CreatedByID != user.ID)
                return new HttpUnauthorizedResult();

            // update the record
            variant.Name = model.Name;
            Entities().SaveChanges();

            return RedirectToAction("Overview", new { id });
        }

        [Authorize]
        public ActionResult Overview(int id)
        {
            var variant = Entities().Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            var model = new VariantOverviewModel(variant);
            return View(model);
        }
    }
}
