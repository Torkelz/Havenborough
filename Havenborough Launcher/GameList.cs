using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace Havenborough_Launcher
{
    class GameList
    {
        public class Game : INotifyPropertyChanged
        {
            private string name;
            private int waitingPlayers;
            private int maxPlayers;

            public event PropertyChangedEventHandler PropertyChanged;

            public string Name
            {
                get { return name; }
                set
                {
                    name = value;
                    OnPropertyChanged("Name");
                }
            }

            public int WaitingPlayers
            {
                get { return waitingPlayers; }
                set
                {
                    waitingPlayers = value;
                    OnPropertyChanged("WaitingPlayers");
                }
            }

            public int MaxPlayers
            {
                get { return maxPlayers; }
                set
                {
                    maxPlayers = value;
                    OnPropertyChanged("MaxPlayers");
                }
            }

            protected void OnPropertyChanged(string name)
            {
                PropertyChangedEventHandler handler = PropertyChanged;
                if (handler != null)
                {
                    handler(this, new PropertyChangedEventArgs(name));
                }
            }

            public override string ToString()
            {
                return Name + " (" + WaitingPlayers + "/" + MaxPlayers + ")";
            }
        }

        Task refreshTask;
        Dispatcher dispatcher;

        public ObservableCollection<Game> Games { get; set; }

        public GameList()
        {
            dispatcher = Dispatcher.CurrentDispatcher;
            Games = new ObservableCollection<Game>();
        }

        public void Refresh()
        {
            refreshTask = Task.Run(() =>
            {
                byte[] data = null;
                try
                {
                    using (Socket clientSocket = Connect())
                    {
                        SendRequest(clientSocket);
                        data = ReadResponse(clientSocket);
                        clientSocket.Shutdown(SocketShutdown.Both);
                    }
                }
                catch (SocketException ex)
                {
                    Debug.WriteLine(ex);
                }
                catch (ArgumentNullException ex)
                {
                    Debug.WriteLine(ex);
                }
                catch (ArgumentOutOfRangeException ex)
                {
                    Debug.WriteLine(ex);
                }
                catch (ObjectDisposedException ex)
                {
                    Debug.WriteLine(ex);
                }
                catch (NotSupportedException ex)
                {
                    Debug.WriteLine(ex);
                }
                catch (InvalidOperationException ex)
                {
                    Debug.WriteLine(ex);
                }

                if (data == null)
                    return;

                dispatcher.Invoke(() =>
                {
                    Games.Clear();
                    foreach (Game game in TranslateData(data))
                    {
                        Games.Add(game);
                    }
                });
            });
        }

        Socket Connect()
        {
            Socket clientSocket = new Socket(SocketType.Stream, ProtocolType.Tcp);
            clientSocket.Connect("localhost", 31415);

            return clientSocket;
        }

        void SendRequest(Socket p_ClientSocket)
        {
            const short packageSize = 4;
            const short requestId = 1;

            byte[] request = new byte[4];
            BitConverter.GetBytes(packageSize).CopyTo(request, 0);
            BitConverter.GetBytes(requestId).CopyTo(request, 2);

            p_ClientSocket.Send(request);
        }

        byte[] ReadResponse(Socket p_ClientSocket)
        {
            byte[] header = new byte[4];
            ReceiveAll(p_ClientSocket, header, 4);

            short length = BitConverter.ToInt16(header, 0);
            short id = BitConverter.ToInt16(header, 2);

            byte[] data = new byte[length - 4];
            ReceiveAll(p_ClientSocket, data, length - 4);

            return data;
        }

        void ReceiveAll(Socket p_ClientSocket, byte[] p_Buffer, int p_Size)
        {
            int receivedBytes = p_ClientSocket.Receive(p_Buffer);

            int totalReceived = receivedBytes;
            while (receivedBytes > 0 && totalReceived < p_Size)
            {
                receivedBytes = p_ClientSocket.Receive(p_Buffer, totalReceived, p_Size - totalReceived, SocketFlags.None);
                totalReceived += receivedBytes;
            }

            if (totalReceived < p_Size)
            {
                throw new SocketException();
            }
        }

        Game[] TranslateData(byte[] p_Data)
        {
            Debug.Assert(p_Data.Length >= 8);

            long numGames = BitConverter.ToInt64(p_Data, 0);
            int currPos = 8;
            Game[] games = new Game[numGames];

            Encoding enc = Encoding.UTF8;

            for (int i = 0; i < numGames; ++i)
            {
                Game g = new Game();

                int stringLength = BitConverter.ToInt32(p_Data, currPos);
                currPos += 4;

                g.Name = enc.GetString(p_Data, currPos, stringLength);
                currPos += stringLength;

                g.WaitingPlayers = BitConverter.ToInt16(p_Data, currPos);
                currPos += 2;

                g.MaxPlayers = BitConverter.ToInt16(p_Data, currPos);
                currPos += 2;

                games[i] = g;
            }

            return games;
        }
    }
}
