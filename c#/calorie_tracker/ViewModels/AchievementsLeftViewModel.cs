//Authors: Dalibor Kalina
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows.Input;
using ITUvec.Models;
using CommunityToolkit.Mvvm.Input;
using GalaSoft.MvvmLight.Messaging;
using ITUvec.Messages;
using System.Windows.Controls;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows.Media;
using ITUvec.Database;
using System.Collections.Generic;
using System.Windows;

namespace ITUvec.ViewModels
{
    public class AchievementsLeftViewModel : BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();

        private string[] _achievement;
        private string[] _achievementIcon;
        private string[] _achievementDescription; 
        private IEnumerable<AchievementsModel> _achievements;
        public IEnumerable<AchievementsModel> Achievements
        {
            get { return _achievements; }
            set { _achievements = value; OnPropertyChanged(nameof(Achievements)); }
        }
        public string[] Achievement
        {
            get { return _achievement; }
            set { _achievement = value;OnPropertyChanged(nameof(Achievement));}
        }
        public string[] AchievementIcon
        {
            get { return _achievementIcon; }
            set { _achievementIcon = value; OnPropertyChanged(nameof(AchievementIcon)); }
        }
        public string[] AchievementDescription
        {
            get { return _achievementDescription; }
            set { _achievementDescription = value; OnPropertyChanged(nameof(AchievementDescription)); }
        }

        // Function for setting correct values of Bind properties on entering the Achievements
        private void ReceiveMessageNavigate(NavigateToPageMessage action)
        {
            if (action.Page == "Achievements")
            {
                LoadAchievementsAsync();
                var myAchievements = Achievements.ToList();
                var countOfGainedAchievements = 0;

                for (int j = 0; j < myAchievements.Count; j++)
                {
                    if (myAchievements[j].Achieved)
                    {
                        countOfGainedAchievements++;
                        AchievementIcon[j] = "/ITUvec;component/images/Achievement_True.png";
                    }
                    else
                    {
                        AchievementIcon[j] = "/ITUvec;component/images/Achievement_False.png";
                    }
                    Achievement[j] = myAchievements[j].Name;
                    AchievementDescription[j] = myAchievements[j].Description;
                }
                Achievement = Achievement;
                AchievementIcon = AchievementIcon;
                AchievementDescription = AchievementDescription;

            }
        }
        // Load achievements from database
        protected async Task LoadAchievementsAsync()
        {
            Achievements = await uof.AchievementsRepository.GetAllAsync();
        }


        public AchievementsLeftViewModel()
        {
            Achievement = new string[10];
            AchievementDescription = new string[10];
            AchievementIcon = new string[10];
            Messenger.Default.Register<NavigateToPageMessage>
            (
                 this,
                 (action) => ReceiveMessageNavigate(action)
            );
            LoadAchievementsAsync();
        }
    }
}
