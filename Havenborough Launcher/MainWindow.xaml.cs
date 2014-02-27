using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Xml;
using MahApps.Metro.Controls;
using System.Windows.Controls;

namespace Havenborough_Launcher
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
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

            RefreshGameList();
        }

        private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            var dataProvider = (this.Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;

            string source = dataProvider.Source.LocalPath;
            dataProvider.Document.Save(source);

            Process.Start("Client.exe");
        }

        private void Refresh_OnClick(object sender, RoutedEventArgs e)
        {
            RefreshGameList();
        }

        private void RefreshGameList()
        {
            var dataProvider = (this.Resources["DataProvider"] as XmlDataProvider);
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

            GameList gameList = this.Resources["gameDataSource"] as GameList;
            if (gameList != null)
                gameList.Refresh(host, port);
        }

        private void OnSelectedGameChanged(object sender, SelectionChangedEventArgs args)
        {
            GameList.Game selectedGame = gameListView.SelectedItem as GameList.Game;
            if (selectedGame == null ||
                selectedGame.Name == null ||
                selectedGame.Name.Length == 0)
            {
                launchButton.IsEnabled = false;
                return;
            }

            var dataProvider = (this.Resources["DataProvider"] as XmlDataProvider);
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

            launchButton.IsEnabled = true;
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
}
