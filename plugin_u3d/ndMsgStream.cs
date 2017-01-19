using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;
using System.Globalization;
using System.Net;

namespace NetMessage
{
//     public class MessageHead
//     {
//         public ushort lenth;
//         public ushort id;
//         public uint flag;
//     }

    public class NDMsgStream
    {
        //    BufferedStream m_buf;
		private MemoryStream m_buf;
        //Encoding m_encoding;

        public NDMsgStream()
        {
            //m_encoding = new UTF8Encoding(false);
            m_buf = new MemoryStream();

        }

        public byte[] ToArray()
        {
            return m_buf.ToArray();
            /*
            byte[] test = new byte[5];
            fixed (byte* p = &test[0])
            {
                *p = 0xff;
            }
             */
        }
        //init from socket
        public int FromArray(IntPtr streamBuf, int size)
        {
            byte[] data = null;
            unsafe
            {                
                data = new byte[size];
                using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)streamBuf, size))
                {
                    ms.Read(data, 0, data.Length);
                }
            }
            m_buf.Write(data, 0, data.Length);
            return data.Length;
        }

       

		public void SetStream(MemoryStream stream)
        {
            m_buf = stream;
        }

        public int WriteUint8(byte data)
        {
            m_buf.WriteByte(data);
            return 1;
        }

        public int WriteInt8(sbyte data)
        {
            m_buf.WriteByte((byte)data);
            return 1;
        }

        public int WriteUint16(ushort data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((short)data));
            m_buf.Write(bufData, 0, bufData.Length);
            return bufData.Length;
        }

        public int WriteInt16(short data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            m_buf.Write(bufData, 0, bufData.Length);
            return bufData.Length;
        }

        public int WriteUint32(uint data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((int)data));
            m_buf.Write(bufData, 0, bufData.Length);
            return bufData.Length;
        }

        public int WriteInt32(int data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            m_buf.Write(bufData, 0, bufData.Length);
            return bufData.Length;
        }

        public int WriteUint64(ulong data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((long)data));
            m_buf.Write(bufData, 0, bufData.Length);
            return bufData.Length;
        }

        public int WriteInt64(long data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            m_buf.Write(bufData, 0, bufData.Length);
            return bufData.Length;
        }

        public int WriteString(string data)
        {
            byte[] byteArray = Encoding.ASCII.GetBytes(data);
            WriteUint16((ushort)(byteArray.Length));
            m_buf.Write(byteArray, 0, byteArray.Length);
            m_buf.WriteByte(0x7f);
            return byteArray.Length + 3;
        }

        public int WriteBuf(byte[] data)
        {
            ushort dataLenth = (ushort)data.Length;

			if (data.Length >= 0xffff)
            {
                return -1;
            }
            WriteUint16(dataLenth);

            m_buf.Write(data, 0, data.Length);
            return data.Length;
        }

        public byte ReadUint8()
        {
            byte[] receivedData = new byte[1];
            m_buf.Read(receivedData, 0, 1);
            return receivedData[0];
        }

        public sbyte ReadInt8()
        {
            byte[] receivedData = new byte[1];
            m_buf.Read(receivedData, 0, 1);
            return (sbyte)receivedData[0];
        }

        public ushort ReadUint16()
        {
            byte[] receivedData = new byte[2];
            m_buf.Read(receivedData, 0, 2);
            return (ushort)IPAddress.NetworkToHostOrder((short)BitConverter.ToUInt16(receivedData, 0));
        }

        public short ReadInt16()
        {
            byte[] receivedData = new byte[2];
            m_buf.Read(receivedData, 0, 2);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt16(receivedData, 0));
        }

        public uint ReadUint32()
        {
            byte[] receivedData = new byte[4];
            m_buf.Read(receivedData, 0, 4);
            return (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(receivedData, 0));
        }

        public int ReadInt32()
        {
            byte[] receivedData = new byte[4];
            m_buf.Read(receivedData, 0, 4);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt32(receivedData, 0));
        }

        public ulong ReadUint64()
        {
            byte[] receivedData = new byte[8];
            m_buf.Read(receivedData, 0, 8);
            return (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(receivedData, 0));
        }

        public long ReadInt64()
        {
            byte[] receivedData = new byte[8];
            m_buf.Read(receivedData, 0, 8);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt64(receivedData, 0));
        }

        public string ReadString()
        {
            ushort stringLen = ReadUint16();
            if(stringLen == 0 ) 
            {
				return string.Empty ;
            }
            stringLen--;
            byte[] receivedData = new byte[stringLen];
			m_buf.Read(receivedData,0,stringLen);
			byte endByte = (byte) m_buf.ReadByte();
            if (endByte != 0x7f)
            {
				return string.Empty ;
            }
            return Encoding.ASCII.GetString(receivedData);
        }


    }
}
