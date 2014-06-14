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
    public class UsersController : Controller
    {
        public ActionResult Index(string id)
        {
            return View(UserService.ListAll());
        }

        public new ActionResult Profile(string id)
        {
            if (id == null)
                return HttpNotFound();

            var selectedUser = UserService.GetByName(id);
            if (selectedUser == null)
                return HttpNotFound();

            return View(selectedUser);
        }

        public ActionResult Variants(string id)
        {
            if (id == null)
                return HttpNotFound();

            var selectedUser = UserService.GetByName(id);
            if (selectedUser == null)
                return HttpNotFound();

            var model = new UserVariantsModel() { UserName = selectedUser.Name };
            model.IsCurrentUser = Request.IsAuthenticated && selectedUser.Name == User.Identity.Name;
            model.Variants = VariantService.ListUserVariants(selectedUser, model.IsCurrentUser);

            return View(model);
        }
    }
}
