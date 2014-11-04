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
        public ActionResult Raw(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            var model = new RawModel(version);

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Raw(int id, string xmlData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            string errors;
            if (!definitions.ValidateXml(xmlData, out errors))
            {
                ModelState.AddModelError("data", "Error validating XML data: " + errors);

                var model = new RawModel(version);
                model.XmlData = xmlData;
                return View(model);
            }

            version.Definition = xmlData;
            Entities().SaveChanges();

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });

            return RedirectToAction("Raw", new { id });
        }

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

        [Authorize]
        public ActionResult Groups(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();
            
            DefinitionService definitions = GetService<DefinitionService>();
            var dirs = definitions.ListGlobalDirections(version);
            var model = new DirectionGroupsModel(version, string.Join(";", dirs), definitions.GetDirectionGroups(version));

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Groups(int id, string groupData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            definitions.SaveDirectionGroups(version, groupData);

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });
            else if (next == "prev")
                return RedirectToAction("Relative", new { id });
            else if (next == "next")
                return RedirectToAction("References", new { id });

            return RedirectToAction("Groups", new { id });
        }

        [Authorize]
        public ActionResult References(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            var dirs = definitions.ListGlobalDirections(version);
            var model = new CellReferencesModel(version, definitions.GetBoardSVG(version, false, true), definitions.GetCellLinks(version), dirs);

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult References(int id, string renameData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            definitions.SaveCellReferenceChanges(version, renameData);

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });
            else if (next == "prev")
                return RedirectToAction("Groups", new { id });
            else if (next == "next")
                return RedirectToAction("Pieces", new { id });

            return RedirectToAction("References", new { id });
        }

        [Authorize]
        public ActionResult Pieces(int id)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            var model = new PieceDefinitionsModel(version, definitions.GetPieceDefinitionXML(version));

            return View(model);
        }

        [Authorize]
        [HttpPost]
        [ValidateInput(false)]
        public ActionResult Pieces(int id, string pieceData, string next)
        {
            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(version.Variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            DefinitionService definitions = GetService<DefinitionService>();
            // do savey-type stuff

            if (next == "done")
                return RedirectToAction("Edit", "Variants", new { id = version.VariantID });
            else if (next == "prev")
                return RedirectToAction("References", new { id });
            else if (next == "next")
                return RedirectToAction("Layout", new { id });

            return RedirectToAction("Pieces", new { id });
        }
    }
}
