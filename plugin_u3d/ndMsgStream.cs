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
        const byte NET_STREAM_STRUCT_END_MARK = 0xff;
        enum eNDnetStreamMarker
        {
            ENDSTREAM_MARKER_UNDEFINE,
            ENDSTREAM_MARKER_INT8,
            ENDSTREAM_MARKER_INT16,
            ENDSTREAM_MARKER_INT32,
            ENDSTREAM_MARKER_INT64,
            ENDSTREAM_MARKER_FLOAT,
            ENDSTREAM_MARKER_DOUBLE,
            ENDSTREAM_MARKER_TEXT,
            ENDSTREAM_MARKER_BIN,
            ENDSTREAM_MARKER_IP32,
            ENDSTREAM_MARKER_IP64,

        };

        // Default buffer length
        public const int BUF_LENGTH = 0x10000;

        static byte[] _m_buf = new byte[BUF_LENGTH];
        public byte[] m_buf 
        { 
            get { return _m_buf; }
        }
        private int m_readIndex;
        private int m_writeIndex;
        private bool m_bStruckEndMarker ;

        public int ValidLength 
        {
            get { return m_writeIndex; }
        }

        public NDMsgStream(int capacity = BUF_LENGTH)
        {
            m_readIndex = 0;
            m_writeIndex = 0;
            m_bStruckEndMarker = false;
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

        #region pub_write_func

        public int WriteUint8(byte a)
        {
            if (a == 0)
            {
                return _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT8, 0);
            }
            else
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT8, 1))
                {
                    return 0;
                }
                return OrgWriteUint8((byte)a) + 1;
            }
            return 0;
        }

        public int WriteInt8(sbyte data)
        {
            return WriteUint8((byte)data);
        }
        public int WriteFloat(float a)
        {
            if (a == 0)
            {
                return _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_FLOAT, 0);
            }
            else
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_FLOAT, 4))
                {
                    return 0;
                }
                return OrgWriteFloat(a) + 1;
            }
            return 0;
        }

        public int WriteUint16(ushort a)
        {
            if (a == 0)
            {
                return _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT16, 0);
            }
            else if (a <= 0xff)
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT16, 1))
                {
                    return 0;
                }
                return OrgWriteUint8((byte)a) + 1;
            }
            else 
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT16, 2))
                {
                    return -1;
                }
                return OrgWriteUint16(a) + 1;
            }
            return 0;
        }

        public int WriteInt16(short data)
        {
            return WriteUint16((ushort)data) ;
        }

        public int WriteUint32(uint a)
        {
            if (a == 0)
            {
                return _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT32, 0);
            }
            else if (a <= 0xff)
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT32, 1))
                {
                    return 0;
                }
                return OrgWriteUint8((byte)a) + 1;
            }
            else if (a <= 0xffff)
            {

                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT32, 2))
                {
                    return 0;
                }
                return OrgWriteUint16((ushort)a) + 1;
            }
            else
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT32, 4))
                {
                    return 0;
                }
                return OrgWriteUint32(a)+1;
            }
            return 0;
        }

        public int WriteInt32(int data)
        {
            return WriteUint32((uint)data);
        }

        public int WriteUint64(ulong a)
        {
            if (a == 0)
            {
                return _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 0);
            }
            else if (a <= 0xff)
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 1))
                {
                    return 0;
                }
                return OrgWriteUint8((byte)a) + 1;
            }
            else if (a <= 0xffff)
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 2))
                {
                    return 0;
                }
                return OrgWriteUint16((ushort)a) + 1;
            }
            else if (a <= 0xffffffff)
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 4))
                {
                    return 0;
                }
                return OrgWriteUint32((uint)a) + 1;
            }
            else
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 8))
                {
                    return 0;
                }
                return OrgWriteUint64(a) + 1;
            }
            return 0;
        }

        public int WriteInt64(long data)
        {
            return WriteUint64((ulong) data) ;
        }

        public int WriteString(string data)
        {
            if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_TEXT, 0))
            {
                return -1;
            }

            byte[] byteArray = Encoding.UTF8.GetBytes(data);
            WriteUint16((ushort)(byteArray.Length));
			if(byteArray.Length ==0) {
				return 2 ;
			}
            myWrite(byteArray, (int)byteArray.Length);
            m_buf[m_writeIndex++] = 0x7f;
            return byteArray.Length + 3;
        }

        public int WriteBuf(byte[] data)
        {
            if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_BIN, 0))
            {
                return -1;
            }

            ushort dataLenth = (ushort)data.Length;

			if (data.Length >= 0xffff)
            {
                return -1;
            }
            WriteUint16(dataLenth);
            myWrite(data, data.Length);
            return data.Length;
        }
        #endregion

        #region pub_read_func

        public byte ReadUint8()
        {
            eNDnetStreamMarker type;
            byte size;
            m_bStruckEndMarker = false;
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                return 0;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_INT8)
            {
                return 0;
            }

            if (size == 0)
            {
                return 0;
            }
            else if (size == 1)
            {
                return OrgReadUint8();
            }
            else
            {
                return 0;
            }
        }

        public sbyte ReadInt8()
        {
            return (sbyte)ReadUint8();
        }
        public float ReadFloat()
        {
            eNDnetStreamMarker type;
            byte size;
            m_bStruckEndMarker = false;
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                return 0;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_FLOAT)
            {
                return 0;
            }

            if (size == 0)
            {
                return 0;
            }
            else 
            {
                return OrgReadFloat();
            }
        }

        public ushort ReadUint16()
        {
            eNDnetStreamMarker type;
            byte size;
            m_bStruckEndMarker = false;
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                return 0;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_INT16)
            {
                return 0;
            }

            if (size == 0)
            {
                return 0;
            }
            else if (size == 1)
            {
                return OrgReadUint8();
            }

            else if (size == 2)
            {
                return OrgReadUint16();
            }
            else
            {
                return 0;
            }
        }

        public short ReadInt16()
        {
            return (short)ReadUint16();
        }

        public uint ReadUint32()
        {
            eNDnetStreamMarker type;
            byte size;
            m_bStruckEndMarker = false;
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                return 0;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_INT32)
            {
                return 0;
            }

            if (size == 0)
            {
                return 0;
            }
            else if (size == 1)
            {
                return OrgReadUint8();
            }

            else if (size == 2)
            {
                return OrgReadUint16();
            }

            else if (size == 4)
            {
                return OrgReadUint32();
            }
            else
            {
                return 0;
            }
        }

        public int ReadInt32()
        {
            return (int)ReadUint32();
        }

        public ulong ReadUint64()
        {
            eNDnetStreamMarker type;
            byte size;
            m_bStruckEndMarker = false;
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                return 0;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_INT64)
            {
                return 0;
            }

            if (size == 0)
            {
                return 0;
            }
            else if (size == 1)
            {
                return OrgReadUint8();
            }

            else if (size == 2)
            {
                return OrgReadUint16();
            }

            else if (size == 4)
            {
                return OrgReadUint32();
            }

            else if (size == 8)
            {
                return OrgReadUint64();
            }
            else
            {
                return 0;
            }
        }

        public long ReadInt64()
        {
            return (System.Int64) ReadUint64();
        }
        public string ReadString()
        {
            eNDnetStreamMarker type;
            byte size;
            m_bStruckEndMarker = false;
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                return string.Empty; ;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_TEXT)
            {
                return string.Empty; ;
            }
            
            ushort stringLen = ReadUint16();
            if (stringLen == 0)
            {
                return string.Empty;
            }
            byte[] receivedData = new byte[stringLen];
            myRead(receivedData, stringLen);
            //m_buf.Read(receivedData, (int)m_buf.Position, stringLen);
            byte endByte = ReadUint8();
            if (endByte != 0x7f)
            {
                return string.Empty;
            }
            return Encoding.UTF8.GetString(receivedData);
        }


        public int ReadBuf(out byte[] data)
        {

            eNDnetStreamMarker type;
            byte size;
            m_bStruckEndMarker = false;
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                data = null;
                return 0;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_BIN)
            {
                data = null;
                return 0;
            }

            ushort length = ReadUint16();
            data = new byte[length];
            myRead(data, length);
            return data.Length;
        }
        #endregion


        #region privateBaseIo
        
        private int _writeMarker(eNDnetStreamMarker marker, int sizebytes)
        {
	        byte m = (byte)((byte)marker << 4 | (byte)(sizebytes & 0xf));
            return OrgWriteUint8(m);	
        }

        int _ReadTypeSize(out eNDnetStreamMarker type, out byte size)
        {
            byte marker = 0;
            type = 0;
            size = 0;
            if (-1 == OrgWriteUint8(marker))
            {
                return -1;
            }
            m_bStruckEndMarker = false;
            if (marker == NET_STREAM_STRUCT_END_MARK)
            {
                m_bStruckEndMarker = true;

                return 0;
            }
            type = (eNDnetStreamMarker)((marker & 0xf0) >> 4);
            size =(byte)( marker & 0xf);
            return 0;
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
        #endregion

        #region original_write_func

        private int OrgWriteUint8(byte data)
        {
            m_buf[m_writeIndex++] = data;
            return 1;
        }

        private int OrgWriteInt8(sbyte data)
        {
            m_buf[m_writeIndex++] = (byte)data;
            return 1;
        }
        private int OrgWriteFloat(float data)
        {
            byte[] bufData = BitConverter.GetBytes(data);
            myWrite(bufData, 4);
            return bufData.Length;
        }

        private int OrgWriteUint16(ushort data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((short)data));
            myWrite(bufData, 2);
            return bufData.Length;
        }

        private int OrgWriteInt16(short data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            myWrite(bufData, 2);
            return bufData.Length;
        }

        private int OrgWriteUint32(uint data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((int)data));
            myWrite(bufData, 4);
            return bufData.Length;
        }

        private int OrgWriteInt32(int data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            myWrite(bufData, 4);
            return bufData.Length;
        }

        private int OrgWriteUint64(ulong data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((long)data));
            myWrite(bufData, 8);
            return bufData.Length;
        }

        private int OrgWriteInt64(long data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            myWrite(bufData, 8);
            return bufData.Length;
        }
        #endregion

        #region original_read_func

        private byte OrgReadUint8()
        {
            return m_buf[m_readIndex++];
        }

        private sbyte OrgReadInt8()
        {
            return (sbyte)m_buf[m_readIndex++];

        }
        private float OrgReadFloat()
        {
            byte[] receivedData = new byte[4];
            myRead(receivedData, 4);
            return BitConverter.ToSingle(receivedData, 0);
        }

        private ushort OrgReadUint16()
        {
            byte[] receivedData = new byte[2];
            myRead(receivedData, 2);
            return (ushort)IPAddress.NetworkToHostOrder((short)BitConverter.ToUInt16(receivedData, 0));
        }

        private short OrgReadInt16()
        {
            byte[] receivedData = new byte[2];
            myRead(receivedData, 2);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt16(receivedData, 0));
        }

        private uint OrgReadUint32()
        {
            byte[] receivedData = new byte[4];
            myRead(receivedData, 4);
            return (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(receivedData, 0));
        }

        private int OrgReadInt32()
        {
            byte[] receivedData = new byte[4];
            myRead(receivedData, 4);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt32(receivedData, 0));
        }

        private ulong OrgReadUint64()
        {
            byte[] receivedData = new byte[8];
            myRead(receivedData, 8);
            return (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(receivedData, 0));
        }

        private long OrgReadInt64()
        {
            byte[] receivedData = new byte[8];
            myRead(receivedData, 8);
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt64(receivedData, 0));
        }
        #endregion
    }
}
