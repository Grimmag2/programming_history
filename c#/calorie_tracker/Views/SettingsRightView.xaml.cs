using ITUvec.Database;
using ITUvec.Models;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ITUvec.Views
{
    /// <summary>
    /// Interaction logic for SettingsRightView.xaml
    /// </summary>
    public partial class SettingsRightView : UserControl
    {
        private UnitOfWork uof = new UnitOfWork();
        public SettingsRightView()
        {
            InitializeComponent();
        }

        private void NameTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (NameTextBox.Text.Length > 0) 
                SaveNameToDatabase();
        }

        private void SurnameTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (SurnameTextBox.Text.Length > 0)
                SaveNameToDatabase();
        }

        protected async Task SaveNameToDatabase()
        {
            UserModel user = await uof.UserRepository.GetUserAsync();
            user.Name = NameTextBox.Text;
            user.Surname = SurnameTextBox.Text;
            await uof.SaveAsync();
        }
    }
}
