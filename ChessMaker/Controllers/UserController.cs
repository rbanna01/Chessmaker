using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;

namespace ChessMaker.Controllers
{
    public class UserController : Controller
    {
        Entities entities = new Entities();

        public ActionResult Index()
        {
            return View(entities.Users.ToList());
        }

        public ActionResult View(int id)
        {
            var selectedUser = entities.Users.Find(id);
            if (selectedUser == null)
                return HttpNotFound();

            return View(selectedUser);
        }
    }
}
