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
        private const string ClientExec = "Client.exe";
        private const string ServerExec = "Server.exe";
        
        public MainWindow()
        {
            InitializeComponent();

            var xmlDataProvider = Resources["DataProvider"] as XmlDataProvider;
            if (xmlDataProvider != null)
                xmlDataProvider.Source = new Uri(Path.GetFullPath("UserOptions.xml"));

            Background = new ImageBrush
            {
                ImageSource = new BitmapImage(new Uri(@"assets\textures\Launcher_Background.jpg",
                    UriKind.Relative))
            };
            Icon = BitmapFrame.Create(new Uri(@"Havenborough.ico", UriKind.RelativeOrAbsolute));

            RefreshGameList();
        }

        private void CharacterNameLoad(object sender, RoutedEventArgs e)
        {
            var data = new List<string>
            {
                "Dzala",
                "Zane"
            };
            var comboBox = sender as ComboBox;
            if (comboBox == null)
                return;

            comboBox.ItemsSource = data;

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
            {
                comboBox.SelectedIndex = 0;
                return;
            }
            XmlElement characterElement = rootElement["Character"];
            if (characterElement == null)
            {
                comboBox.SelectedIndex = 0;
                return;
            }
            var name = characterElement.GetAttribute("Name");
            if (name.Length != 0)
                comboBox.SelectedIndex = data.IndexOf(name) == -1 ? 0 : data.IndexOf(name);
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

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
            {
                comboBox.SelectedIndex = 0;
                return;
            }
            XmlElement rootNode = dataProvider.Document["UserOptions"];
            if (rootNode == null)
            {
                comboBox.SelectedIndex = 0;
                return;
            }
            XmlElement characterNode = rootNode["Character"];
            if (characterNode == null)
            {
                comboBox.SelectedIndex = 0;
                return;
            }
            var style = characterNode.GetAttribute("Style");
            if (style.Length != 0)
                comboBox.SelectedIndex = data.IndexOf(style) == -1 ? 0 : data.IndexOf(style);
        }

        private void ScreenResolutionLoad(object sender, RoutedEventArgs e)
        {
            var data = new List<string>
            {
                "2560x1440",
                "1920x1080",
                "1680x1050",
                "1600x1200",
                "1440x900",
                "1366x768",
                "1280x720",
                "1024x768",
                "800x600"
            };
            var comboBox = sender as ComboBox;
            if (comboBox == null)
                return;
            comboBox.ItemsSource = data;

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
                return;
            XmlElement settingsElement = rootElement["Settings"];
            if (settingsElement == null)
                return;
            XmlElement resolutionElement = settingsElement["Resolution"];
            if (resolutionElement == null)
                return;
            var width = resolutionElement.GetAttribute("Width");
            var height = resolutionElement.GetAttribute("Height");
            if (width.Length == 0 || height.Length == 0)
                comboBox.SelectedIndex = 6;
            else
                comboBox.SelectedIndex = data.IndexOf(width + "x" + height);
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
            if (comboBox == null)
                return;

            comboBox.ItemsSource = data;

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
            {
                comboBox.SelectedIndex = 2;
                return;
            }
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
            {
                comboBox.SelectedIndex = 2;
                return;
            }
            XmlElement settingsElement = rootElement["Settings"];
            if (settingsElement == null)
            {
                comboBox.SelectedIndex = 2;
                return;
            }
            XmlElement resolutionElement = settingsElement["ShadowMapResolution"];
            if (resolutionElement == null)
            {
                comboBox.SelectedIndex = 2;
                return;
            }
            var resolution = resolutionElement.GetAttribute("Value");
            if (resolution.Length != 0)
            {
                for (int i = 0; i < data.Count; i++)
                {
                    if (resolution == data[i].Size)
                    {
                        comboBox.SelectedIndex = i;
                        return;
                    }  
                    else
                        comboBox.SelectedIndex =  2;         
                }
            }
        }

        private void LaunchButton_OnClick(object sender, RoutedEventArgs e)
        {
            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;

            string source = dataProvider.Source.LocalPath;
            dataProvider.Document.Save(source);
            try
            {
                Process.Start(ClientExec);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.ToString(), "Failed to start game");
            }  
        }

        private void RefreshButton_OnClick(object sender, RoutedEventArgs e)
        {
            RefreshGameList();
        }

        private void HostServerButton_OnClick(object sender, RoutedEventArgs e)
        {
            try
            {
                Process.Start(ServerExec);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.ToString(), "Failed to host server");
            	return;
            }
            
            RefreshGameList();
        }

        private void CreditsButton_OnClick(object sender, RoutedEventArgs e)
        {
            var window = new Credits();
            window.Show();
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
                doc = dataProvider.Document;

            XmlElement rootElement = doc["UserOptions"];
            if (rootElement == null)
                return;
            XmlElement serverElement = rootElement["Server"];
            if (serverElement == null)
                return;

            XmlAttribute hostAttribute = serverElement.GetAttributeNode("Hostname");
            string host = hostAttribute != null ? hostAttribute.Value : "localhost";

            XmlAttribute portAttribute = serverElement.GetAttributeNode("Port");
            int port = 31415;
            if (portAttribute != null)
                int.TryParse(portAttribute.Value, out port);

            var gameList = Resources["GameDataSource"] as GameList;
            if (gameList != null)
                gameList.Refresh(host, port);
        }

        private void OnSelectedGameChanged(object sender, SelectionChangedEventArgs e)
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

            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
            {
                return;
            }

            XmlElement gameElement = rootElement["Game"];
            if (gameElement == null)
            {
                gameElement = dataProvider.Document.CreateElement("Game");
                rootElement.AppendChild(gameElement);
            }

            XmlAttribute levelAttribute = dataProvider.Document.CreateAttribute("Level");
            levelAttribute.Value = selectedGame.Name;
            gameElement.Attributes.SetNamedItem(levelAttribute);

            LaunchButton.IsEnabled = true;
        }

        private void CharacterNameChanged(object sender, SelectionChangedEventArgs e)
        {
            var comboBox = sender as ComboBox;
            if (comboBox == null)
                return;
            var data = comboBox.SelectedItem;
            if (data == null)
                return;

            var styleData = data.ToString();

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
                return;
            XmlElement characterElement = rootElement["Character"];
            if (characterElement == null)
                return;
            XmlAttribute nameAttribute = characterElement.GetAttributeNode("Name");
            if (nameAttribute == null)
                return;

            nameAttribute.Value = styleData;
        }

        private void CharacterStyleChanged(object sender, SelectionChangedEventArgs e)
        {
            var comboBox = sender as ComboBox;
            if (comboBox == null)
                return;
            var data = comboBox.SelectedItem;
            if (data == null)
                return;

            var styleData = data.ToString();

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
                return;
            XmlElement characterElement = rootElement["Character"];
            if (characterElement == null)
                return;
            XmlAttribute styleAttribute = characterElement.GetAttributeNode("Style");
            if (styleAttribute == null)
                return;

            styleAttribute.Value = styleData;
        }

        private void ScreenResolutionChanged(object sender, SelectionChangedEventArgs e)
        {
            var comboBox = sender as ComboBox;
            if (comboBox == null)
                return;
            var data = comboBox.SelectedItem;
            if (data == null)
                return;

            var resolutionData = data.ToString().Split('x');
            
            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
                return;
            XmlElement settingsElement = rootElement["Settings"];
            if (settingsElement == null)
                return;
            XmlElement resolutionElement = settingsElement["Resolution"];
            if (resolutionElement == null)
                return;
            XmlAttribute widthAttribute = resolutionElement.GetAttributeNode("Width");
            XmlAttribute heightAttribute = resolutionElement.GetAttributeNode("Height");
            if (widthAttribute == null || heightAttribute == null)
                return;

            widthAttribute.Value = resolutionData[0];
            heightAttribute.Value = resolutionData[1];
        }

        private void ShadowResolutionChanged(object sender, SelectionChangedEventArgs e)
        {
            var comboBox = sender as ComboBox;
            if (comboBox == null)
                return;
            var data = comboBox.SelectedItem;
            if (data == null)
                return;

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
                return;
            XmlElement settingsElement = rootElement["Settings"];
            if (settingsElement == null)
                return;
            XmlElement resolutionElement = settingsElement["ShadowMapResolution"];
            if (resolutionElement == null)
                return;
            XmlAttribute sizeAttribute = resolutionElement.GetAttributeNode("Value");
            if (sizeAttribute == null)
                return;

            sizeAttribute.Value = data.ToString();
        }

        private void SliderChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var slider = sender as Slider;
            if (slider == null || FovValue == null || MouseSenseValue == null)
                return;

            switch (slider.Name)
            {
                case "FovSlider":
                    FovValue.Text = ((int) slider.Value).ToString(CultureInfo.InvariantCulture);
                    if ((int) slider.Value == 70)
                        FovValue.Text = "Quake Pro";
                    break;
                case "MouseSenseSlider":
                {
                    var val = slider.Value.ToString(CultureInfo.InvariantCulture);
                    switch (val.Length)
                    {
                        case 1:
                            MouseSenseValue.Text = val;
                            break;
                        case 3:
                            MouseSenseValue.Text = val.Substring(0, 3);
                            break;
                        default:
                            MouseSenseValue.Text = val.Substring(0, 4);
                            break;
                    }
                }
                    break;
            }
               
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