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
    public class SettingsLeftViewModel : BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();

        private int _gender;
        private string _ageTextBoxText;
        private string _heightTextBoxText;
        private string _weightTextBoxText;
        private int _weightComboBoxvalue;
        private int _heightComboBoxvalue;
        private string _stornoValidationButtonsVisibility;
        private string _settingsButtonsVisibility;
        private string[] _genderButtonsColor;

        public RelayCommand SaveProfileSettingsCommand { get; set; }
        public RelayCommand StornoProfileSettingsCommand { get; set; }
        public RelayCommand YesStornoProfileSettingsCommand { get; set; }
        public RelayCommand NoStornoProfileSettingsCommand { get; set; }
        public RelayCommand MaleTransitionCommand { get; set; }
        public RelayCommand FemaleTransitionCommand { get; set; }

        private UserModel _user;
        public UserModel User
        {
            get { return _user; }
            set { _user = value; OnPropertyChanged(nameof(User)); }
        }
        public int Gender
        {
            get { return _gender; }
            set 
            { 
                _gender = value;
                if (value == 0 && GenderButtonsColor[0] != "Visible")
                    GenderButtonsColor = new string[4] { "Visible", "Hidden", "Hidden", "Visible" };
                else if(value == 1 && GenderButtonsColor[2] != "Visible")
                    GenderButtonsColor = new string[4] { "Hidden", "Visible", "Visible", "Hidden" };
                OnPropertyChanged(nameof(Gender)); 
            }
        }
        public string AgeTextBoxText
        {
            get { return _ageTextBoxText; }
            set { _ageTextBoxText = value; OnPropertyChanged(nameof(AgeTextBoxText)); }
        }
        public string HeightTextBoxText
        {
            get { return _heightTextBoxText; }
            set { _heightTextBoxText = value; OnPropertyChanged(nameof(HeightTextBoxText)); }
        }
        public string WeightTextBoxText
        {
            get { return _weightTextBoxText; }
            set { _weightTextBoxText = value; OnPropertyChanged(nameof(WeightTextBoxText)); }
        }
        public int WeightComboBoxValue
        {
            get { return _weightComboBoxvalue; }
            set { _weightComboBoxvalue = value;OnPropertyChanged(nameof(WeightComboBoxValue)); }
        }
        public int HeightComboBoxValue
        {
            get { return _heightComboBoxvalue; }
            set { _heightComboBoxvalue = value; OnPropertyChanged(nameof(HeightComboBoxValue)); }
        }
        public string StornoValidationButtonsVisibility
        {
            get { return _stornoValidationButtonsVisibility; }
            set { _stornoValidationButtonsVisibility = value; OnPropertyChanged(nameof(StornoValidationButtonsVisibility)); }
        }
        public string SettingsButtonsVisibility
        {
            get { return _settingsButtonsVisibility; }
            set { _settingsButtonsVisibility = value; OnPropertyChanged(nameof(SettingsButtonsVisibility)); }
        }
        public string[] GenderButtonsColor
        {
            get { return _genderButtonsColor; }
            set { _genderButtonsColor = value; OnPropertyChanged(nameof(GenderButtonsColor)); }
        }

        // Function for setting correct values of Bind properties on entering the Settings
        private void ReceiveMessage(NavigateToPageMessage action)
        {
            if(action.Page == "Settings")
            {
                LoadUserAsync();
                Gender = User.Gender;
                AgeTextBoxText = User.Age.ToString();
                HeightTextBoxText = User.Height.ToString();
                WeightTextBoxText = User.Weight.ToString();
                HeightComboBoxValue = User.HeightUnit;
                WeightComboBoxValue = User.WeightUnit;

                SettingsButtonsVisibility = "Visible";
                StornoValidationButtonsVisibility = "Hidden";
            }  
        }
        // Load User data from database
        protected async Task LoadUserAsync()
        {
            User = await uof.UserRepository.GetUserAsync();
        }

        // Save newly given data to database
        public async void SaveProfileSettings()
        {

            User.Gender = Gender;
            if (AgeTextBoxText != "" && int.Parse(AgeTextBoxText) > 0)
                User.Age = int.Parse(AgeTextBoxText);
            else
                AgeTextBoxText = User.Age.ToString();
            
            if (HeightTextBoxText != "" && int.Parse(HeightTextBoxText) > 0)
                User.Height = int.Parse(HeightTextBoxText);
            else
                HeightTextBoxText = User.Height.ToString();

            if (WeightTextBoxText != "" && int.Parse(WeightTextBoxText) > 0)
            {
                User.Weight = int.Parse(WeightTextBoxText);
                DateModel? today = await uof.DateRepository.GetByDateAsync(Data.CurrentDate.Date);
                if(today != null)
                    today.Weight = int.Parse(WeightTextBoxText);
            }
            else
                WeightTextBoxText = User.Weight.ToString();
            User.HeightUnit = HeightComboBoxValue;
            User.WeightUnit = WeightComboBoxValue;
            //User.Picture = instance.ProfilePicture;
            //User.Name = instance.NameTextBoxText;
            //User.Surname= instance.SurnameTextBoxText;

            await uof.SaveAsync();

            var msg = new NavigateToPageMessage() { Page = "Profile" };
            Messenger.Default.Send<NavigateToPageMessage>(msg);
            //refresh water view
            var msg2 = new ChangeDateMessage() {};
            Messenger.Default.Send<ChangeDateMessage>(msg2);
        }

        // Give prompt to cancle newly made changes
        public void StornoProfileSettings()
        {
            SettingsButtonsVisibility = "Hidden";
            StornoValidationButtonsVisibility = "Visible";
        }

        // Agree on canceling newly made changes in settings
        public void YesStornoProfileSettings()
        {
            SettingsButtonsVisibility = "Visible";
            StornoValidationButtonsVisibility = "Hidden";

            Gender = User.Gender;
            AgeTextBoxText = User.Age.ToString();
            HeightTextBoxText = User.Height.ToString();
            WeightTextBoxText = User.Weight.ToString();
            HeightComboBoxValue = User.HeightUnit;
            WeightComboBoxValue = User.WeightUnit;
        }

        // Disagree on canceling newly made changes
        public void NoStornoProfileSettings()
        {
            SettingsButtonsVisibility = "Visible";
            StornoValidationButtonsVisibility = "Hidden";
        }

        // Changing Gender to Male
        public void MaleTransition()
        {
            if(Gender == 1)
            {
                Gender = 0;
            }    
        }
        // Changing Gender to Female
        public void FemaleTransition()
        {
            if (Gender == 0)
            {
                Gender = 1;
            }
        }

        public SettingsLeftViewModel()
        {
            Messenger.Default.Register<NavigateToPageMessage>
            (
                 this,
                 (action) => ReceiveMessage(action)
            );
            LoadUserAsync();

            GenderButtonsColor = new string[4] { "Visible", "Hidden", "Hidden", "Visible" };

            SaveProfileSettingsCommand = new RelayCommand(SaveProfileSettings);
            StornoProfileSettingsCommand = new RelayCommand(StornoProfileSettings);
            YesStornoProfileSettingsCommand = new RelayCommand(YesStornoProfileSettings);
            NoStornoProfileSettingsCommand = new RelayCommand(NoStornoProfileSettings);

            MaleTransitionCommand = new RelayCommand(MaleTransition);
            FemaleTransitionCommand = new RelayCommand(FemaleTransition);
        }
    }
}
