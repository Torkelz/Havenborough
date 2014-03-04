using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Xml;

namespace Havenborough_Launcher
{
    /// <summary>
    ///     Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public MainWindow()
        {
            var backgroundBrush = new ImageBrush
            {
                ImageSource = new BitmapImage(new Uri(@"assets\textures\Launcher_Background.jpg",
                    UriKind.Relative))
            };

            InitializeComponent();
            var xmlDataProvider = Resources["DataProvider"] as XmlDataProvider;
            if (xmlDataProvider != null)
                xmlDataProvider.Source = new Uri(Path.GetFullPath("UserOptions.xml"));
            Background = backgroundBrush;

            RefreshGameList();
        }

        private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;

            string source = dataProvider.Source.LocalPath;
            dataProvider.Document.Save(source);

            //Process.Start("Client.exe");
        }

        private void Refresh_OnClick(object sender, RoutedEventArgs e)
        {
            RefreshGameList();
        }

        private void RefreshGameList()
        {
            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;

            XmlDocument doc;
            if (dataProvider.Document == null)
            {
                doc = new XmlDocument();
                doc.Load(dataProvider.Source.LocalPath);
            }
            else
            {
                doc = dataProvider.Document;
            }

            XmlElement rootNode = doc["UserOptions"];
            if (rootNode == null)
                return;

            XmlElement serverNode = rootNode["Server"];
            if (serverNode == null)
                return;

            XmlNode hostNode = serverNode.Attributes.GetNamedItem("Hostname");
            string host = "localhost";
            if (hostNode != null)
                host = hostNode.Value;

            XmlNode portNode = serverNode.Attributes.GetNamedItem("Port");
            int port = 31415;
            if (portNode != null)
                int.TryParse(portNode.Value, out port);

            var gameList = Resources["GameDataSource"] as GameList;
            if (gameList != null)
                gameList.Refresh(host, port);
        }

        private void OnSelectedGameChanged(object sender, SelectionChangedEventArgs args)
        {
            var selectedGame = GameListView.SelectedItem as GameList.Game;
            if (selectedGame == null ||
                selectedGame.Name == null ||
                selectedGame.Name.Length == 0)
            {
                LaunchButton.IsEnabled = false;
                return;
            }

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;

            XmlElement rootNode = dataProvider.Document["UserOptions"];
            if (rootNode == null)
            {
                return;
            }

            XmlElement gameNode = rootNode["Game"];
            if (gameNode == null)
            {
                gameNode = dataProvider.Document.CreateElement("Game");
                rootNode.AppendChild(gameNode);
            }

            XmlAttribute levelAttribute = dataProvider.Document.CreateAttribute("Level");
            levelAttribute.Value = selectedGame.Name;
            gameNode.Attributes.SetNamedItem(levelAttribute);

            LaunchButton.IsEnabled = true;
        }

        private void ShadowResolutionLoad(object sender, RoutedEventArgs e)
        {
            var data = new List<ShadowResolution>
            {
                new ShadowResolution("V.High", "4096"),
                new ShadowResolution("High", "2048"),
                new ShadowResolution("Medium", "1024"),
                new ShadowResolution("Low", "512")
            };
            var comboBox = sender as ComboBox;
            if(comboBox == null)
                return;

            comboBox.ItemsSource = data;
            comboBox.SelectedIndex = 2;
        }

        private void CharacterNameLoad(object sender, RoutedEventArgs e)
        {
            var data = new List<string>
            {
                "Dzala",
                "Zane"
            };
            var comboBox = sender as ComboBox;
            if(comboBox == null)
                return;

            comboBox.ItemsSource = data;
            comboBox.SelectedIndex = 0;
        }

        private void CharacterStyleLoad(object sender, RoutedEventArgs e)
        {
            var data = new List<string>
            {
                "Green",
                "Blue",
                "Red",
                "Black"
            };
            var comboBox = sender as ComboBox;
            if (comboBox == null)
                return;

            comboBox.ItemsSource = data;
            comboBox.SelectedIndex = 0;
        }
    }

    public class BoolInverterConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool)
            {
                return !(bool) value;
            }


            return !bool.Parse(value.ToString());
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool)
            {
                return !(bool) value;
            }
            return !bool.Parse(value.ToString());
        }

        #endregion
    }

    public class StringToBoolConverter : IValueConverter
    {
        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return bool.Parse(value.ToString());
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value.ToString().ToLower();
        }

        #endregion
    }
}