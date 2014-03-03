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
            private string _name;
            private int _waitingPlayers;
            private int _maxPlayers;

            public event PropertyChangedEventHandler PropertyChanged;

            public string Name
            {
                get { return _name; }
                set
                {
                    _name = value;
                    OnPropertyChanged("Name");
                }
            }

            public int WaitingPlayers
            {
                get { return _waitingPlayers; }
                set
                {
                    _waitingPlayers = value;
                    OnPropertyChanged("WaitingPlayers");
                }
            }

            public int MaxPlayers
            {
                get { return _maxPlayers; }
                set
                {
                    _maxPlayers = value;
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

        readonly Dispatcher _dispatcher;

        public ObservableCollection<Game> Games { get; set; }

        public GameList()
        {
            _dispatcher = Dispatcher.CurrentDispatcher;
            Games = new ObservableCollection<Game>();
        }

        public void Refresh(string host, int port)
        {
            Task.Run(() =>
            {
                byte[] data = null;
                try
                {
                    using (Socket clientSocket = Connect(host, port))
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

                _dispatcher.Invoke(() =>
                {
                    Games.Clear();
                    foreach (Game game in TranslateData(data))
                    {
                        Games.Add(game);
                    }
                });
            });
        }

        Socket Connect(string host, int port)
        {
            Socket clientSocket = new Socket(SocketType.Stream, ProtocolType.Tcp);
            clientSocket.Connect(host, port);

            return clientSocket;
        }

        void SendRequest(Socket clientSocket)
        {
            const short packageSize = 4;
            const short requestId = 1;

            byte[] request = new byte[4];
            BitConverter.GetBytes(packageSize).CopyTo(request, 0);
            BitConverter.GetBytes(requestId).CopyTo(request, 2);

            clientSocket.Send(request);
        }

        byte[] ReadResponse(Socket clientSocket)
        {
            byte[] header = new byte[4];
            ReceiveAll(clientSocket, header, 4);

            short length = BitConverter.ToInt16(header, 0);

            byte[] data = new byte[length - 4];
            ReceiveAll(clientSocket, data, length - 4);

            return data;
        }

        void ReceiveAll(Socket clientSocket, byte[] buffer, int size)
        {
            int receivedBytes = clientSocket.Receive(buffer);

            int totalReceived = receivedBytes;
            while (receivedBytes > 0 && totalReceived < size)
            {
                receivedBytes = clientSocket.Receive(buffer, totalReceived, size - totalReceived, SocketFlags.None);
                totalReceived += receivedBytes;
            }

            if (totalReceived < size)
            {
                throw new SocketException();
            }
        }

        Game[] TranslateData(byte[] data)
        {
            Debug.Assert(data.Length >= 8);

            long numGames = BitConverter.ToInt64(data, 0);
            int currPos = 8;
            Game[] games = new Game[numGames];

            Encoding enc = Encoding.UTF8;

            for (int i = 0; i < numGames; ++i)
            {
                Game g = new Game();

                int stringLength = BitConverter.ToInt32(data, currPos);
                currPos += 4;

                g.Name = enc.GetString(data, currPos, stringLength);
                currPos += stringLength;

                g.WaitingPlayers = BitConverter.ToInt16(data, currPos);
                currPos += 2;

                g.MaxPlayers = BitConverter.ToInt16(data, currPos);
                currPos += 2;

                games[i] = g;
            }

            return games;
        }
    }
}
