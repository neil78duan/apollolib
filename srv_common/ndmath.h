/* file ndmath.h
 *
 * math 
 *
 * create by duan 
 * 2011/2/24 11:28:54
 */

#ifndef _NDMATH_H_
#define _NDMATH_H_

#include <math.h>
#include <cstdlib>

struct vector2f
{
	float x ,y;
} ;

struct vector3f
{
	float x ,y,z;
} ;

// axis-aligned bounding box
struct AABB
{
    vector2f vTLPos;
    vector2f vBRPos;
};

struct rect2D
{
    vector2f v[4];
    //vector2f v1,v2,v3,v4;   // 矩形的4个点
};

float InvSqrt (float x);

class NDVec2 :public vector2f
{
public:
	NDVec2() {
		x = 0 ;y=0;
	}
	NDVec2(float v1, float v2){x=v1;y=v2;}
	NDVec2(float *v){x=v[0];y=v[1];}
	~NDVec2() {}

	void add(NDVec2& v){x += v.x; y += v.y;}
	NDVec2 add2(const NDVec2& v) const { return NDVec2(x + v.x, y + v.y);}
	void sub(NDVec2& v){x -= v.x; y -= v.y;}
	void mul(float f){ x*= f; y*=f;}

	float length() const{ return sqrt(x * x + y * y) ;}
	float squard() const{return (x * x + y * y) ;}
    void perp(vector2f& v){ v.x = y; v.y = -x; }
    void perp(vector2f& v) const { v.x = y; v.y = -x; }

	int  normalize();

	const NDVec2 operator+ (const NDVec2 &v)const { return NDVec2(x + v.x, y + v.y);}
	const NDVec2 operator- (const NDVec2 &v)const { return NDVec2(x - v.x, y - v.y);} 
	const NDVec2 operator* (float f)const { return NDVec2(x * f , y * f);}
    float operator* (const NDVec2& v) { return (v.x*x + v.y *y) ; }
    float operator* (const NDVec2& v) const { return (v.x*x + v.y *y) ; }

	bool operator ==(const NDVec2& v) {return (x == v.x  && y == v.y); }
	bool operator ==(const NDVec2& v) const {return (x == v.x  && y == v.y);}
	bool IsZero(){return (x ==0 && y ==0);}
    bool IsParallel(const vector2f& v) { return x*v.y == y*v.x; }

	float dotproduct(const NDVec2& v){return (v.x*x + v.y *y) ;};

	NDVec2& operator +=(const NDVec2& v){ x += v.x ; y += v.y ; return *this; }
	NDVec2& operator -=(const NDVec2& v){ x -= v.x ; y -= v.y ; return *this; }
	NDVec2& operator *=(float s){ x *= s ; y *= s ; return *this; };
	NDVec2& operator = (const NDVec2& v){ x = v.x ; y = v.y ; return *this; };
	NDVec2& operator = (const vector2f& v){ x = v.x ; y = v.y ; return *this; };
	NDVec2& operator = (const float *v){ x = v[0] ; y = v[1] ; return *this; };
	//float x,y;
};

class NDVec3 : public vector3f
{
public:
	NDVec3() {x=0,y=0;z=0;}
	NDVec3(float v1, float v2,float v3)
	{
		x = v1; y = v2;z = v3;
	}
	NDVec3(float *v)
	{
		x = v[0]; y = v[1];z = v[2];
	}
	NDVec3(vector3f &v)
	{
		x = v.x; y = v.y;z = v.z;
	}
	~NDVec3() {}

	void add(NDVec3& v){x += v.x; y += v.y; z += v.z;}
	NDVec3 add2(const NDVec3& v) const { return NDVec3(x + v.x, y + v.y, z + v.z);}
	void sub(const NDVec3& v){x -= v.x; y -= v.y;z -= v.z;}
	void mul(float f){ x*= f; y*=f;z*=f;}
	void SetZero() { x=0; y = 0; z = 0;}

	float length(){ return sqrt(x * x + y * y + z*z) ;}
	float length() const {return sqrt(x * x + y * y + z*z);}
	float squard() const {return (x * x + y * y + z * z) ;}

	float length2d() const {return sqrt(x * x + y * y );}
	float squard2d() const {return (x * x + y * y) ;}


	const NDVec3 operator+ (const NDVec3 &v)const{ return NDVec3(x + v.x, y + v.y, z + v.z);}
	const NDVec3 operator- (const NDVec3 &v)const{ return NDVec3(x - v.x, y - v.y, z - v.z);} 
	const NDVec3 operator* (float f) const { return NDVec3(x * f , y * f , z*f);}
	const NDVec3 operator* (const NDVec3 &v)const;

	bool operator ==(NDVec3& v) {return (x == v.x  && y == v.y && z == v.z); }
	bool IsZero(){return (squard() < 0.00001f);}
    

	int  normalize();
	float dotproduct(const NDVec3& v) const {return (v.x*x + v.y *y + z*v.z) ;}
	void crossproduct(const NDVec3& v) ;
	void crossproduct(const NDVec3& v, NDVec3 &out) ;

	NDVec2 Get2D() const {return NDVec2(x, y);}

	NDVec3& operator +=(const NDVec3& v){ x += v.x ; y += v.y ; z+= v.z; return *this; }
	NDVec3& operator -=(const NDVec3& v){ x -= v.x ; y -= v.y ; z -= v.z; return *this; }
	NDVec3& operator *=(float s){ x *= s ; y *= s ;z *= s; return *this; };
	NDVec3& operator = (const NDVec3& v){ x = v.x ; y = v.y ;z = v.z; return *this; };
	NDVec3& operator = (const vector3f& v){ x = v.x ; y = v.y ;z= v.z; return *this; };
	NDVec3& operator = (const float *v){ x = v[0] ; y = v[1] ;z= v[2]; return *this; };
	
	void fetch(float *v)const{v[0]=x;v[1]=y; v[2]=z;};

	NDVec3 rotate_axis_degree( const float Rad, const NDVec3& Axis ) const
	{
		const float S	= sin(Rad);//GMath.SinTab(Angle);
		const float C	= cos(Rad);//GMath.CosTab(Angle);
	
		const float XX	= Axis.x * Axis.x;
		const float YY	= Axis.y * Axis.y;
		const float ZZ	= Axis.z * Axis.z;
	
		const float XY	= Axis.x * Axis.y;
		const float YZ	= Axis.y * Axis.z;
		const float ZX	= Axis.z * Axis.x;
	
		const float XS	= Axis.x * S;
		const float YS	= Axis.y * S;
		const float ZS	= Axis.z * S;
	
		const float OMC	= 1.f - C;
	
		return NDVec3(
			(OMC * XX + C ) * x + (OMC * XY - ZS) * y + (OMC * ZX + YS) * z,
			(OMC * XY + ZS) * x + (OMC * YY + C ) * y + (OMC * YZ - XS) * z,
			(OMC * ZX - YS) * x + (OMC * YZ + XS) * y + (OMC * ZZ + C ) * z
			);
	}
};

// NDRect 2D矩形
class NDRect2D : public rect2D
{
public:
    NDRect2D(){}
    NDRect2D(const rect2D& rect);
    //NDRect2D(const NDVec2& vLocation, const NDVec2& vDir, const float width);

public:
    void Build(const NDVec2& vLocation, const NDVec2& vDir, const float width);
    bool PtInRect(const vector2f& pt);
    bool Overlapped(const NDRect2D& rect);
    void Center(vector2f& pt);
    void GetAABB(AABB& rect) const;
    const rect2D& GetRect2D(){ return *this; }

private:
    static bool s_SeparatingAxisCollision(const NDRect2D& r1, const NDRect2D& r2);
    static void s_CalcRectProj(const NDVec2& vSPAxis, const NDRect2D& r, float& fMin, float& fMax);

private:
    void _BuildFromRect2D();

public:
    NDVec2 m_vLocation;
    NDVec2 m_vSide[2];
    float m_fLength[2];
};

NDVec2& RotateAngleCCW(NDVec2& vec, float theta);
NDVec3& RotateAngleCCWV3(NDVec3& vec, float theta);
float RadianbetweenTwoNormalizeVector(const NDVec3& u, const NDVec3& v);
bool IsVectorOrderCW(const NDVec3& u, const NDVec3& v);
bool IsInDistance(const NDVec3& p1, const NDVec3& p2, float squareDistance);
float CalculateRadianInUE3(const NDVec3& eye, const NDVec3& aim);
//NDVec3 RandomRangeByRangeRadius(const NDVec3& eye, unsigned short minRadius, unsigned short maxRadius);

bool IsInDistance2D(vector2f* p1,vector2f* p2, float dist);
//返回[MIN, MAX]
// template<class T>
// T RandomRange(const T& min, const T& max)
// {
// 	if (max <= min)
// 		return max;
// 	
// 	return T(min + (float)rand() * (max - min) / RAND_MAX);
// }

const float PI = 3.141592653f;
const float TWOPI = 2 * PI;
//const unsigned short MAX_NDUINT16 = unsigned short(2<<16) -1;
const unsigned short MAX_NDUINT16 = 0xffff;


// cos sin表, 共360个角度, 用角度做索引
#define COS_SIN_BUF_SIZE 360
extern const vector2f nd_cossin_buff[COS_SIN_BUF_SIZE];


#endif
