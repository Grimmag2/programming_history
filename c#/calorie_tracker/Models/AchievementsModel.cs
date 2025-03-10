//Authors: Dalibor Kalina
using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ITUvec.Models
{
    public record AchievementsModel : INotifyPropertyChanged
    {
        public int Id { get; set; }
        public string Name { get; set; } = "";
        public string Description { get; set; } = "";
        public bool Achieved { get; set; }

        public event PropertyChangedEventHandler? PropertyChanged;
    }
}
