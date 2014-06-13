using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;

namespace ChessMaker.Controllers
{
    public class UsersController : Controller
    {
        Entities entities = new Entities();

        public ActionResult Index(int? id)
        {
            if (id == null)
                return View(entities.Users.ToList());
            
            var selectedUser = entities.Users.Find(id);
            if (selectedUser == null)
                return HttpNotFound();

            return View("View", selectedUser);
        }

        public ActionResult Variants(int id)
        {
            var selectedUser = entities.Users.Find(id);
            if (selectedUser == null)
                return HttpNotFound();

            var model = new UserVariantsModel() { UserName = selectedUser.Name, Variants = new List<VariantSelectionModel>() };
            model.IsCurrentUser = Request.IsAuthenticated && selectedUser.Name == User.Identity.Name;
            
            if (model.IsCurrentUser)
            {// for current user, use the most up-to-date version of each of their variants, and don't require the variant to be public
                var variants = selectedUser.Variants.Where(v => v.CreatedBy == selectedUser);
                foreach (var variant in variants)
                {
                    var version = entities.VariantVersions.Where(v => v.VariantID == variant.ID).OrderByDescending(v => v.LastModified).Single();
                    model.Variants.Add(new VariantSelectionModel(version));
                }
            }
            else
            {// if not current user, only look at public variants, and use the public version only
                var variantVersions = selectedUser.Variants.Where(v => v.CreatedBy == selectedUser && v.PublicVersionID.HasValue).Select(v => v.PublicVersion);
                foreach (var version in variantVersions)
                    model.Variants.Add(new VariantSelectionModel(version));
            }

            return View(model);
        }
    }
}
