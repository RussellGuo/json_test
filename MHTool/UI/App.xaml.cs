using ISPCore.Util;
using net.meano.Util;
using System;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Threading;
#if !DEBUG
using System.Collections.Generic;
using System.Reflection;
#endif

namespace MHTool
{
    /// <summary>
    /// App.xaml 的交互逻辑
    /// </summary>
    public partial class App : Application
    {
        public App()
        {
            Current.DispatcherUnhandledException += UICatchException;
            AppDomain.CurrentDomain.UnhandledException += ThreadCatchException;

#if !DEBUG
            string libraryPrefix = MethodBase.GetCurrentMethod().DeclaringType.Namespace;
            AddAssemblyResource(libraryPrefix, "ISPCore.dll");
            AddAssemblyResource(libraryPrefix, "Cryptography.dll");
            AppDomain.CurrentDomain.AssemblyResolve += StartupAssembly;
#endif
        }

        public static string AppDirectoryPath()
        {
            return AppDomain.CurrentDomain.SetupInformation.ApplicationBase;
        }

        public static string BuildVersion(bool isRevisionEnabled)
        {
            StringBuilder versionBuilder = new();
            versionBuilder
                .Append(ResourceAssembly.GetName().Version.Major).Append(".")
                .Append(ResourceAssembly.GetName().Version.Minor).Append(".")
                .Append(ResourceAssembly.GetName().Version.Build);

            if (isRevisionEnabled)
            {
                versionBuilder.Append(".").Append(ResourceAssembly.GetName().Version.Revision);
            }

            return versionBuilder.ToString();
        }

        void UICatchException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            string errorMessage = "未处理UI线程异常：\n异常信息-> " + e.Exception.Message + "\n堆栈信息-> " + e.Exception.StackTrace;
            string CrashInfoPath = "./log/CrashInfo-" + DateTime.Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt"; ;
            FileStream errorFS = new(CrashInfoPath, FileMode.Create);
            byte[] errorBytes = Encoding.UTF8.GetBytes(errorMessage);
            errorFS.Write(errorBytes, 0, errorBytes.Length);
            errorFS.Flush();
            errorFS.Close();
            Logger.Error(errorMessage);
            MessageBox.Show(
                "非常抱歉，您可能触发了意外操作，操作已经终止，若不影响使用，可以重试。\n请保留Log文件，并将Log提供给Megahunt FAE进行分析处理。",
                "意外操作",
                MessageBoxButton.OK,
                MessageBoxImage.Information
            );
            e.Handled = true;
        }

        void ThreadCatchException(object sender, UnhandledExceptionEventArgs e)
        {
            string errorMessage = "未处理线程异常：\n异常信息-> " + ((Exception)e.ExceptionObject).Message + "\n堆栈信息-> " + ((Exception)e.ExceptionObject).StackTrace;
            string CrashInfoPath = "./log/CrashInfo-" + DateTime.Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt"; ;
            FileStream errorFS = new(CrashInfoPath, FileMode.Create);
            byte[] errorBytes = Encoding.UTF8.GetBytes(errorMessage);
            errorFS.Write(errorBytes, 0, errorBytes.Length);
            errorFS.Flush();
            errorFS.Close();
            Logger.Fatal(errorMessage);
            MessageBox.Show(
                "非常抱歉，应用程序出现错误，即将关闭。\n请保留Log文件，并将Log提供给Megahunt FAE进行分析处理。",
                "程序终止",
                MessageBoxButton.OK,
                MessageBoxImage.Information
            );
        }

#if !DEBUG
        private static readonly Dictionary<string, Assembly> DllDictionary = new Dictionary<string, Assembly>();
        private static bool AddAssemblyResource(string libraryPrefix, string libraryFileName)
        {
            Assembly As = GetAssemblyDll($"{libraryPrefix}.{libraryFileName}");
            if (As == null) return false;
            DllDictionary.Add(Path.GetFileNameWithoutExtension(libraryFileName), As);
            return true;
        }

        private static Assembly GetAssemblyDll(string AssemblyDllPath)
        {
            Assembly executingAssembly = Assembly.GetExecutingAssembly();
            using (Stream stream = executingAssembly.GetManifestResourceStream(AssemblyDllPath))
            {
                if (stream == null)
                    return null;
                byte[] assemblyRawBytes = new byte[stream.Length];
                stream.Read(assemblyRawBytes, 0, assemblyRawBytes.Length);
                Assembly As = Assembly.Load(assemblyRawBytes);
                return As;
            }
        }

        private static Assembly StartupAssembly(object sender, ResolveEventArgs args)
        {
            AssemblyName assemblyName = new AssemblyName(args.Name);
            string ResourceName = assemblyName.Name;
            if (DllDictionary.ContainsKey(ResourceName))
            {
                return DllDictionary[ResourceName];
            }
            else
            {
                return null;
            }
        }
#endif
    }
}
