using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Havenborough_Launcher
{
    /// <summary>
    /// Interaction logic for CreditsWindow.xaml
    /// </summary>
    public partial class CreditsWindow
    {
        public CreditsWindow()
        {
            InitializeComponent();

            Background = new ImageBrush
            {
                ImageSource = new BitmapImage(new Uri(@"assets\textures\Launcher_CreditsBackground.png", UriKind.Relative))
            };
        }

        private void Close_OnClick(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
