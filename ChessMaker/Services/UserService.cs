using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace ChessMaker.Services
{
    public class UserService
    {
        public static User GetByName(string name)
        {
            using (Entities entities = new Entities())
            {
                return entities.Users.FirstOrDefault(u => u.Name == name);
            }
        }

        internal static List<User> ListAll()
        {
            using (Entities entities = new Entities())
            {
                return entities.Users.ToList();
            }
        }
    }
}