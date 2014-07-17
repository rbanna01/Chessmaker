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
            VariantService service = GetService<VariantService>();
            var variants = service.ListVariants(User.Identity.Name);
            return View(variants);
        }
        
        [Authorize]
        public ActionResult New()
        {
            var model = new VariantEditModel();
            return View("Edit", model);
        }

        [Authorize]
        [HttpPost]
        public ActionResult New([Bind(Include = "Name,NumPlayers")] VariantEditModel model)
        {
            if (!ModelState.IsValid)
                return RedirectToAction("New");

            UserService users = GetService<UserService>();
            VariantService variants = GetService<VariantService>();

            var user = users.GetByName(User.Identity.Name);
            var version = variants.CreateNewVariant(user, model);
            return RedirectToAction("Shape", "Designer", new { version.ID });
        }

        [Authorize]
        public ActionResult Edit(int id)
        {
            var variant = Entities().Variants.Find(id);
            if (variant == null)
                return HttpNotFound();
            
            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            VariantService variants = GetService<VariantService>();
            var model = variants.CreateEditModel(variant);
            return View(model);
        }

        [Authorize]
        [HttpPost]
        public ActionResult Edit(int id, [Bind(Include = "Name,NumPlayers")] VariantEditModel model)
        {
            var variant = Entities().Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            if (!ModelState.IsValid)
                return RedirectToAction("Edit", new { id = id });

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            // update the record
            variant.Name = model.Name;
            Entities().SaveChanges();

            return RedirectToAction("Edit", new { id = id });
        }

        [Authorize]
        public ActionResult CopyVersion(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            VariantService variants = GetService<VariantService>();
            var newVersion = variants.CreateNewVersion(version);

            return RedirectToAction("Shape", "Designer", new { id = newVersion.ID });
        }

        [Authorize]
        public ActionResult DeleteVersion(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            int variantID = version.VariantID;

            VariantService variants = GetService<VariantService>();
            if (!variants.CanDelete(version))
                return new HttpUnauthorizedResult();

            Entities().VariantVersions.Remove(version);
            Entities().SaveChanges();

            return RedirectToAction("Edit", new { id = variantID });
        }

        [Authorize]
        public ActionResult MakePublic(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            version.Variant.PublicVersionID = id;
            Entities().SaveChanges();

            return RedirectToAction("Edit", new { id = version.VariantID });
        }

        [Authorize]
        public ActionResult MakePrivate(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();
            
            if (version.Variant.PublicVersionID.HasValue && version.Variant.PublicVersionID == id)
            {
                version.Variant.PublicVersionID = null;
                Entities().SaveChanges();
            }

            return RedirectToAction("Edit", new { id = version.VariantID });
        }
    }
}
