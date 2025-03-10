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

    public class ProfileRightViewModel: BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();

        private string _achievementButtonText = "Profile";
        private IEnumerable<AchievementsModel> _achievements;
        private string _achievementTextBlockText1;
        private string _achievementTextBlockText2;
        private string _achievementTextBlockText3;
        private string _achievementTextBlockText4;
        private string _achievementImage1;
        private string _achievementImage2;
        private string _achievementImage3;
        private string _achievementImage4;
        private string _achievementsCountTextBlockText;

        public IEnumerable<AchievementsModel> Achievements
        {
            get { return _achievements; }
            set { _achievements = value; OnPropertyChanged(nameof(Achievements)); }
        }
        public string AchievementButtonText
        {
            get { return _achievementButtonText; }
            set { _achievementButtonText = value; OnPropertyChanged(nameof(AchievementButtonText)); }
        }

        public string AchievementTextBlockText1
        {
            get { return _achievementTextBlockText1; }
            set { _achievementTextBlockText1 = value; OnPropertyChanged(nameof(AchievementTextBlockText1)); }
        }
        public string AchievementTextBlockText2
        {
            get { return _achievementTextBlockText2; }
            set { _achievementTextBlockText2 = value; OnPropertyChanged(nameof(AchievementTextBlockText2)); }
        }
        public string AchievementTextBlockText3
        {
            get { return _achievementTextBlockText3; }
            set { _achievementTextBlockText3 = value; OnPropertyChanged(nameof(AchievementTextBlockText3)); }
        }
        public string AchievementTextBlockText4
        {
            get { return _achievementTextBlockText4; }
            set { _achievementTextBlockText4 = value; OnPropertyChanged(nameof(AchievementTextBlockText4)); }
        }
        public string AchievementImage1
        {
            get { return _achievementImage1; }
            set { _achievementImage1 = value; OnPropertyChanged(nameof(_achievementImage1)); }
        }
        public string AchievementImage2
        {
            get { return _achievementImage2; }
            set { _achievementImage2 = value; OnPropertyChanged(nameof(_achievementImage2)); }
        }
        public string AchievementImage3
        {
            get { return _achievementImage3; }
            set { _achievementImage3 = value; OnPropertyChanged(nameof(_achievementImage3)); }
        }
        public string AchievementImage4
        {
            get { return _achievementImage4; }
            set { _achievementImage4 = value; OnPropertyChanged(nameof(_achievementImage4)); }
        }
        public string AchievementsCountTextBlockText
        {
            get { return _achievementsCountTextBlockText; }
            set { _achievementsCountTextBlockText = value; OnPropertyChanged(_achievementsCountTextBlockText); }
        }

        public RelayCommand NavigateToAchievementsCommand { get; set; }

        // navigate to achievements
        public void NavigateToAchievements()
        {
            string message = "Achievements";
            if (AchievementButtonText == "Profile")
            {
                AchievementButtonText = "Achievement";
            }
            else
            {
                AchievementButtonText = "Profile";
            }
            var msg = new NavigateToPageMessage() { Page = message };
            Messenger.Default.Send<NavigateToPageMessage>(msg);
        }
        // Function for setting correct values of Bind properties on entering the Profile
        private void ReceiveMessageNavigate(NavigateToPageMessage action)
        {
            if (action.Page == "Profile")
            {
                LoadAchievementsAsync();
                var myAchievements = Achievements.ToList();

                var countOfGainedAchievements = 0;

                for (int j = 0; j < myAchievements.Count; j++)
                {
                    if (myAchievements[j].Achieved)
                    {
                        countOfGainedAchievements++;
                    }
                }

                AchievementsCountTextBlockText = countOfGainedAchievements.ToString() + "/" + myAchievements.Count.ToString() + " odměn získáno";

                Random rand = new Random();
                int[] chosenAchievements = { -1, -1, -1, -1 };
                int i = 0;
                while (i < chosenAchievements.Length)
                {
                    int temp = rand.Next(0, 10);
                    if (!chosenAchievements.Contains(temp))
                    {
                        chosenAchievements[i] = temp;
                        i++;
                    }
                }
                AchievementTextBlockText1 = myAchievements[chosenAchievements[0]].Name;
                AchievementTextBlockText2 = myAchievements[chosenAchievements[1]].Name;
                AchievementTextBlockText3 = myAchievements[chosenAchievements[2]].Name;
                AchievementTextBlockText4 = myAchievements[chosenAchievements[3]].Name;

                AchievementImage1 = "/ITUvec;component/images/Achievement_" + myAchievements[chosenAchievements[0]].Achieved.ToString() + ".png";
                AchievementImage2 = "/ITUvec;component/images/Achievement_" + myAchievements[chosenAchievements[1]].Achieved.ToString() + ".png";
                AchievementImage3 = "/ITUvec;component/images/Achievement_" + myAchievements[chosenAchievements[2]].Achieved.ToString() + ".png";
                AchievementImage4 = "/ITUvec;component/images/Achievement_" + myAchievements[chosenAchievements[3]].Achieved.ToString() + ".png";
            }
        }

        // Load Achievements from database
        protected async Task LoadAchievementsAsync()
        {
            Achievements = await uof.AchievementsRepository.GetAllAsync();
        }

        public ProfileRightViewModel()
        {
            Messenger.Default.Register<NavigateToPageMessage>
            (
                 this,
                 (action) => ReceiveMessageNavigate(action)
            );
            LoadAchievementsAsync();
            NavigateToAchievementsCommand = new RelayCommand(NavigateToAchievements);
        }
    }
}
