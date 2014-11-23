using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Web.Routing;
using System.Web.Security;
using System.Xml;
using WebMatrix.WebData;

namespace ChessMaker.Controllers
{
    public class GameController : ControllerBase
    {
        [AllowAnonymous]
        public ActionResult New()
        {
            VariantService variants = GetService<VariantService>();
            var model = PopulateNewGameModel(variants);
            return View(model);
        }

        [HttpPost]
        [ValidateInput(true)]
        public ActionResult New(GameMode modeSelect, int? variantSelect, int? aiSelect, string[] opponent)
        {
            VariantService variants = GetService<VariantService>();
            VariantVersion version;
            List<User> opponentUsers;
            AIDifficultyModel aiDifficulty;
            if (!ValidateNewGame(modeSelect, variantSelect, aiSelect, opponent, variants, out version, out opponentUsers, out aiDifficulty))
            {
                var model = PopulateNewGameModel(variants);
                return View(model);
            }

            var routeValues = new RouteValueDictionary();
            routeValues.Add("ID", version.Variant.Tag);

            if (!version.Variant.PublicVersionID.HasValue || version.Variant.PublicVersionID != version.ID)
                routeValues.Add("version", version.Number);

            switch (modeSelect)
            {
                case GameMode.Offline:
                    return RedirectToAction("Offline", routeValues);

                case GameMode.AI:
                    routeValues.Add("difficulty", aiDifficulty.ID);
                    return RedirectToAction("AI", routeValues);

                default:
                    throw new NotImplementedException("Not yet implemented creating " + modeSelect + " games");
            }
        }


        private NewGameModel PopulateNewGameModel(VariantService service)
        {
            var model = new NewGameModel();
            model.Variants = service.ListPlayableVersions(User.Identity.Name);
            model.Difficulties = service.ListAiDifficulties();
            model.AllowOnlinePlay = User.Identity.IsAuthenticated;
            return model;
        }

        private bool ValidateNewGame(GameMode modeSelect, int? variantSelect, int? aiSelect, string[] opponent, VariantService variants, out VariantVersion version, out List<User> opponentUsers, out AIDifficultyModel aiDifficulty)
        {
            opponentUsers = null;
            aiDifficulty = default(AIDifficultyModel);

            // is variant version ID valid?
            version = variantSelect.HasValue ? Entities().VariantVersions.Find(variantSelect.Value) : null;
            if (version == null)
            {
                ModelState.AddModelError("variantSelect", "Please select a variant.");
                return false;
            }

            // can user access variant version?
            UserService users = GetService<UserService>();
            if (!users.IsAllowedToPlay(version, User.Identity.Name))
            {
                ModelState.AddModelError("variantSelect", "Cannot access selected variant version.");
                return false;
            }

            // if game mode is public, is variant version public?
            if (modeSelect == GameMode.Public && (!version.Variant.PublicVersionID.HasValue || version.Variant.PublicVersionID != version.Variant.PublicVersionID))
            {
                ModelState.AddModelError("variantSelect", "Public games cannot use a private variant.");
                return false;
            }

            // if game mode is AI, is difficulty selection valid?
            aiDifficulty = aiSelect.HasValue ? variants.ListAiDifficulties().SingleOrDefault(ai => ai.ID == aiSelect.Value) : null;
            if (modeSelect == GameMode.AI && aiDifficulty == null)
            {
                ModelState.AddModelError("aiSelect", "Invalid AI difficulty.");
                return false;
            }

            // if game mode is private, are all opponent fields filled in, valid users, not the current user, and not duplicated?
            if (modeSelect == GameMode.Private)
            {
                if (opponent.Length != version.Variant.PlayerCount - 1 || opponent.Count(o => string.IsNullOrWhiteSpace(o)) != 0)
                {
                    ModelState.AddModelError("opponent", string.Format(version.Variant.Name + " requires {0} players, so you must specify {1} opponent{2}.", version.Variant.PlayerCount, version.Variant.PlayerCount - 1, version.Variant.PlayerCount == 2 ? string.Empty : "s"));
                    return false;
                }

                if (opponent.FirstOrDefault(o => string.Equals(o.Trim(), User.Identity.Name, StringComparison.InvariantCultureIgnoreCase)) != null)
                {
                    ModelState.AddModelError("opponent", "You cannot enter your own name as an opponent.");
                    return false;
                }

                opponentUsers = new List<User>(opponent.Length);
                foreach (var o in opponent)
                {
                    User u = users.GetByName(o.Trim());
                    if (u == null)
                    {
                        ModelState.AddModelError("opponent", "Not a valid user name: " + o);
                        return false;
                    }

                    if (opponentUsers.FirstOrDefault(x => string.Equals(x.Name, u.Name, StringComparison.InvariantCultureIgnoreCase)) != null)
                    {
                        ModelState.AddModelError("opponent", "You cannot enter the same opponent multiple times");
                        return false;
                    }

                    opponentUsers.Add(u);
                }
            }

            return true;
        }

        private VariantVersion DeterminePlayVersion(string variantTag, int? versionID, VariantService variants)
        {
            var variant = variants.GetByTag(variantTag);
            
            if (variant == null)
                return null;

            if (versionID.HasValue && variant.CreatedBy.Name == User.Identity.Name)
            {
                VariantVersion version = variants.GetVersionNumber(variant, versionID.Value);

                if (version == null || version.VariantID != variant.ID)
                    return null;

                return version;
            }
            else
                return variant.PublicVersion;
        }

        [AllowAnonymous]
        public ActionResult Definition(string id, int? version)
        {
            VariantService variants = GetService<VariantService>();
            VariantVersion versionToPlay = DeterminePlayVersion(id, version, variants);

            if (versionToPlay == null)
                return HttpNotFound("Cannot determine variant version to play");

            return Content(versionToPlay.Definition, "text/xml");
        }

        [AllowAnonymous]
        public ActionResult Offline(string id, int? version)
        {
            VariantService variants = GetService<VariantService>();
            VariantVersion versionToPlay = DeterminePlayVersion(id, version, variants);
            
            if (versionToPlay == null)
                return HttpNotFound("Cannot determine variant version to play");

            var model = new GamePlayModel(versionToPlay, GameMode.Offline);
            return View("Play", model);
        }

        [AllowAnonymous]
        public ActionResult AI(string id, int? version, int difficulty)
        {
            VariantService variants = GetService<VariantService>();
            VariantVersion versionToPlay = DeterminePlayVersion(id, version, variants);

            if (versionToPlay == null)
                return HttpNotFound("Cannot determine variant version to play");

            var model = new GamePlayModel(versionToPlay, GameMode.AI);
            model.AIs = new[] { null, variants.ListAiDifficulties().Single(ai => ai.ID == difficulty) };

            return View("Play", model);
        }

        [AllowAnonymous]
        public ActionResult AI_vs_AI(string id, int? version, int[] difficulty)
        {
            VariantService variants = GetService<VariantService>();
            VariantVersion versionToPlay = DeterminePlayVersion(id, version, variants);

            if (versionToPlay == null)
                return HttpNotFound("Cannot determine variant version to play");

            var model = new GamePlayModel(versionToPlay, GameMode.AI_vs_AI);

            var AIs = new AIDifficultyModel[versionToPlay.Variant.PlayerCount];
            for (var i = 0; i < AIs.Length && i < difficulty.Length; i++)
                AIs[i] = variants.ListAiDifficulties().Single(ai => ai.ID == difficulty[i]);

            for (var i = difficulty.Length; i < AIs.Length; i++)
                AIs[i] = AIs[difficulty.Length];

            model.AIs = AIs;
            return View("Play", model);
        }
/*
        [Authorize]
        public ActionResult Find()
        {
            VariantService service = GetService<VariantService>();
            return View("FindGame", service.ListPlayableVersions(User.Identity.Name));
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
