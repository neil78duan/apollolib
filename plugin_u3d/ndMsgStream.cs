using System;
using System.Collections.Generic;
using System.Text;


using System.Runtime.InteropServices;

using System.IO;
using System.Globalization;
using System.Net;

namespace NetMessage
{
    public struct NDMsgStream
    {
        // Default buffer length
        public const int BUF_LENGTH = 0x10000;

        static byte[] _m_buf = new byte[BUF_LENGTH];
        public byte[] m_buf 
        { 
            get { return _m_buf; }
        }
        private int m_readIndex;
        private int m_writeIndex;

        public int ValidLength 
        {
            get { return m_writeIndex; }
        }

        public NDMsgStream(int capacity = BUF_LENGTH)
        {
            m_readIndex = 0;
            m_writeIndex = 0;
        }

        public byte[] ToArray()
        {
            return m_buf;
        }

        //init from socket
        public int FromArray(byte[] data)
        {
            m_readIndex = 0;
            data.CopyTo(_m_buf, 0);
            return data.Length;
        }

        public int FromAddr(IntPtr streamBuf, int size)
        {
            m_readIndex = 0;
            Marshal.Copy(streamBuf, m_buf, 0, size);
            return size;
        }

        public int WriteUint8(byte data)
        {
            m_buf[m_writeIndex++] = data;
            return 1;
        }

        public int WriteInt8(sbyte data)
        {
            m_buf[m_writeIndex++] =(byte) data;
            //m_buf.WriteByte((byte)data);
            return 1;
        }
        public int WriteFloat(float data)
        {
            byte[] bufData = BitConverter.GetBytes(data);
            myWrite(bufData, 4);
            return bufData.Length;
        }

        public int WriteUint16(ushort data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((short)data));
            myWrite(bufData, 2);

            //m_buf.Write(bufData, (int)m_buf.Length, bufData.Length);
            return bufData.Length;
        }

        public int WriteInt16(short data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            myWrite(bufData, 2);
            return bufData.Length;
        }

        public int WriteUint32(uint data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((int)data));
            myWrite(bufData, 4);
            //m_buf.Write(bufData, (int)m_buf.Length, bufData.Length);
            return bufData.Length;
        }

        public int WriteInt32(int data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            myWrite(bufData, 4);
            //m_buf.Write(bufData, (int)m_buf.Length, bufData.Length);
            return bufData.Length;
        }

        public int WriteUint64(ulong data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((long)data));
            myWrite(bufData, 8);
            //m_buf.Write(bufData, (int)m_buf.Length, bufData.Length);
            return bufData.Length;
        }

        public int WriteInt64(long data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            myWrite(bufData, 8);
            //m_buf.Write(bufData, (int)m_buf.Length, bufData.Length);
            return bufData.Length;
        }

        public int WriteString(string data)
        {
            byte[] byteArray = Encoding.ASCII.GetBytes(data);
            WriteUint16((ushort)(byteArray.Length));
            myWrite(byteArray, (int)byteArray.Length);
            m_buf[m_writeIndex++] = 0x7f;
            //m_buf.Write(byteArray, (int)m_buf.Length, byteArray.Length);
            //m_buf.WriteByte(0x7f);
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

            myWrite(data, data.Length);
            //m_buf.Write(data, (int)m_buf.Length, data.Length);
            return data.Length;
        }

        public byte ReadUint8()
        {
            return m_buf[m_readIndex++];
        }

        public sbyte ReadInt8()
        {
            return (sbyte) m_buf[m_readIndex++];

        }
        public float ReadFloat()
        {
            byte[] receivedData = new byte[4];
            myRead(receivedData, 4);
            //m_buf.Read(receivedData, (int)m_buf.Position, 4);
            return BitConverter.ToSingle(receivedData,0);
        }

        public ushort ReadUint16()
        {
            byte[] receivedData = new byte[2];
            myRead(receivedData, 2);
            //m_buf.Read(receivedData, (int)m_buf.Position, 2);
            return (ushort)IPAddress.NetworkToHostOrder((short)BitConverter.ToUInt16(receivedData, 0));
        }

        public short ReadInt16()
        {
            byte[] receivedData = new byte[2];
            myRead(receivedData, 2);
            //m_buf.Read(receivedData, (int)m_buf.Position, 2);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt16(receivedData, 0));
        }

        public uint ReadUint32()
        {
            byte[] receivedData = new byte[4];
            myRead(receivedData, 4);
            //m_buf.Read(receivedData, (int)m_buf.Position, 4);
            return (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(receivedData, 0));
        }

        public int ReadInt32()
        {
            byte[] receivedData = new byte[4];
            myRead(receivedData, 4);
            //m_buf.Read(receivedData, (int)m_buf.Position, 4);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt32(receivedData, 0));
        }

        public ulong ReadUint64()
        {
            byte[] receivedData = new byte[8];
            myRead(receivedData, 8);
            //m_buf.Read(receivedData, (int)m_buf.Position, 8);
            return (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(receivedData, 0));
        }

        public long ReadInt64()
        {
            byte[] receivedData = new byte[8];
            myRead(receivedData, 8);
            //m_buf.Read(receivedData, (int)m_buf.Position, 8);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt64(receivedData, 0));
        }

        public string ReadString()
        {
            ushort stringLen = ReadUint16();
            if(stringLen == 0 ) 
            {
				return string.Empty ;
            }
            byte[] receivedData = new byte[stringLen];
            myRead(receivedData, stringLen);
            //m_buf.Read(receivedData, (int)m_buf.Position, stringLen);
			byte endByte = ReadUint8();
            if (endByte != 0x7f)
            {
				return string.Empty ;
            }
            return Encoding.ASCII.GetString(receivedData);
        }
        

        public int ReadBuf(out byte[] data)
        {
            ushort length = ReadUint16();
            data = new byte[length];

            myRead(data, length);
            //m_buf.Read(data, (int)m_buf.Position, length);

            return data.Length;
        }

        private void myWrite(byte[] data, int len)
        {   
            for (var i = 0; i < len; ++i)
            {
                m_buf[m_writeIndex++] = data[i];
            }
        }


        private void myRead(byte[] data, int len)
        {
            for (var i = 0; i < len; ++i)
            {
                data[i] = m_buf[m_readIndex++] ;
            }
        }
    }
}
