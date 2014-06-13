using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Web.Security;
using WebMatrix.WebData;

namespace ChessMaker.Controllers
{
    public class GameController : Controller
    {
        Entities entities = new Entities();
        
        [Authorize]
        public ActionResult Host()
        {
            var model = new GameSetupModel();
            model.Variants = ListVariants(true);
            model.PromptPlayerSelection = true;
            model.ConfirmText = "Create game";
            model.Heading = "Host private game";
            model.SubmitAction = "CreateOnline";
            return View("SetupGame", model);
        }

        [Authorize]
        public ActionResult Find()
        {
            return View("FindGame", ListVariants(false));
        }

        [AllowAnonymous]
        public ActionResult Offline(int? id)
        {
            if (id == null)
            {
                var model = new GameSetupModel();
                model.Variants = ListVariants(true);
                model.ConfirmText = "Play offline";
                model.Heading = "Setup offline game";
                model.SubmitAction = "CreateOffline";
                return View("SetupGame", model);
            }

            var variant = entities.Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            return View("PlayOffline", variant.PublicVersion);
        }

        [AllowAnonymous]
        public ActionResult AI(int? id, int? difficulty)
        {
            if (id == null)
            {
                var model = new GameSetupModel();
                model.Variants = ListVariants(true);
                model.Difficulties = ListAiDifficulties();
                model.ConfirmText = "Play AI";
                model.Heading = "Setup game vs AI";
                model.SubmitAction = "CreateAI";
                return View("SetupGame", model);
            }

            var version = entities.VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            return View("PlayAI", version);
        }

        [Authorize]
        [HttpPost]
        public ActionResult CreateOnline(int variantSelect)
        {
            var version = entities.VariantVersions.Find(variantSelect);
            if (version == null)
                return HttpNotFound();

            Game game = new Game();
            game.VariantVersion = version;
            game.Status = entities.GameStatuses.First(s => s.Name == "Private setup");
            entities.Games.Add(game);
            
            return RedirectToAction("Index", new { id = game.ID});
        }

        [AllowAnonymous]
        [HttpPost]
        public ActionResult CreateOffline(int variantSelect)
        {
            return RedirectToAction("Offline", new { id = variantSelect });
        }

        [AllowAnonymous]
        [HttpPost]
        public ActionResult CreateAI(int variantSelect, int aiSelect)
        {
            return RedirectToAction("AI", new { id = variantSelect, difficulty = aiSelect });
        }

        [AllowAnonymous]
        public ActionResult Index(int id)
        {
            var game = entities.Games.Find(id);
            if (game == null)
                return HttpNotFound();

            if (game.Status.Name == "Private setup")
                return View("Lobby", game);

            return View("PlayOnline", game);
        }

        private List<VariantSelectionModel> ListVariants(bool includePrivateVariants)
        {
            var variantList = new List<VariantSelectionModel>();

            var publicVariants = entities.Variants
                .Where(v => v.PublicVersion != null)
                .OrderBy(v => v.Name)
                .Select(v => v.PublicVersion);

            foreach (var variant in publicVariants)
                variantList.Add(new VariantSelectionModel(variant));

            if (!includePrivateVariants)
                return variantList;

            var privateVariants = entities.VariantVersions
                .Where(v => v.Variant.CreatedBy.Name == User.Identity.Name)
                .OrderBy(v => v.VariantID)
                .ThenBy(v => v.ID);

            foreach (var version in privateVariants)
            {
                string customName = string.Format("{0} @ {1}{2}",
                    version.Variant.Name,
                    version.LastModified.ToString("d"),
                    version.Variant.PublicVersionID.HasValue && version.Variant.PublicVersionID == version.ID ? " (public)" : string.Empty
                );
                variantList.Add(new VariantSelectionModel(version, customName));
            }

            return variantList;
        }

        private List<AIDifficultyModel> ListAiDifficulties()
        {
            var list = new List<AIDifficultyModel>();
            list.Add(new AIDifficultyModel() { ID = 1, Name = "Completely random" });
            return list;
        }
    }
}
