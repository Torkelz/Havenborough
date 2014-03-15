using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using MahApps.Metro.Controls;

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
            var stackPanel = sender as StackPanel;
            if (stackPanel == null)
                return;
            var label = stackPanel.GetChildObjects().First() as Label;
            var textBlock = stackPanel.GetChildObjects().Last() as TextBlock;
            if (label == null || textBlock == null)
                return;

            switch (stackPanel.Name)
            {
                case "CheckpointInstruction":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsCheckpoint.png");
                    label.Width *= 0.5f;
                    label.Height *= 0.5f;
                    break;
                }
                case "ArrowInstruction":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsArrow.png");
                    label.Width *= 0.75f;
                    label.Height *= 0.75f;
                    break;
                }
                case "CheckpointsLeftInstruction":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsCheckpointsLeft.png");
                    label.Width *= 0.75f;
                    label.Height *= 0.75f;
                    break;
                }
                case "ManabarInstruction":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsManabar.png");
                    label.Width *= 0.75f;
                    label.Height *= 0.75f;
                    break;
                }
                case "TimeInstruction":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsTime.png");
                    label.Width *= 0.75f;
                    label.Height *= 0.75f;
                    break;
                }
                case "PositionInstruction":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsPosition.png");
                    label.Width *= 0.75f;
                    label.Height *= 0.75f;
                    break;
                }
                case "ClimbInstruction":
                {
                    SetImage(label, @"assets\textures\Launcher_InstructionsClimb.png");
                    label.Width *= 0.75f;
                    label.Height *= 0.75f;
                    break;
                }
            }
            textBlock.Width = label.Width;
        }

        private static void SetImage(Control label, string filepath)
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