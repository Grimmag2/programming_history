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
using System.Threading.Tasks;
using ITUvec.Database;

namespace ITUvec.ViewModels
{
    public class SettingsMenuViewModel : BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();

        private string _profileButtonText = "Settings";


        public string ProfileButtonText
        {
            get { return _profileButtonText; }
            set { _profileButtonText = value; OnPropertyChanged(nameof(ProfileButtonText)); }
        }

        public RelayCommand NavigateToProfileCommand { get; set; }

        // navigate to profile = go back
        public void NavigateToProfile()
        {
            string message = "Profile";
            if (ProfileButtonText == "Settings")
            {
                ProfileButtonText = "Profile";
            }
            else
            {
                ProfileButtonText = "Settings";
            }
            var msg = new NavigateToPageMessage() { Page = message };
            Messenger.Default.Send<NavigateToPageMessage>(msg);
        }

        public SettingsMenuViewModel()
        {
            NavigateToProfileCommand = new RelayCommand(NavigateToProfile);
        }
    }
}
