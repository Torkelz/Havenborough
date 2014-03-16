using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
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
        private static bool _sfxStartUp = true;
        
        /// <summary>
        /// Launcher main window. Creates main window and displays for user interaction.
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();

            var xmlDataProvider = Resources["DataProvider"] as XmlDataProvider;
            if (xmlDataProvider != null)
                xmlDataProvider.Source = new Uri(Path.GetFullPath("UserOptions.xml"));

            Background = new ImageBrush
            {
                ImageSource = new BitmapImage(new Uri(@"assets\textures\Launcher_Background.jpg", UriKind.Relative))
            };
            MusicVolumeMedia.Source = new Uri(@"assets\sounds\launcher\Music.mp3", UriKind.Relative);
            MusicVolumeMedia.Play();

            RefreshGameList();
        }

        private void CharacterName_OnLoaded(object sender, RoutedEventArgs e)
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

        private void CharacterName_OnChanged(object sender, SelectionChangedEventArgs e)
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

        private void CharacterStyle_OnLoaded(object sender, RoutedEventArgs e)
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

        private void CharacterStyle_OnChanged(object sender, SelectionChangedEventArgs e)
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

        private void ScreenResolution_OnLoaded(object sender, RoutedEventArgs e)
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

        private void ScreenResolution_OnChanged(object sender, SelectionChangedEventArgs e)
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

        private void ShadowResolution_OnLoaded(object sender, RoutedEventArgs e)
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
                    comboBox.SelectedIndex =  2;         
                }
            }
        }

        private void ShadowResolution_OnChanged(object sender, SelectionChangedEventArgs e)
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

        private void KeyBindBox_OnLoaded(object sender, RoutedEventArgs e)
        {
            var textBox = sender as TextBox;
            if (textBox == null)
                return;

            int virtualKey;
            int.TryParse(textBox.Text, out virtualKey);
            textBox.Text = KeyInterop.KeyFromVirtualKey(virtualKey).ToString();
        }

        private void KeyBindPanel_OnKeyDown(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            var key = e.SystemKey != Key.None ? e.SystemKey : e.Key;

            var stackPanel = sender as StackPanel;
            if (stackPanel == null)
                return;
            var objects = stackPanel.Children;
            var textBox = objects[0] as TextBox;
            var textBlock = objects[1] as TextBlock;
            if (textBox == null || textBlock == null)
                return;

            var dataProvider = (Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null)
                return;
            XmlElement rootElement = dataProvider.Document["UserOptions"];
            if (rootElement == null)
                return;
            XmlElement controlsElement = rootElement["Controls"];
            if (controlsElement == null)
                return;

            for (XmlNode xmlNode = controlsElement["KeyMap"]; xmlNode != null;
                xmlNode = xmlNode.NextSibling)
            {
                var keyMapElement = xmlNode as XmlElement;
                if (keyMapElement == null)
                    continue;

                if (keyMapElement.GetAttribute("Display") != textBlock.Text)
                    continue;

                keyMapElement.SetAttribute("Key", (KeyInterop.VirtualKeyFromKey(key)).ToString(CultureInfo.InvariantCulture));
                textBox.Text = key.ToString();
                break;
            }
            Keyboard.ClearFocus();
            textBox.Background = Brushes.Transparent;
        }

        private void MouseButtonBindTextBox_OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            var textBox = sender as TextBox;
            if (textBox == null)
                return;

            textBox.Text = e.ChangedButton.ToString();
        }

        private void Controlers_OnMouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            e.Handled = true;
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
                //Process.Start(ClientExec);
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
            var window = new CreditsWindow();
            window.Show();
        }

        private void InstructionsButton_OnClick(object sender, RoutedEventArgs e)
        {
            var window = new InstructionsWindow();
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

        private void SelectedGame_OnChanged(object sender, SelectionChangedEventArgs e)
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

        private void Slider_OnChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var slider = sender as Slider;
            if (slider == null || FovValue == null || MouseSenseValue == null || MusicVolumeValue == null || 
                SfxVolumeValue == null)
                return;

            switch (slider.Name)
            {
                case "FovSlider":
                {
                    FovValue.Text = (int) slider.Value == 70 ?
                        "Quake Pro" : ((int) slider.Value).ToString(CultureInfo.InvariantCulture);
                        break;
                }
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
                    break;

                }
                case "MusicVolumeSlider":
                {
                    MusicVolumeValue.Text = (int) slider.Value == 0 ? 
                        "Muted" : ((int) slider.Value).ToString(CultureInfo.InvariantCulture);
                    MusicVolumeMedia.Volume = slider.Value * 0.01;
                    break;
                }
                case "SfxVolumeSlider":
                {
                    SfxVolumeValue.Text = (int)slider.Value == 0 ?
                        "Muted" : ((int)slider.Value).ToString(CultureInfo.InvariantCulture);
                    if (_sfxStartUp)
                        _sfxStartUp = false;
                    else
                    {
                       SfxVolumeMedia.Source = new Uri(@"assets\sounds\launcher\", UriKind.Relative);
                       SfxVolumeMedia.Volume = slider.Value * 0.01;
                       SfxVolumeMedia.Play(); 
                    }
                    break;
                }
            }
        }

        private void TextBox_OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            var textBox = sender as TextBox;
            if (textBox == null)
                return;

            textBox.Background = new SolidColorBrush(Color.FromRgb(0x45, 0x45, 0x45));
        }

        private void TextBox_OnLostFocus(object sender, RoutedEventArgs e)
        {
            var textBox = sender as TextBox;
            if (textBox == null)
                return;

            textBox.Background = new SolidColorBrush(Color.FromRgb(0x25, 0x25, 0x25));
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