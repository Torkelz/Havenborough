using System;
using System.Windows;
using System.Windows.Controls;
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
            Icon = BitmapFrame.Create(new Uri(@"Havenborough.ico", UriKind.RelativeOrAbsolute));
        }

        private void Close_OnClick(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void ImageLabels_OnLoaded(object sender, RoutedEventArgs e)
        {
            var label = sender as Label;
            if (label == null)
                return;

            switch (label.Name)
            {
                case "CheckpointImage":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsCheckpoint.png");
                    break;
                }
                case "ArrowImage":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsArrow.png");
                    break;
                }
                case "CheckpointsLeftImage":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsCheckpointsLeft.png");
                    break;
                }
                case "ManabarImage":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsManabar.png");
                    break;
                }
            }

        }

        private static void SetImage(Label label, string filepath)
        {
            if (label == null)
                return;

            ImageSource image = new BitmapImage(new Uri(filepath, UriKind.Relative));

            label.Background = new ImageBrush
            {
                ImageSource = image
            };
            label.Width = image.Width;
            label.Height = image.Height;
        }
    }
}