/* file ndmath.cpp
 * 
 * math 
 *
 * create by duan
 *
 * 2011/2/24 13:30:52
 */

#include "ndmath.h"
#include <memory.h>

const vector2f nd_cossin_buff[] = 
{    
    {1.00000f,0.00000f},{0.99985f,0.01745f},{0.99939f,0.03490f},{0.99863f,0.05234f},{0.99756f,0.06976f},{0.99619f,0.08716f},{0.99452f,0.10453f},{0.99255f,0.12187f},{0.99027f,0.13917f},{0.98769f,0.15643f},
    {0.98481f,0.17365f},{0.98163f,0.19081f},{0.97815f,0.20791f},{0.97437f,0.22495f},{0.97030f,0.24192f},{0.96593f,0.25882f},{0.96126f,0.27564f},{0.95630f,0.29237f},{0.95106f,0.30902f},{0.94552f,0.32557f},
    {0.93969f,0.34202f},{0.93358f,0.35837f},{0.92718f,0.37461f},{0.92050f,0.39073f},{0.91355f,0.40674f},{0.90631f,0.42262f},{0.89879f,0.43837f},{0.89101f,0.45399f},{0.88295f,0.46947f},{0.87462f,0.48481f},
    {0.86603f,0.50000f},{0.85717f,0.51504f},{0.84805f,0.52992f},{0.83867f,0.54464f},{0.82904f,0.55919f},{0.81915f,0.57358f},{0.80902f,0.58778f},{0.79864f,0.60181f},{0.78801f,0.61566f},{0.77715f,0.62932f},
    {0.76604f,0.64279f},{0.75471f,0.65606f},{0.74315f,0.66913f},{0.73135f,0.68200f},{0.71934f,0.69466f},{0.70711f,0.70711f},{0.69466f,0.71934f},{0.68200f,0.73135f},{0.66913f,0.74314f},{0.65606f,0.75471f},
    {0.64279f,0.76604f},{0.62932f,0.77715f},{0.61566f,0.78801f},{0.60182f,0.79864f},{0.58779f,0.80902f},{0.57358f,0.81915f},{0.55919f,0.82904f},{0.54464f,0.83867f},{0.52992f,0.84805f},{0.51504f,0.85717f},
    {0.50000f,0.86603f},{0.48481f,0.87462f},{0.46947f,0.88295f},{0.45399f,0.89101f},{0.43837f,0.89879f},{0.42262f,0.90631f},{0.40674f,0.91355f},{0.39073f,0.92050f},{0.37461f,0.92718f},{0.35837f,0.93358f},
    {0.34202f,0.93969f},{0.32557f,0.94552f},{0.30902f,0.95106f},{0.29237f,0.95630f},{0.27564f,0.96126f},{0.25882f,0.96593f},{0.24192f,0.97030f},{0.22495f,0.97437f},{0.20791f,0.97815f},{0.19081f,0.98163f},
    {0.17365f,0.98481f},{0.15644f,0.98769f},{0.13917f,0.99027f},{0.12187f,0.99255f},{0.10453f,0.99452f},{0.08716f,0.99619f},{0.06976f,0.99756f},{0.05234f,0.99863f},{0.03490f,0.99939f},{0.01745f,0.99985f},
    {0.00000f,1.00000f},{-0.01745f,0.99985f},{-0.03490f,0.99939f},{-0.05233f,0.99863f},{-0.06976f,0.99756f},{-0.08715f,0.99619f},{-0.10453f,0.99452f},{-0.12187f,0.99255f},{-0.13917f,0.99027f},{-0.15643f,0.98769f},
    {-0.17365f,0.98481f},{-0.19081f,0.98163f},{-0.20791f,0.97815f},{-0.22495f,0.97437f},{-0.24192f,0.97030f},{-0.25882f,0.96593f},{-0.27564f,0.96126f},{-0.29237f,0.95631f},{-0.30902f,0.95106f},{-0.32557f,0.94552f},
    {-0.34202f,0.93969f},{-0.35837f,0.93358f},{-0.37461f,0.92718f},{-0.39073f,0.92051f},{-0.40674f,0.91355f},{-0.42262f,0.90631f},{-0.43837f,0.89879f},{-0.45399f,0.89101f},{-0.46947f,0.88295f},{-0.48481f,0.87462f},
    {-0.50000f,0.86603f},{-0.51504f,0.85717f},{-0.52992f,0.84805f},{-0.54464f,0.83867f},{-0.55919f,0.82904f},{-0.57357f,0.81915f},{-0.58778f,0.80902f},{-0.60181f,0.79864f},{-0.61566f,0.78801f},{-0.62932f,0.77715f},
    {-0.64279f,0.76605f},{-0.65606f,0.75471f},{-0.66913f,0.74315f},{-0.68200f,0.73136f},{-0.69466f,0.71934f},{-0.70711f,0.70711f},{-0.71934f,0.69466f},{-0.73135f,0.68200f},{-0.74314f,0.66913f},{-0.75471f,0.65606f},
    {-0.76604f,0.64279f},{-0.77714f,0.62932f},{-0.78801f,0.61566f},{-0.79863f,0.60182f},{-0.80902f,0.58779f},{-0.81915f,0.57358f},{-0.82904f,0.55919f},{-0.83867f,0.54464f},{-0.84805f,0.52992f},{-0.85717f,0.51504f},
    {-0.86602f,0.50000f},{-0.87462f,0.48481f},{-0.88295f,0.46947f},{-0.89101f,0.45399f},{-0.89879f,0.43837f},{-0.90631f,0.42262f},{-0.91354f,0.40674f},{-0.92050f,0.39073f},{-0.92718f,0.37461f},{-0.93358f,0.35837f},
    {-0.93969f,0.34202f},{-0.94552f,0.32557f},{-0.95106f,0.30902f},{-0.95630f,0.29237f},{-0.96126f,0.27564f},{-0.96593f,0.25882f},{-0.97030f,0.24192f},{-0.97437f,0.22495f},{-0.97815f,0.20791f},{-0.98163f,0.19081f},
    {-0.98481f,0.17365f},{-0.98769f,0.15644f},{-0.99027f,0.13918f},{-0.99255f,0.12187f},{-0.99452f,0.10453f},{-0.99619f,0.08716f},{-0.99756f,0.06976f},{-0.99863f,0.05234f},{-0.99939f,0.03490f},{-0.99985f,0.01745f},
    {-1.00000f,0.00000f},{-0.99985f,-0.01745f},{-0.99939f,-0.03490f},{-0.99863f,-0.05233f},{-0.99756f,-0.06975f},{-0.99619f,-0.08715f},{-0.99452f,-0.10453f},{-0.99255f,-0.12187f},{-0.99027f,-0.13917f},{-0.98769f,-0.15643f},
    {-0.98481f,-0.17365f},{-0.98163f,-0.19081f},{-0.97815f,-0.20791f},{-0.97437f,-0.22495f},{-0.97030f,-0.24192f},{-0.96593f,-0.25882f},{-0.96126f,-0.27563f},{-0.95631f,-0.29237f},{-0.95106f,-0.30901f},{-0.94552f,-0.32557f},
    {-0.93969f,-0.34202f},{-0.93358f,-0.35837f},{-0.92718f,-0.37460f},{-0.92051f,-0.39073f},{-0.91355f,-0.40673f},{-0.90631f,-0.42262f},{-0.89880f,-0.43837f},{-0.89101f,-0.45399f},{-0.88295f,-0.46947f},{-0.87462f,-0.48481f},
    {-0.86603f,-0.50000f},{-0.85717f,-0.51504f},{-0.84805f,-0.52992f},{-0.83867f,-0.54464f},{-0.82904f,-0.55919f},{-0.81915f,-0.57357f},{-0.80902f,-0.58778f},{-0.79864f,-0.60181f},{-0.78801f,-0.61566f},{-0.77715f,-0.62932f},
    {-0.76605f,-0.64279f},{-0.75471f,-0.65606f},{-0.74315f,-0.66913f},{-0.73136f,-0.68200f},{-0.71934f,-0.69466f},{-0.70711f,-0.70710f},{-0.69466f,-0.71934f},{-0.68200f,-0.73135f},{-0.66913f,-0.74314f},{-0.65606f,-0.75471f},
    {-0.64279f,-0.76604f},{-0.62932f,-0.77714f},{-0.61566f,-0.78801f},{-0.60182f,-0.79863f},{-0.58779f,-0.80902f},{-0.57358f,-0.81915f},{-0.55920f,-0.82904f},{-0.54464f,-0.83867f},{-0.52992f,-0.84805f},{-0.51504f,-0.85717f},
    {-0.50000f,-0.86602f},{-0.48481f,-0.87462f},{-0.46947f,-0.88295f},{-0.45399f,-0.89100f},{-0.43837f,-0.89879f},{-0.42262f,-0.90631f},{-0.40674f,-0.91354f},{-0.39073f,-0.92050f},{-0.37461f,-0.92718f},{-0.35837f,-0.93358f},
    {-0.34202f,-0.93969f},{-0.32557f,-0.94552f},{-0.30902f,-0.95106f},{-0.29238f,-0.95630f},{-0.27564f,-0.96126f},{-0.25882f,-0.96592f},{-0.24193f,-0.97029f},{-0.22495f,-0.97437f},{-0.20792f,-0.97815f},{-0.19081f,-0.98163f},
    {-0.17365f,-0.98481f},{-0.15644f,-0.98769f},{-0.13918f,-0.99027f},{-0.12187f,-0.99255f},{-0.10453f,-0.99452f},{-0.08716f,-0.99619f},{-0.06976f,-0.99756f},{-0.05234f,-0.99863f},{-0.03490f,-0.99939f},{-0.01746f,-0.99985f},
    {-0.00000f,-1.00000f},{0.01745f,-0.99985f},{0.03490f,-0.99939f},{0.05233f,-0.99863f},{0.06975f,-0.99756f},{0.08715f,-0.99620f},{0.10452f,-0.99452f},{0.12187f,-0.99255f},{0.13917f,-0.99027f},{0.15643f,-0.98769f},
    {0.17364f,-0.98481f},{0.19081f,-0.98163f},{0.20791f,-0.97815f},{0.22495f,-0.97437f},{0.24192f,-0.97030f},{0.25882f,-0.96593f},{0.27563f,-0.96126f},{0.29237f,-0.95631f},{0.30901f,-0.95106f},{0.32556f,-0.94552f},
    {0.34202f,-0.93969f},{0.35836f,-0.93358f},{0.37460f,-0.92719f},{0.39073f,-0.92051f},{0.40673f,-0.91355f},{0.42261f,-0.90631f},{0.43837f,-0.89880f},{0.45399f,-0.89101f},{0.46947f,-0.88295f},{0.48481f,-0.87462f},
    {0.50000f,-0.86603f},{0.51503f,-0.85717f},{0.52992f,-0.84805f},{0.54464f,-0.83867f},{0.55919f,-0.82904f},{0.57357f,-0.81915f},{0.58778f,-0.80902f},{0.60181f,-0.79864f},{0.61566f,-0.78801f},{0.62932f,-0.77715f},
    {0.64278f,-0.76605f},{0.65606f,-0.75471f},{0.66913f,-0.74315f},{0.68200f,-0.73136f},{0.69466f,-0.71934f},{0.70710f,-0.70711f},{0.71934f,-0.69466f},{0.73135f,-0.68200f},{0.74314f,-0.66913f},{0.75471f,-0.65606f},
    {0.76604f,-0.64279f},{0.77714f,-0.62932f},{0.78801f,-0.61567f},{0.79863f,-0.60182f},{0.80901f,-0.58779f},{0.81915f,-0.57358f},{0.82904f,-0.55920f},{0.83867f,-0.54464f},{0.84805f,-0.52992f},{0.85716f,-0.51504f},
    {0.86602f,-0.50000f},{0.87462f,-0.48481f},{0.88295f,-0.46948f},{0.89100f,-0.45399f},{0.89879f,-0.43838f},{0.90631f,-0.42262f},{0.91354f,-0.40674f},{0.92050f,-0.39074f},{0.92718f,-0.37461f},{0.93358f,-0.35837f},
    {0.93969f,-0.34202f},{0.94552f,-0.32557f},{0.95106f,-0.30902f},{0.95630f,-0.29238f},{0.96126f,-0.27564f},{0.96592f,-0.25882f},{0.97029f,-0.24193f},{0.97437f,-0.22496f},{0.97815f,-0.20792f},{0.98163f,-0.19081f},
    {0.98481f,-0.17365f},{0.98769f,-0.15644f},{0.99027f,-0.13918f},{0.99255f,-0.12187f},{0.99452f,-0.10453f},{0.99619f,-0.08716f},{0.99756f,-0.06976f},{0.99863f,-0.05234f},{0.99939f,-0.03490f},{0.99985f,-0.01746f},
};


float InvSqrt (float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1); // 计算第一个近似根
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x); // 牛顿迭代法
	return x;
}

double InvSqrt(double number)
{
	double x2, y;
	const double threehalfs = 1.5F;
	union
	{
		double d;
		unsigned long long i;
	}d;
	x2 = number * 0.5F;
	y = number;
	d.d =  y;
	d.i = (unsigned long long)0x5fe6ec85e7de30daLL - (d.i >> 1);
	y = d.d;
	y = y * (threehalfs - (x2 * y * y)); //1st iteration
	y = y * (threehalfs - (x2 * y * y)); //2nd iteration, this can be removed
	return y;
}

int  NDVec2::normalize()
{
	float length = InvSqrt (x*x + y*y) ;
	if(length <= 0.00001) {
		return 0 ;
	}
	x *= length ;
	y *= length ;
	return 0;
}

int  NDVec3::normalize()
{

	float length = InvSqrt (x*x + y*y+ z*z) ;
	if(length <= 0.00001f) {
		return 0 ;
	}
	x *= length ;
	y *= length ;
	z *= length;
	return 0;
}

const NDVec3 NDVec3::operator* (const NDVec3 &v)const
{
	float valx = y * v.z - v.y * z;
	float valy = z * v.x - v.z * x;
	float valz = x * v.y - v.x * y;
	return NDVec3(valx,valy,valz);
}

void NDVec3::crossproduct(const NDVec3& v , NDVec3 &out) 
{
	float valx = y * v.z - v.y * z;
	float valy = z * v.x - v.z * x;
	float valz = x * v.y - v.x * y;
	out.x = valx ;
	out.y = valy ;
	out.z = valz ;
}

void NDVec3::crossproduct(const NDVec3& v) 
{
	float valx = y * v.z - v.y * z;
	float valy = z * v.x - v.z * x;
	float valz = x * v.y - v.x * y;
	x = valx ;
	y = valy ;
	z = valz ;
}

//2D中对一向量旋转theta角度, 逆时针为正半轴
NDVec2& RotateAngleCCW(NDVec2& vec, float theta)
{
	NDVec2 tmp;
	float radian = theta * PI / 180.0f;

	tmp.x = cos(radian) * vec.x - sin(radian) * vec.y;
	tmp.y = sin(radian) * vec.x + cos(radian) * vec.y;

	vec = tmp;

	return vec;
}

//融入NDVec3接口但只旋转Z轴
NDVec3& RotateAngleCCWV3(NDVec3& vec, float theta)
{
	NDVec3 tmp;
	float radian = theta * PI / 180.0f;

	tmp.x = cos(radian) * vec.x - sin(radian) * vec.y;
	tmp.y = sin(radian) * vec.x + cos(radian) * vec.y;
	tmp.z = vec.z;

	vec = tmp;

	return vec;
}

//返回两向量夹角的弧度值
float RadianbetweenTwoNormalizeVector(const NDVec3& u, const NDVec3& v)
{
	NDVec2 u2(u.x, u.y);
	NDVec2 v2(v.x, v.y);
	float radian = acosf(u2.dotproduct(v2));
	return radian;
}

//判断两向量的位置关系 
bool IsVectorOrderCW(const NDVec3& u, const NDVec3& v)
{
	float det = u.x * v.y - u.y * v.x;
	return det > 0 ? true : false;
}

bool IsInDistance(const NDVec3& p1, const NDVec3& p2, float distance)
{
	NDVec3 tmp = p1;
	tmp.sub(p2);

	float d2 = tmp.length2d();

	return d2 < distance ? true : false;
}
bool IsInDistance2D(vector2f* p1,vector2f* p2, float dist)
{
	float x,y ,mindist;
#define CHECK_INTERVAL(pos1,pos2,realdist, interval) do {	\
		if (pos1 > pos2){					\
			interval = pos1 - pos2 ;		\
		}									\
		else {								\
			interval = pos2 - pos1 ;		\
		}									\
		if (interval > realdist){			\
			return false ;					\
		}									\
	}while(0)
	
	CHECK_INTERVAL(p1->x, p2->x,dist, x);
	CHECK_INTERVAL(p1->y, p2->y,dist, y);
#undef CHECK_INTERVAL
	mindist = dist * 0.7071f ;
	if (x <mindist && y < mindist){
		return true ;
	}
	return (x*x + y*y )< (dist *dist );
}

float CalculateRadianInUE3(const NDVec3& eye, const NDVec3& aim)
{
	NDVec3 dir = aim - eye;
	dir.normalize();
	NDVec3 unit(1.0f, 0.0f, 0.0f);
	float radian = RadianbetweenTwoNormalizeVector(dir, unit);
	if (IsVectorOrderCW(dir, unit))
		radian = TWOPI - radian;

	return radian;
}
// 
// NDVec3 RandomRangeByRangeRadius(const NDVec3& eye, unsigned short minRadius, unsigned short maxRadius)
// {
// 	NDVec3 unit(1.0f, 0.0f, 0.0f);
// 	NDVec3 after = RotateAngleCCWV3(unit, RandomRange(0.0f, 360.0f));
// 	unsigned short radius = RandomRange((unsigned short)minRadius, (unsigned short)maxRadius);
// 	after.mul(radius);
// 	after += eye;
// 
// 	return after;
// }

// ----------------------------------------------------------------------------

NDRect2D::NDRect2D( const rect2D& rect )
    : rect2D(rect)
{
    //ND_TRACE_FUNC();

    _BuildFromRect2D();

    //nd_assert(v4.x != 0 && v4.y != 0);
}
//
//NDRect2D::NDRect2D( const NDVec2& vLocation, const NDVec2& vDir, const float width )
//{
//    // ND_TRACE_FUNC();
//
//
//}

void NDRect2D::Build( const NDVec2& vLocation, const NDVec2& vDir, const float width )
{
    // ND_TRACE_FUNC();

    NDVec2 vDirPerp;
    vDir.perp(vDirPerp);
    vDirPerp.normalize();
    vDirPerp.mul(width);

    v[0] = vLocation;
    v[1] = vLocation + vDir;
    v[2] = vLocation + vDirPerp;
    v[3] = vLocation + vDirPerp + vDir;

    _BuildFromRect2D();
}

void NDRect2D::_BuildFromRect2D()
{
    // ND_TRACE_FUNC();

    NDVec2 vTmp1,vTmp2,vTmp3;
    vTmp1.x = v[1].x - v[0].x;
    vTmp1.y = v[1].y - v[0].y;

    vTmp2.x = v[2].x - v[0].x;
    vTmp2.y = v[2].y - v[0].y;

    vTmp3.x = v[3].x - v[0].x;
    vTmp3.y = v[3].y - v[0].y;

    NDVec2 vTmp;
    vTmp = vTmp1 - vTmp2;

    if (vTmp.IsParallel(vTmp3))
    {
        vTmp = vTmp3 - vTmp2;
        if (vTmp.IsParallel(vTmp1))
        {
            m_vLocation = v[0];
            m_fLength[0] = vTmp3.length();
            m_fLength[1] = vTmp1.length();

            vTmp3.normalize();
            vTmp1.normalize();

            m_vSide[0] = vTmp3;
            m_vSide[1] = vTmp1;
        }
        else
        {
            m_vLocation = v[0];
            m_fLength[0] = vTmp3.length();
            m_fLength[1] = vTmp2.length();

            vTmp3.normalize();
            vTmp2.normalize();

            m_vSide[0] = vTmp3;
            m_vSide[1] = vTmp2;
        }
    }
    else
    {
        m_vLocation = v[0];
        m_fLength[0] = vTmp1.length();
        m_fLength[1] = vTmp2.length();

        vTmp1.normalize();
        vTmp2.normalize();

        m_vSide[0] = vTmp1;
        m_vSide[1] = vTmp2;
    }
}


bool NDRect2D::PtInRect( const vector2f& pt )
{
    //ND_TRACE_FUNC();

    if (m_fLength[0] == 0 || m_fLength[1] == 0)
    {
        return false;
    }

    float u,v;

    NDVec2 ndV2(pt.x - m_vLocation.x, pt.y - m_vLocation.y);

    float f11 = (m_vSide[0] * m_vSide[0]);
    float f12 = (m_vSide[0] * m_vSide[1]);
    float f13 = (m_vSide[0] * ndV2);
    float f22 = (m_vSide[1] * m_vSide[1]);
    float f23 = (m_vSide[1] * ndV2);

    float fDen = f11 * f22 - f12 * f12;
    if ( -0.00001f <= fDen && fDen <= 0.00001f)
    {
        return false;
    }

    v = (f11 * f23 - f12 * f13) / fDen;
    u = (f22 * f13 - f23 * f12) / fDen;

    if (0 <= u && u <= 1 && 0 <= v && v <= 1 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool NDRect2D::Overlapped( const NDRect2D& rect )
{
    //ND_TRACE_FUNC();

    if (m_fLength[0] == 0 || m_fLength[1] == 0)
    {
        return false;
    }

    return s_SeparatingAxisCollision(*this, rect);
}

void NDRect2D::Center( vector2f& pt )
{
    NDVec2 vTmp = m_vLocation + 
                  (m_vSide[0] * m_fLength[0] * 0.5) + 
                  (m_vSide[1] * m_fLength[1] * 0.5);

    pt.x = vTmp.x;
    pt.y = vTmp.y;
}

void NDRect2D::s_CalcRectProj(const NDVec2& vSPAxis, const NDRect2D& r, float& fMin, float& fMax)
{
    NDVec2 vTmp;
    vTmp.x = r.v[0].x;
    vTmp.y = r.v[0].y;
    fMin = fMax = vSPAxis * vTmp;

    for (int i = 1; i < 4; ++i)
    {
        vTmp.x = r.v[i].x;
        vTmp.y = r.v[i].y;

        float fTmp = vSPAxis * vTmp;
        if (fTmp < fMin)
        {
            fMin = fTmp;
        }
        else if (fTmp > fMax)
        {
            fMax = fTmp;
        }
    }
}

bool NDRect2D::s_SeparatingAxisCollision(const NDRect2D& r1, const NDRect2D& r2 )
{
    //ND_TRACE_FUNC();

    float min1, max1, min2, max2;
    NDVec2 vPerp;

    for (int i = 0; i < 2; ++i)
    {
        r1.m_vSide[i].perp(vPerp);
        s_CalcRectProj(vPerp, r1, min1, max1);
        s_CalcRectProj(vPerp, r2, min2, max2);
        if (max1 < min2 || max2 < min1)
        {
            return false;
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        r2.m_vSide[i].perp(vPerp);
        s_CalcRectProj(vPerp, r1, min1, max1);
        s_CalcRectProj(vPerp, r2, min2, max2);
        if (max1 < min2 || max2 < min1)
        {
            return false;
        }
    }

    return true;
}

void NDRect2D::GetAABB( AABB& aabb ) const
{
    //ND_TRACE_FUNC();

    aabb.vTLPos.x = aabb.vBRPos.x = v[0].x;
    aabb.vTLPos.y = aabb.vBRPos.y = v[0].y;

    for (int i = 1; i < 4; ++i)
    {
        if (v[i].x < aabb.vBRPos.x)
        {
            aabb.vBRPos.x = v[i].x;
        }
        else if (v[i].x > aabb.vTLPos.x)
        {
            aabb.vTLPos.x = v[i].x;
        }

        if (v[i].y < aabb.vTLPos.y)
        {
            aabb.vTLPos.y = v[i].y;
        }
        else if (v[i].y > aabb.vBRPos.y)
        {
            aabb.vBRPos.y = v[i].y;
        }
    }
}





