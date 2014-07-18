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
            var model = new BoardShapeModel(version, definitions.GetBoardSVG(version), definitions.GetCellLinks(version));

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Shape(int id, string shapeData, string linkData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            definitions.SaveBoardData(version, shapeData, linkData);

            if (next == "Done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });

            else if (next == "Links" || next == "Rules" || next == "Pieces" || next == "Layout")
                return RedirectToAction(next, new { id = id });
            else
                return RedirectToAction("Shape", new { id = id });
        }

        [Authorize]
        public ActionResult Links(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            var model = new BoardLinksModel(version, definitions.GetBoardSVG(version, true), definitions.GetCellLinks(version));

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Links(int id, string shapeData, string linkData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();
            /*
            DefinitionService definitions = GetService<DefinitionService>();
            definitions.SaveBoardData(version, shapeData, linkData);
            */
            if (next == "Done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });

            else if (next == "Shape" || next == "Rules" || next == "Pieces" || next == "Layout")
                return RedirectToAction(next, new { id = id });
            else
                return RedirectToAction("Links", new { id = id });
        }
    }
}
