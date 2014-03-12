using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Havenborough_Launcher
{
    /// <summary>
    /// Interaction logic for InstructionsWindow.xaml
    /// </summary>
    public partial class InstructionsWindow
    {
        public InstructionsWindow()
        {
            InitializeComponent();

            Background = new ImageBrush
            {
                ImageSource = new BitmapImage(new Uri(@"assets\textures\Launcher_Background.jpg",
                    UriKind.Relative))
            };
            Icon = BitmapFrame.Create(new Uri(@"Havenborough.ico", UriKind.RelativeOrAbsolute));
        }

        private void Close_OnClick(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}