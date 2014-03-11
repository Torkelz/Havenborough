using System;
using System.Collections.Generic;
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
using System.Windows.Shapes;

namespace Havenborough_Launcher
{
    /// <summary>
    /// Interaction logic for Credits.xaml
    /// </summary>
    public partial class Credits
    {
        public Credits()
        {
            InitializeComponent();

            Background = new ImageBrush
            {
                ImageSource = new BitmapImage(new Uri(@"assets\textures\Launcher_Background.jpg",
                    UriKind.Relative))
            };
            Icon = BitmapFrame.Create(new Uri(@"Havenborough.ico", UriKind.RelativeOrAbsolute));
        }

        private void Back_OnClick(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
