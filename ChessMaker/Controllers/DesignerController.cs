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
                return RedirectToAction("Dirs1", new { id });
            
            return RedirectToAction("Shape", new { id });
        }

        [Authorize]
        public ActionResult Dirs1(int id)
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
        public ActionResult Dirs1(int id, string linkData, string next)
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
                return RedirectToAction("Dirs2", new { id });

            return RedirectToAction("Dirs1", new { id });
        }

        [Authorize]
        public ActionResult Dirs2(int id)
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
        public ActionResult Dirs2(int id, string relData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            // do some savey stuff

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });
            else if (next == "prev")
                return RedirectToAction("Dirs1", new { id });
            else if (next == "next")
                return RedirectToAction("Pieces", new { id });

            return RedirectToAction("Dirs2", new { id });
        }
    }
}
