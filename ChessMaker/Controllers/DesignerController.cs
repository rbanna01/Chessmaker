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

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });
            else if (next == "next")
                return RedirectToAction("Global", new { id });
            
            return RedirectToAction("Shape", new { id });
        }

        [Authorize]
        public ActionResult Global(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            var model = new GlobalDirectionsModel(version, definitions.GetBoardSVG(version, true), definitions.GetCellLinks(version));

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Global(int id, string linkData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();
            
            DefinitionService definitions = GetService<DefinitionService>();
            definitions.SaveLinkData(version, linkData);

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });
            else if (next == "prev")
                return RedirectToAction("Shape", new { id });
            else if (next == "next")
                return RedirectToAction("Relative", new { id });

            return RedirectToAction("Global", new { id });
        }

        [Authorize]
        public ActionResult Relative(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            var model = new RelativeDirectionsModel(version, definitions.CalculateGlobalDirectionDiagram(version), definitions.GetRelativeDirs(version));

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Relative(int id, string relData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            definitions.SaveRelativeDirs(version, relData);

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });
            else if (next == "prev")
                return RedirectToAction("Global", new { id });
            else if (next == "next")
                return RedirectToAction("Groups", new { id });

            return RedirectToAction("Relative", new { id });
        }
    }
}
