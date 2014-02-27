using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Havenborough_Launcher
{
    class ShadowResolution
    {
        public string Display
        { get; set; }
        public string size;

        public ShadowResolution(string _display, string _size)
        {
            Display = _display;
            size = _size;
        }

        public override string ToString()
        {
            return size;
        }
    }
}
