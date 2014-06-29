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

            DefinitionService definitions = GetService<DefinitionService>();
            var model = new BoardShapeModel(version, definitions.GetBoardSVG(version));

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

            DefinitionService definitions = GetService<DefinitionService>();
            definitions.SaveBoardData(version, data);

            if (next == "Done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });

            else if (next == "Links" || next == "Rules" || next == "Pieces" || next == "Layout")
                return RedirectToAction(next, new { id = id });
            else
                return RedirectToAction("Shape", new { id = id });
        }
    }
}
