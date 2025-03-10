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
    public class AchievementsRightViewModel : BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();

        private IEnumerable<AchievementsModel> _achievements;
        private string _achievementsCountTextBlockText;
        private string _achievementsSentenceTextBlockText;

        public IEnumerable<AchievementsModel> Achievements
        {
            get { return _achievements; }
            set { _achievements = value; OnPropertyChanged(nameof(Achievements)); }
        }
        public string AchievementsCountTextBlockText
        {
            get { return _achievementsCountTextBlockText; }
            set { _achievementsCountTextBlockText = value; OnPropertyChanged(_achievementsCountTextBlockText); }
        }
        public string AchievementsSentenceTextBlockText
        {
            get { return _achievementsSentenceTextBlockText; }
            set { _achievementsSentenceTextBlockText = value; OnPropertyChanged(_achievementsSentenceTextBlockText); }
        }


        // Function for setting correct values of Bind properties on entering the Achievements
        private async Task ReceiveMessageNavigate(NavigateToPageMessage action)
        {
            if (action.Page == "Achievements")
            {
                await LoadAchievementsAsync();
                var myAchievements = Achievements.ToList();

                var countOfGainedAchievements = 0;

                for (int j = 0; j < myAchievements.Count; j++)
                {
                    if (myAchievements[j].Achieved)
                    {
                        countOfGainedAchievements++;
                    }
                }

                AchievementsCountTextBlockText = countOfGainedAchievements.ToString() + "/" + myAchievements.Count.ToString() + " dosaženo";
                AchievementsSentenceTextBlockText = "Dosáhli jste " + countOfGainedAchievements.ToString() + " úspěchů. Jen tak dál!";
            }
        }
        // Load achievements from database
        protected async Task LoadAchievementsAsync()
        {
            Achievements = await uof.AchievementsRepository.GetAllAsync();
        }

        public AchievementsRightViewModel()
        {
            Messenger.Default.Register<NavigateToPageMessage>
            (
                 this,
                 async (action) => await ReceiveMessageNavigate(action)
            );
            LoadAchievementsAsync();
        }
    }
}
