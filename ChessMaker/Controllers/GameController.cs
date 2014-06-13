using ChessMaker.Models;
using ChessMaker.Services;
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
            model.Variants = VariantService.ListVariants(User.Identity.Name, true);
            model.PromptPlayerSelection = true;
            model.ConfirmText = "Create game";
            model.Heading = "Host private game";
            model.SubmitAction = "CreateOnline";
            return View("SetupGame", model);
        }

        [Authorize]
        public ActionResult Find()
        {
            return View("FindGame", VariantService.ListVariants(User.Identity.Name, false));
        }

        [AllowAnonymous]
        public ActionResult Offline(int? id)
        {
            if (id == null)
            {
                var model = new GameSetupModel();
                model.Variants = VariantService.ListVariants(User.Identity.Name, true);
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
                model.Variants = VariantService.ListVariants(User.Identity.Name, true);
                model.Difficulties = VariantService.ListAiDifficulties();
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
    }
}
