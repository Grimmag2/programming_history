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
using System.IO;
using Microsoft.Win32;

namespace ITUvec.ViewModels
{
    public class SettingsRightViewModel : BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();

        private string _profilePicture;
        private string _nameTextBoxText;
        private string _surnameTextBoxText;
        private UserModel _user;
        private UserModel _user2;
        public UserModel User
        {
            get { return _user; }
            set { _user = value; OnPropertyChanged(nameof(User)); }
        }
        public UserModel User2
        {
            get { return _user2; }
            set { _user2 = value; OnPropertyChanged(nameof(User2)); }
        }

        public string NameTextBoxText
        {
            get { return _nameTextBoxText; }
            set { _nameTextBoxText = value; OnPropertyChanged(nameof(NameTextBoxText)); }
        }
        public string SurnameTextBoxText
        {
            get { return _surnameTextBoxText; }
            set { _surnameTextBoxText = value; OnPropertyChanged(nameof(SurnameTextBoxText)); }
        }
        public string ProfilePicture
        {
            get { return _profilePicture; }
            set { _profilePicture = value; OnPropertyChanged(nameof(ProfilePicture)); }
        }

        // Function for setting correct values of Bind properties on entering the Settings
        private void ReceiveMessageNavigate(NavigateToPageMessage action)
        {
            if(action.Page == "Settings")
            {
                LoadUserAsync();
                LoadUserAsyncWithoutTracking();
                ProfilePicture = User.Picture.ToString();
                NameTextBoxText = User.Name.ToString();
                SurnameTextBoxText = User.Surname.ToString();
            }  
        }
        // Load user data from database without tracking
        protected async Task LoadUserAsyncWithoutTracking()
        {
            User = await uof.UserRepository.GetUserAsyncWithoutTracking();
        }
        // load user data from database
        protected async Task LoadUserAsync()
        {
            User2 = await uof.UserRepository.GetUserAsync();
        }

        public RelayCommand EditProfilePictureCommand { get; set; }

        // Give prompt to choose new profile picture and saving it to database
        public async void EditProfilePicture()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Title = "Select Image",
                Filter = "Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg|All files (*.*)|*.*"
            };
            if (openFileDialog.ShowDialog() == true)
            {
                string selectedImagePath = openFileDialog.FileName;

                ProfilePicture = selectedImagePath;
                User2.Picture = ProfilePicture;
                await uof.SaveAsync();
            }
        }


        public SettingsRightViewModel()
        {
            Messenger.Default.Register<NavigateToPageMessage>
            (
                 this,
                 (action) => ReceiveMessageNavigate(action)
            );
            EditProfilePictureCommand = new RelayCommand(EditProfilePicture);
            LoadUserAsync();
            LoadUserAsyncWithoutTracking();
        }
    }
}
