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
    public class GameController : ControllerBase
    {
        [AllowAnonymous]
        public ActionResult New()
        {
            VariantService service = GetService<VariantService>();

            var model = new NewGameModel();
            model.Variants = service.ListPlayableVersions(User.Identity.Name);
            model.Difficulties = service.ListAiDifficulties();
            model.AllowOnlinePlay = User.Identity.IsAuthenticated;

            var variantOptions = model.Variants.Select(m => string.Format("<option value=\"{1}\" numPlayers=\"{2}\"{3}>{0}</option>", m.Name, m.VersionID, m.NumPlayers, m.IsPrivate ? " private=\"private\"" : string.Empty));
            return View(model);
        }

/*
        [Authorize]
        public ActionResult Find()
        {
            VariantService service = GetService<VariantService>();
            return View("FindGame", service.ListPlayableVersions(User.Identity.Name));
        }

        [AllowAnonymous]
        public ActionResult Offline(int id, int? version)
        {
            VariantService service = GetService<VariantService>();

            var variant = Entities().Variants.Find(id);
            
            if ( variant == null)
                return HttpNotFound();

            VariantVersion versionToPlay;
            if (version.HasValue && variant.CreatedBy.Name == User.Identity.Name)
            {
                versionToPlay = service.GetVersionNumber(variant, version.Value);
            }
            else
                versionToPlay = variant.PublicVersion;

            if (versionToPlay == null)
                return HttpNotFound();

            return View("PlayOffline", versionToPlay);
        }

        [AllowAnonymous]
        public ActionResult AI(int id, int? difficulty)
        {
            VariantService service = GetService<VariantService>();

            var version = Entities().VariantVersions.Find(id);
            if (version == null)
                return HttpNotFound();

            return View("PlayAI", version);
        }

        [Authorize]
        [HttpPost]
        public ActionResult CreateOnline(int variantSelect)
        {
            var version = Entities().VariantVersions.Find(variantSelect);
            if (version == null)
                return HttpNotFound();

            Game game = new Game();
            game.VariantVersion = version;
            game.Status = Entities().GameStatuses.First(s => s.Name == "Private setup");
            Entities().Games.Add(game);
            
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
            var game = Entities().Games.Find(id);
            if (game == null)
                return HttpNotFound();

            if (game.Status.Name == "Private setup")
                return View("Lobby", game);

            return View("PlayOnline", game);
        }
*/
    }
}
