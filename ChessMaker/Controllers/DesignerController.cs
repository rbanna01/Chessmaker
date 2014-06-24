using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Net.Http;
using System.Net;

namespace ChessMaker.Controllers
{
    public class DesignerController : ControllerBase
    {
        [Authorize]
        public ActionResult Shape(int id)
        {
            var variant = Entities().Variants.Find(id);
            if (variant == null)
                return HttpNotFound();

            UserService users = GetService<UserService>();
            if (!users.IsAllowedToEdit(variant, User.Identity.Name))
                return new HttpUnauthorizedResult();

            return View();
        }
    }
}
