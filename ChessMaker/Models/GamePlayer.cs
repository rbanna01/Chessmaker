//------------------------------------------------------------------------------
// <auto-generated>
//    This code was generated from a template.
//
//    Manual changes to this file may cause unexpected behavior in your application.
//    Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace ChessMaker.Models
{
    using System;
    using System.Collections.Generic;
    
    public partial class GamePlayer
    {
        public GamePlayer()
        {
            this.Moves = new HashSet<Move>();
        }
    
        public int ID { get; set; }
        public int GameID { get; set; }
        public Nullable<int> UserID { get; set; }
        public string Name { get; set; }
        public bool HasAccepted { get; set; }
    
        public virtual Game Game { get; set; }
        public virtual User User { get; set; }
        public virtual ICollection<Move> Moves { get; set; }
    }
}
