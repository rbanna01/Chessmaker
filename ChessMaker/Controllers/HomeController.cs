using ChessMaker.Models;
using ChessMaker.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Web.Security;

namespace ChessMaker.Controllers
{
    public class HomeController : ControllerBase
    {
        public ActionResult Index()
        {
            IndexModel m = new IndexModel();

            var variantService = GetService<VariantService>();
            m.MostPopularVariants = variantService.GetVariantsByPopularity();
            m.LatestVariants = variantService.GetTenNewestVariants();

            if(Request.IsAuthenticated)
            { 
                var gameService = GetService<GameService>();
                int? userID = (int?) Membership.GetUser().ProviderUserKey;

                if(userID.HasValue)
                {
                    int ID = (int)userID;
                    m.GamesPlayersMove = gameService.GetGamesByNextPlayerID(ID);
                    m.GamesNotPlayersMove = gameService.GetGamesWhereNotPlayersTurn(ID);
                    
                }
            }



            return View(m);
        }
    }
}
