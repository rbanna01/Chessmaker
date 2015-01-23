using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleTest
{
    class Program
    {
        [DllImport("C.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void Initialize(string definition);

        [DllImport("C.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void GetBoardSVG(StringBuilder buffer, int maxLen);

        static void Main(string[] args)
        {
            Initialize("blah");

            StringBuilder sb = new StringBuilder(1000);
            GetBoardSVG(sb, sb.Capacity);

            Console.WriteLine("received: {0}", sb);
            Console.ReadKey();
        }
    }
}
