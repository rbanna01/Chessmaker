using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;

namespace ChessMaker.Controllers
{
    public class UsersController : ControllerBase
    {
        public ActionResult Index(string id)
        {
            UserService users = GetService<UserService>();
            return View(users.ListAll());
        }

        public new ActionResult Profile(string id)
        {
            if (id == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            var selectedUser = users.GetByName(id);
            if (selectedUser == null)
                return HttpNotFound();

            return View(selectedUser);
        }

        public ActionResult Variants(string id)
        {
            if (id == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            var selectedUser = users.GetByName(id);
            if (selectedUser == null)
                return HttpNotFound();

            VariantService variants = GetService<VariantService>();

            var model = new UserVariantsModel() { UserName = selectedUser.Name };
            model.IsCurrentUser = Request.IsAuthenticated && selectedUser.Name == User.Identity.Name;
            model.Variants = variants.ListUserVariants(selectedUser, model.IsCurrentUser);

            return View(model);
        }

        public ActionResult Theme()
        {
            UserService users = GetService<UserService>();
            var user = users.GetByName(User.Identity.Name);

            var model = new ThemeModel();

            // here's brown-ish low-contrast theme, as used on wikipedia. Looks good, but doens't go with the blue.
            // ffce9e / e8ad73 / d18b47
            // ff4535 / f73a26 / f02e18

            model.Light = /*user.ThemeColorLight ??*/ "d0d0d0";
            model.Mid = /*user.ThemeColorMid ??*/ "878787";
            model.Dark = /*user.ThemeColorDark ??*/ "505050";

            model.LightSelected = /*user.ThemeColorLightSelected ??*/ "ffa0a0";
            model.MidSelected = /*user.ThemeColorMidSelected ??*/ "ff8787";
            model.DarkSelected = /*user.ThemeColorDarkSelected ??*/ "ff6060";

            Response.ContentType = "text/css";
            return View(model);
        }
    }
}
