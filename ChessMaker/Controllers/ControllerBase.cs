using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using ChessMaker.Models;
using System.Web.Mvc;
using ChessMaker.Services;

namespace ChessMaker.Controllers
{
    public class ControllerBase : Controller
    {
        public Entities Entities()
        {
            Entities result = HttpContext.Items[typeof(Entities).Name] as Entities;

            if (result == null)
            {
                result = new Entities();
                HttpContext.Items[typeof(Entities).Name] = result;
            }

            return result;
        }

        public T GetService<T>() where T : ServiceBase, new()
        {
            T result = HttpContext.Items[typeof(T).Name] as T;

            if (result == null)
            {
                result = new T();
                result.Entities = Entities();
                HttpContext.Items[typeof(T).Name] = result;
            }

            return result;
        }
    }
}