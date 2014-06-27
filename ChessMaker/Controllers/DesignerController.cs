using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Net.Http;
using System.Net;
using System.Xml;

namespace ChessMaker.Controllers
{
    public class DesignerController : ControllerBase
    {
        [Authorize]
        public ActionResult Shape(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            var model = new BoardShapeModel(version);

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Shape(int id, string data, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            // do something with "data" value (from hidden field) to save the changes
            // this is of course, bollocks. We don't want to be saving/loading the actual SVG.
            // this also lacks error handling, validation, etc.
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(data);
            doc.DocumentElement.RemoveAttribute("style");
            version.Definition = doc.OuterXml;
            Entities().SaveChanges();

            if (next == "Done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });

            else if (next == "Links" || next == "Rules" || next == "Pieces" || next == "Layout")
                return RedirectToAction(next, new { id = id });
            else
                return RedirectToAction("Shape", new { id = id });
        }
    }
}
