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

        public ActionResult Index(int? id)
        {
            if (id == null)
                return View(entities.Users.ToList());
            
            var selectedUser = entities.Users.Find(id);
            if (selectedUser == null)
                return HttpNotFound();

            return View("View", selectedUser);
        }
    }
}
