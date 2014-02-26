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
using System.Xml;
using MahApps.Metro.Controls;

namespace Havenborough_Launcher
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        private GameList gameList;

        public MainWindow()
        {
            ImageBrush backgroundBrush = new ImageBrush();
            backgroundBrush.ImageSource = new BitmapImage(new Uri(@"assets\textures\Launcher.jpg",
                UriKind.Relative));
           
            InitializeComponent();
            var xmlDataProvider = this.Resources["DataProvider"] as XmlDataProvider;
            if (xmlDataProvider != null)
                xmlDataProvider.Source = new Uri(System.IO.Path.GetFullPath("UserOptions.xml"));
            this.Background = backgroundBrush;

            gameList = new GameList(
                (GameList.Game[] p_Games) =>
                {
                    gameListBox.Dispatcher.Invoke(() =>
                        {
                            string gameText = "";
                            foreach (GameList.Game game in p_Games)
                            {
                                gameText += game.name + " (" + game.waitingPlayers + '/' + game.maxPlayers + ")\n";
                            }

                            gameListBox.Text = gameText;
                        });
                });
            gameList.Refresh();
        }

        private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            var dataProvider = (this.Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;

            string source = dataProvider.Source.LocalPath;
            dataProvider.Document.Save(source);

            //Process.Start("Client.exe");
        }

        private void Refresh_OnClick(object sender, RoutedEventArgs e)
        {
            gameList.Refresh();
        }
    }   
    public class BoolInverterConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is bool)
            {
                return !(bool)value;
            }


            return !bool.Parse(value.ToString());
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is bool)
            {
                return !(bool)value;
            }
            return !bool.Parse(value.ToString());
        }

        #endregion
    }
    public class StringToBoolConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {

            return bool.Parse(value.ToString());
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {

            return value.ToString().ToLower();
        }

        #endregion
    }
    public class DropDownConverter
    {
        
    }
}

