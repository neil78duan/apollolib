using System;
using System.Collections.Generic;
using System.Text;


using System.Runtime.InteropServices;

using System.IO;
using System.Globalization;
using System.Net;

namespace NetMessage
{
    public struct binaryData
    {
        private byte[] m_data;
        public byte[] Data
        {
            get { return m_data; }
        }
        public int Length
        {
            get { return m_data.Length; }
        }
        public void SetData(byte[] data)
        {
            m_data = new byte[data.Length];
            data.CopyTo(m_data, 0);
        }

        public int Read(ref NDMsgStream netstream)
        {
            return netstream.ReadBuf(out m_data);
        }
        public int Write(ref NDMsgStream netstream)
        {
            return netstream.WriteBuf(m_data);
        }
    }
	public struct varDataType
	{
		enum NDVTYPE_ELEMENT_TYPE
		{
			ND_VT_INT ,
			ND_VT_FLOAT ,
			ND_VT_STRING,
			ND_VT_INT8 ,
			ND_VT_INT16 ,
			ND_VT_INT64 ,
			ND_VT_BINARY ,
		};
		private int m_type ;
		
		public int Type 
		{
            get { return m_type; }
        }
		
		private ulong m_iValue ;
		private float m_fValue;
		private string m_strValue ;
		private byte[] m_binValue;
		
		public void setInt(uint a) 
		{
			m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_INT ;
			m_iValue = (ulong)a ;
		}
		public void setInt8(byte a)  
		{
			m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_INT8 ;
			m_iValue = (ulong)a ;
		}
		public void setInt16(ushort a)  
		{
			m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_INT16 ;
			m_iValue = (ulong)a ;
		}
		
		public void setInt64(ulong a)  
		{
			m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_INT64 ;
			m_iValue = (ulong)a ;
		}
		public void setFloat(float a)  
		{
			m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_FLOAT ;
			m_fValue = a ;
		}
		public void setString(string text) 
		{
			m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_STRING ;
			m_strValue = text ;
			
		}
		public void setBinary(byte[] data)
		{
			m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_BINARY ;			
            m_binValue = new byte[data.Length];
			data.CopyTo(m_binValue, 0);
		}
		
		
		public uint getInt() 
		{
			return (uint)m_iValue;
		}
		public byte getInt8()
		{
			return (byte)m_iValue;
		}
		public ushort getInt16() 
		{
			return (ushort)m_iValue;
		}
		public ulong getInt64() 
		{
			return m_iValue;
		}
		public float getFloat() 
		{
			return m_fValue ;
		}
		public string getString() 
		{
			return m_strValue ;
		}
		
		public byte[]  getBinary()
		{
			return m_binValue ;
		}
		
		///read from stream
		public int Read(ref NDMsgStream netstream)
        {
			public int type = netstream.PeekType() ;
			
			if(type==NDMsgStream.eNDnetStreamMarker.ENDSTREAM_MARKER_INT8) {
				byte a= netstream.ReadUint8();
				setInt8(a) ;
			}
			else if(type==NDMsgStream.eNDnetStreamMarker.ENDSTREAM_MARKER_INT16) {				
				ushort a= netstream.ReadUint16();
				setInt16(a) ;
			}
			else if(type==NDMsgStream.eNDnetStreamMarker.ENDSTREAM_MARKER_INT32) {				
				uint a= netstream.ReadUint32();
				setInt32(a) ;
			}
			else if(type==NDMsgStream.eNDnetStreamMarker.ENDSTREAM_MARKER_INT64) {				
				ulong a= netstream.ReadUint64();
				setInt64(a) ;
			}
			else if(type==NDMsgStream.eNDnetStreamMarker.ENDSTREAM_MARKER_FLOAT) {
								
				float a= netstream.ReadFloat();
				setFloat(a) ;
			}
			else if(type==NDMsgStream.eNDnetStreamMarker.ENDSTREAM_MARKER_TEXT) {
				m_strValue = netstream.ReadString();				
				m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_STRING ;
			}
			
			else if(type==NDMsgStream.eNDnetStreamMarker.ENDSTREAM_MARKER_BIN) {				
				if(netstream.ReadBuf(out m_binValue)>0) {
					m_type = NDVTYPE_ELEMENT_TYPE.ND_VT_BINARY ;
				}
			}
			else {
				return -1;
			}
			return 0;
        }
        public int Write(ref NDMsgStream netstream)
        {
			int ret = -1 ;
			switch (m_type) {
				
			case NDVTYPE_ELEMENT_TYPE.ND_VT_INT :
				ret = netstream.WriteUint32((uint)m_iValue);
				break ;
			case NDVTYPE_ELEMENT_TYPE.ND_VT_FLOAT :
				ret = netstream.WriteFloat(m_fValue);
				break ;
			case NDVTYPE_ELEMENT_TYPE.ND_VT_STRING:
				ret = netstream.WriteString(m_strValue);
				break ;
			case NDVTYPE_ELEMENT_TYPE.ND_VT_INT8 :
				ret = netstream.WriteUint8((byte)m_iValue);
				break ;
			case NDVTYPE_ELEMENT_TYPE.ND_VT_INT16 :
				ret = netstream.WriteUint16((ushort)m_iValue);
				break ;
			case NDVTYPE_ELEMENT_TYPE.ND_VT_INT64 :
				ret = netstream.WriteUint64(m_iValue);
				break ;
			case NDVTYPE_ELEMENT_TYPE.ND_VT_BINARY :				
				ret = netstream.WriteBuf(m_binValue);
				break ;
			default :
				break ;
			}
			return -1;
        }
		
	}

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
            ENDSTREAM_CMD_SKIP_STRUCT_MARK,
            ENDSTREAM_CMD_ENABLE_STRUCT_MARK,

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
		private bool m_bDataReadEnd ;

        private bool m_bSkipEndMarker ;

        public bool IsReachedEnd()
		{
			return m_bDataReadEnd ;
		}

        public bool IsStructEnd()
        {
            return m_bStruckEndMarker || m_bDataReadEnd; 
        }

        public void ClearStructEnd()
        {
            m_bStruckEndMarker = false;
        }


        public int ValidLength 
        {
            get { return m_writeIndex; }
        }

        public NDMsgStream(int capacity = BUF_LENGTH)
        {
            m_readIndex = 0;
            m_writeIndex = 0;
            m_bStruckEndMarker = false;
			m_bDataReadEnd = false ;
            m_bSkipEndMarker = false;
        }

        public byte[] ToArray()
        {
            return m_buf;
        }

        //init from socket
        public int FromArray(byte[] data)
        {
            m_bStruckEndMarker = false;
            m_bDataReadEnd = false;

            m_readIndex = 0;
            //data.CopyTo(_m_buf, 0);
			Array.Copy(data,_m_buf,data.Length);
            return data.Length;
        }

        public int FromAddr(IntPtr streamBuf, int size)
        {

            m_bStruckEndMarker = false;
            m_bDataReadEnd = false;

            m_readIndex = 0;
            Marshal.Copy(streamBuf, m_buf, 0, size);
            return size;
        }
		public void WriteStructEnd()
		{
			OrgWriteUint8(NET_STREAM_STRUCT_END_MARK) ;
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
                    return -1;
                }
                int ret = OrgWriteUint8((byte)a);
                if(ret ==-1)
                {
                    return ret;
                }
                return ret + 1;
            }
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
                    return -1;
                }
                int ret = OrgWriteFloat(a);
                if (ret == -1)
                {
                    return ret;
                }
                return ret + 1;
            }
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
                int ret = OrgWriteUint16(a);
                if (ret == -1)
                {
                    return ret;
                }
                return ret + 1;
            }
        }

        public int WriteInt16(short data)
        {
            return WriteUint16((ushort)data) ;
        }

        public int WriteUint32(uint a)
        {
            int ret = 0;
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
                ret = OrgWriteUint8((byte)a);
            }
            else if (a <= 0xffff)
            {

                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT32, 2))
                {
                    return 0;
                }
                ret = OrgWriteUint16((ushort)a);
            }
            else
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT32, 4))
                {
                    return 0;
                }
                ret = OrgWriteUint32(a);
            }

            if (ret == -1)
            {
                return ret;
            }
            return ret + 1;
        }

        public int WriteInt32(int data)
        {
            return WriteUint32((uint)data);
        }

        public int WriteUint64(ulong a)
        {
            int ret = 0;
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
                ret = OrgWriteUint8((byte)a) ;
            }
            else if (a <= 0xffff)
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 2))
                {
                    return 0;
                }
                ret = OrgWriteUint16((ushort)a) ;
            }
            else if (a <= 0xffffffff)
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 4))
                {
                    return 0;
                }
                ret = OrgWriteUint32((uint)a);
            }
            else
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_INT64, 8))
                {
                    return 0;
                }
                ret = OrgWriteUint64(a) ;
            }

            if (ret == -1)
            {
                return ret;
            }
            return ret + 1;
        }

        public int WriteInt64(long data)
        {
            return WriteUint64((ulong) data) ;
        }

        public int WriteString(string data)
        {
            byte[] byteArray = Encoding.UTF8.GetBytes(data);

			if(byteArray.Length ==0) 
            {
                if(-1== _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_TEXT, 0))
                {
                    return -1;
                }
                return 1;
			}
            else
            {
                if (-1 == _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_TEXT, 1))
                {
                    return -1;
                }
            }


            if(-1==OrgWriteUint16((ushort)(byteArray.Length)) )
            {
                return -1;
            }
            if(-1==myWrite(byteArray, (int)byteArray.Length) )
            {
                return -1;
            }
            m_buf[m_writeIndex++] = 0x7f;
            return byteArray.Length + 4;
        }

        public int WriteBuf(byte[] data)
        {
            if (data == null)
            {
                _writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_BIN, 0);
                return 1;
            }
            ushort dataLenth = (ushort)data.Length;

			if (data.Length >= 0xffff)
            {
                return -1;
            }
            else if (data.Length == 0)
            {
                if(-1==_writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_BIN, 0) )
                {
                    return -1;
                }
                return 1;
            }
            else
            {
                if(-1==_writeMarker(eNDnetStreamMarker.ENDSTREAM_MARKER_BIN, 1) )
                {
                    return -1;
                }
            }

            if(-1==OrgWriteUint16(dataLenth))
            {
                return -1;
            }
            if(-1==myWrite(data, data.Length) )
            {
                return -1;
            }
            return data.Length + 3;
        }
        #endregion

        #region pub_read_func

        public byte ReadUint8()
        {
            eNDnetStreamMarker type;
            byte size;
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
            if (-1 == _ReadTypeSize(out type, out size) || m_bStruckEndMarker)
            {
                return string.Empty; ;
            }
            if (type != eNDnetStreamMarker.ENDSTREAM_MARKER_TEXT)
            {
                return string.Empty; ;
            }
            if (size == 0)
            {
                return string.Empty;
            }


            ushort stringLen = OrgReadUint16();
            if (stringLen == 0)
            {
                return string.Empty;
            }
            byte[] receivedData = new byte[stringLen];
            myRead(receivedData, stringLen);
            //m_buf.Read(receivedData, (int)m_buf.Position, stringLen);
            byte endByte = OrgReadUint8();
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

            if (size == 0)
            {
                data = null;
                return 0;

            }

            ushort length = OrgReadUint16();
            data = new byte[length];
            myRead(data, length);
            return data.Length;
        }

        public bool TrytoMoveStructEnd() 
        {
            if(m_bSkipEndMarker)
            {
                return true;
            }
            if (m_bStruckEndMarker)
            {
                m_bStruckEndMarker = false;
                return true;
            }

            int orgIndex =   m_readIndex;
            while(m_readIndex < m_buf.Length) 
            {
                eNDnetStreamMarker type;
                byte size;
                if (-1 == _ReadTypeSize(out type, out size) )
                {
                    m_readIndex = orgIndex ;
                    return false ;
                }
                if( m_bStruckEndMarker)
                {
                    m_bStruckEndMarker = false;
                    return true ;
                }
                if(type == eNDnetStreamMarker.ENDSTREAM_MARKER_UNDEFINE) {
                    m_readIndex = orgIndex ;
                    return false ;
                }

                else if (type == eNDnetStreamMarker.ENDSTREAM_MARKER_BIN)
                {
                    if (size != 0)
                    {
                        ushort data_size = OrgReadUint16();
                        m_readIndex += data_size;
                    }

                }
                else if(type == eNDnetStreamMarker.ENDSTREAM_MARKER_TEXT)
                {
                    if (size != 0)
                    {
                        ushort data_size = OrgReadUint16();
                        m_readIndex += data_size + 1;
                    }
                }
                else {
                    m_readIndex += size ;
                }
            }
            
            m_readIndex = orgIndex ;
            return false ;
        }
       
        #endregion


        #region privateBaseIo
        
        private int _writeMarker(eNDnetStreamMarker marker, int sizebytes)
        {
	        byte m = (byte)((byte)marker << 4 | (byte)(sizebytes & 0xf));
            return OrgWriteUint8(m);	
        }
		public int PeekType()
		{
			int tmpindex = m_readIndex ;
			
            eNDnetStreamMarker type;
            byte size;
			if (-1 == _ReadTypeSize(out type, out size) ) {
				return -1 ;
			}
			m_readIndex = tmpindex;
			return (int) type ;
		}

        int _ReadTypeSize(out eNDnetStreamMarker type, out byte size)
        {
            type = 0;
            size = 0;
            m_bStruckEndMarker = false;
            byte marker;

            BEGIN_READ_MARKER:
            marker = OrgReadUint8();
            if (marker == 0 || m_bDataReadEnd)
            {
                return -1;
            }
            
            if (marker == NET_STREAM_STRUCT_END_MARK)
            {
                if (m_bSkipEndMarker)
                {
                    goto BEGIN_READ_MARKER;
                }
                m_bStruckEndMarker = true;
                return 0;
            }
            type = (eNDnetStreamMarker)((marker & 0xf0) >> 4);
            size =(byte)( marker & 0xf);
            
            if (type == eNDnetStreamMarker.ENDSTREAM_CMD_SKIP_STRUCT_MARK)
            {
                m_bSkipEndMarker = true;
                goto BEGIN_READ_MARKER;
            }
            else if (type == eNDnetStreamMarker.ENDSTREAM_CMD_ENABLE_STRUCT_MARK)
            {
                m_bSkipEndMarker = false;
                goto BEGIN_READ_MARKER;
            }

            return 0;
        }
      

        private int myWrite(byte[] data, int len)
        {
            if (m_writeIndex + len > m_buf.Length)
            {
                return -1;
            }
            for (var i = 0; i < len; ++i)
            {
                m_buf[m_writeIndex++] = data[i];
            }
            return len;
        }


        private int myRead(byte[] data, int len)
        {
			if(m_readIndex + len > m_buf.Length)
			{
				m_bDataReadEnd = true ;
				return 1 ;
			}
            for (var i = 0; i < len; ++i)
            {
                data[i] = m_buf[m_readIndex++] ;
            }
            return len;
        }
        #endregion

        #region original_write_func

        private int OrgWriteUint8(byte data)
        {
            if (m_writeIndex + 1 > m_buf.Length)  {
                return -1;
            }

            m_buf[m_writeIndex++] = data;
            return 1;
        }

        private int OrgWriteInt8(sbyte data)
        {
            if (m_writeIndex + 1 > m_buf.Length)
            {
                return -1;
            }

            m_buf[m_writeIndex++] = (byte)data;
            return 1;
        }
        private int OrgWriteFloat(float data)
        {
           
            byte[] bufData = BitConverter.GetBytes(data);
            return myWrite(bufData, 4);
        }

        private int OrgWriteUint16(ushort data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((short)data));
            return myWrite(bufData, 2);
        }

        private int OrgWriteInt16(short data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            return myWrite(bufData, 2);
        }

        private int OrgWriteUint32(uint data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((int)data));
            return myWrite(bufData, 4);
        }

        private int OrgWriteInt32(int data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            return myWrite(bufData, 4);
        }

        private int OrgWriteUint64(ulong data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder((long)data));
            return myWrite(bufData, 8);
        }

        private int OrgWriteInt64(long data)
        {
            byte[] bufData = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(data));
            return  myWrite(bufData, 8);
        }
        #endregion

        #region original_read_func

        private byte OrgReadUint8()
        {
			if(m_readIndex + 1 > m_buf.Length)
			{
				m_bDataReadEnd = true ;
				return 0 ;
			}
			
            return m_buf[m_readIndex++];
        }

        private sbyte OrgReadInt8()
        {
            if (m_readIndex + 1 > m_buf.Length)
            {
                m_bDataReadEnd = true;
                return 0;
            }
            return (sbyte)m_buf[m_readIndex++];

        }
        private float OrgReadFloat()
        {
            byte[] receivedData = new byte[4];
            if(-1==myRead(receivedData, 4))
            {
                return 0;
            }
            return BitConverter.ToSingle(receivedData, 0);
        }

        private ushort OrgReadUint16()
        {
            byte[] receivedData = new byte[2];
            //myRead(receivedData, 2);
            if (-1 == myRead(receivedData, 2))
            {
                return 0;
            }
            return (ushort)IPAddress.NetworkToHostOrder((short)BitConverter.ToUInt16(receivedData, 0));
        }

        private short OrgReadInt16()
        {
            byte[] receivedData = new byte[2];
            //myRead(receivedData, 2);
            if (-1 == myRead(receivedData, 2))
            {
                return 0;
            }
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt16(receivedData, 0));
        }

        private uint OrgReadUint32()
        {
            byte[] receivedData = new byte[4];
            //myRead(receivedData, 4);
            if (-1 == myRead(receivedData, 4))
            {
                return 0;
            }
            return (uint)IPAddress.NetworkToHostOrder((int)BitConverter.ToUInt32(receivedData, 0));
        }

        private int OrgReadInt32()
        {
            byte[] receivedData = new byte[4];
            //myRead(receivedData, 4);
            if (-1 == myRead(receivedData, 4))
            {
                return 0;
            }
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt32(receivedData, 0));
        }

        private ulong OrgReadUint64()
        {
            byte[] receivedData = new byte[8];
            //myRead(receivedData, 8);
            if (-1 == myRead(receivedData, 8))
            {
                return 0;
            }
            return (ulong)IPAddress.NetworkToHostOrder((long)BitConverter.ToUInt64(receivedData, 0));
        }

        private long OrgReadInt64()
        {
            byte[] receivedData = new byte[8];
            //myRead(receivedData, 8);
            if (-1 == myRead(receivedData, 8))
            {
                return 0;
            }
            return IPAddress.NetworkToHostOrder(BitConverter.ToInt64(receivedData, 0));
        }
        #endregion
    }
}
