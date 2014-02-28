namespace Havenborough_Launcher
{
    class ShadowResolution
    {
        public string Display
        { get; set; }
        public string Size;

        public ShadowResolution(string display, string size)
        {
            Display = display;
            Size = size;
        }

        public override string ToString()
        {
            return Size;
        }
    }
}
