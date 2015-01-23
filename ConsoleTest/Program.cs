using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleTest
{
    class Program
    {
        [DllImport("C.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool Initialize(string definition, int svgBufferLength);

        [DllImport("C.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void GetBoardSVG(StringBuilder buffer, int maxLen);

        static void Main(string[] args)
        {
            var definition = File.ReadAllText("..\\..\\Definition.xml");

            if (!Initialize(definition, 8192))
            {
                Console.WriteLine("Initialization failed - please check definition");
                Console.ReadKey();
                return;
            }

            StringBuilder sb = new StringBuilder(8192);
            GetBoardSVG(sb, sb.Capacity);

            Console.WriteLine("received: {0}", sb);


            File.WriteAllText("..\\..\\Render.svg", sb.ToString());

            Console.ReadKey();
        }
    }
}
