﻿using ChessMaker.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace ChessMaker.Services
{
    public class UserService : ServiceBase
    {
        public User GetByName(string name)
        {
            return Entities.Users.FirstOrDefault(u => u.Name == name);
        }

        public List<User> ListAll()
        {
            return Entities.Users.ToList();
        }

        public bool IsAllowedToEdit(Variant variant, string userName)
        {
            var user = GetByName(userName);
            return user != null && variant.CreatedByID == user.ID;
        }
    }
}