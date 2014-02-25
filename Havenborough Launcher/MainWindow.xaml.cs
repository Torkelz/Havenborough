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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml;

namespace Havenborough_Launcher
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            string appPath = System.IO.Path.GetDirectoryName(
                System.Reflection.Assembly.GetExecutingAssembly().CodeBase);
            var xmlDataProvider = this.Resources["DataProvider"] as XmlDataProvider;
            if (xmlDataProvider != null)
                xmlDataProvider.Source = new Uri(System.IO.Path.Combine(appPath, "@/../../../Client/Bin/UserOptions.xml"));
        }

        private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            var dataProvider = (this.Resources["DataProvider"] as XmlDataProvider);
            if (dataProvider == null) return;
            string appPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().CodeBase);
            var source = new Uri(System.IO.Path.Combine(appPath, "@/../../../Client/Bin/UserOptions.xml"));
            dataProvider.Document.Save(source.LocalPath);
        }

        private void inventoryTypeComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

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

