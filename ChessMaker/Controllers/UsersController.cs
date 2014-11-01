﻿using ChessMaker.Models;
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

            model.Light = /*user.ThemeColorLight ??*/ "ffffff";
            model.Mid = /*user.ThemeColorMid ??*/ "7f7f7f";
            model.Dark = /*user.ThemeColorDark ??*/ "000000";

            model.LightSelected = /*user.ThemeColorLightSelected ??*/ "ffabab";
            model.MidSelected = /*user.ThemeColorMidSelected ??*/ "aa5656";
            model.DarkSelected = /*user.ThemeColorDarkSelected ??*/ "540000";

            Response.ContentType = "text/css";
            return View(model);
        }
    }
}
