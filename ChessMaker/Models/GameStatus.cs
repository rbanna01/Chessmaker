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
    
    public partial class GameStatus
    {
        public GameStatus()
        {
            this.Games = new HashSet<Game>();
        }
    
        public byte ID { get; set; }
        public string Name { get; set; }
        public bool IsClosed { get; set; }
    
        public virtual ICollection<Game> Games { get; set; }
    }
}
