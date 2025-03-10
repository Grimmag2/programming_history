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
using System.Drawing;
using System.Diagnostics;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Drawing.Imaging;

namespace ITUvec.ViewModels
{
    public class ProfileMenuViewModel: BaseViewModel
    {
        private UnitOfWork uof = new UnitOfWork();

        private string previousLocation = "Main";

        public RelayCommand NavigateToMainCommand { get; set; }
        public RelayCommand NavigateToSettingsCommand { get; set; }
        public RelayCommand ShareButtonCommand { get; set; }

        // navigate to main
        public void NavigateToMain()
        {
            string message = previousLocation;
            var msg = new NavigateToPageMessage() { Page = message };
            Messenger.Default.Send<NavigateToPageMessage>(msg);
        }

        // navigate to settings
        public void NavigateToSettings()
        {
            string message = "Settings";
            var msg = new NavigateToPageMessage() { Page = message };
            Messenger.Default.Send<NavigateToPageMessage>(msg);
        }


        [DllImport("user32.dll")]
        static extern IntPtr GetForegroundWindow();

        [DllImport("user32.dll", SetLastError = true)]
        static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
        [StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }
        // Save screenshot of window to user-chosen directory
        public void ShareButton()
        {
            RECT rect = new RECT();
            GetWindowRect(GetForegroundWindow(), out rect);
            Bitmap bitmap = new Bitmap(rect.Right - (rect.Left + 20), rect.Bottom - (rect.Top + 8));

            using (Graphics graphics = Graphics.FromImage(bitmap))
            {
                graphics.CopyFromScreen(rect.Left+8, rect.Top, 0, 0, new System.Drawing.Size(rect.Right-(rect.Left+20), rect.Bottom-(rect.Top+8)));
            }

            SaveFileDialog dialog = new SaveFileDialog();
            dialog.FileName = "share";
            dialog.Filter = "Images (*.png,*.jpeg)|*.png;*.jpeg";
            bool? result = dialog.ShowDialog();
            if ((bool)result == true)
            {
                bitmap.Save(dialog.FileName, System.Drawing.Imaging.ImageFormat.Png);
            }
            bitmap.Dispose();
        }

        // Function for setting correct values of Bind properties on entering the Profile
        private void ReceiveMessageNavigate(NavigateToPageMessage action)
        {
            if (action.Page == "Profile" && action.Parameter is string)
            {
                if ((string)(action.Parameter) == "FromCalendar")
                    previousLocation = "Calendar";
                else if ((string)(action.Parameter) == "FromAddFood")
                    previousLocation = "AddFood";
            }
            else
                previousLocation = "Main";
        }

        public ProfileMenuViewModel()
        {
            Messenger.Default.Register<NavigateToPageMessage>(this, (action) => ReceiveMessageNavigate(action));

            NavigateToMainCommand = new RelayCommand(NavigateToMain);
            NavigateToSettingsCommand = new RelayCommand(NavigateToSettings);
            ShareButtonCommand = new RelayCommand(ShareButton);
        }
    }
}
