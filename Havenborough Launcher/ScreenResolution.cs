namespace Havenborough_Launcher
{
    class ScreenResolution
    {
        public string Width { get; set; }
        public string Height { get; set; }
        public string Display { get; set; }

        public ScreenResolution(string width, string height, string display)
        {
            Width = width;
            Height = height;
            Display = display;
        }

        public override string ToString()
        {
            return Display;
        }
    }
}
