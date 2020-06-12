//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CContactDetector.h"
#include <cmath>
#include <cfloat>

#ifndef FLT_MAX
#define FLT_MAX __FLT_MAX__
#endif

/**
* 刚体接触检测
*/
bool CContactDetector::Detect(const CRigidBody* body1, const CRigidBody* body2, SArbiter* arbiter) {
	const SGeometry& g1 = body1->GetGeometry();
	const SGeometry& g2 = body2->GetGeometry();
	const CRigidBody* b1 = body1;
	const CRigidBody* b2 = body2;
	int shp1 = static_cast<int>(g1.shape);
	int shp2 = static_cast<int>(g2.shape);
	if (shp1 > shp2) {
		b1 = body2;
		b2 = body1;
		shp1 = static_cast<int>(g2.shape);
		shp2 = static_cast<int>(g1.shape);
	}
	// 根据不同形状选择不同方法检测，形状定义参见 CGeometryCreator.h
	switch (shp1 * 100 + shp2) {
	case 101: arbiter->numContacts = BoxBoxTest(b1, b2, arbiter->contact); break;
	case 102: arbiter->numContacts = BoxSphereTest(b1, b2, arbiter->contact); break;
	case 107: arbiter->numContacts = BoxPlaneTest(b1, b2, arbiter->contact); break;
	case 202: arbiter->numContacts = SphereSphereTest(b1, b2, arbiter->contact); break;
	case 203: arbiter->numContacts = SphereCapsuleTest(b1, b2, arbiter->contact); break;
	case 204: arbiter->numContacts = SphereCylinderTest(b1, b2, arbiter->contact); break;
	case 205: arbiter->numContacts = SphereTorusTest(b1, b2, arbiter->contact); break;
	case 206: arbiter->numContacts = SphereConeTest(b1, b2, arbiter->contact); break;
	case 207: arbiter->numContacts = SpherePlaneTest(b1, b2, arbiter->contact); break;
	case 303: arbiter->numContacts = CapsuleCapsuleTest(b1, b2, arbiter->contact); break;
	case 307: arbiter->numContacts = CapsulePlaneTest(b1, b2, arbiter->contact); break;
	case 407: arbiter->numContacts = CylinderPlaneTest(b1, b2, arbiter->contact); break;
	case 507: arbiter->numContacts = TorusPlaneTest(b1, b2, arbiter->contact); break;
	case 607: arbiter->numContacts = ConePlaneTest(b1, b2, arbiter->contact); break;
	default: arbiter->numContacts = 0; break;
	}
	// 判断是否需要交换刚体
	if (b1 > b2) {
		arbiter->body1 = const_cast<CRigidBody*>(b2);
		arbiter->body2 = const_cast<CRigidBody*>(b1);
		for (int i = 0; i < arbiter->numContacts; i++) {
			CVector3 temp = arbiter->contact[i].rel1;
			arbiter->contact[i].rel1 = arbiter->contact[i].rel2;
			arbiter->contact[i].rel2 = temp;
			arbiter->contact[i].normal.Scale(-1.0f);
		}
	} else {
		arbiter->body1 = const_cast<CRigidBody*>(b1);
		arbiter->body2 = const_cast<CRigidBody*>(b2);
	}
	return arbiter->numContacts > 0;
}

/**
* 计算盒体与盒体，使用分离轴测试
*/
int CContactDetector::BoxBoxTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	CVector3 distance = pos1 - pos2;
	const SGeometry& geo1 = body1->GetGeometry();
	const SGeometry& geo2 = body2->GetGeometry();
	CVector3 size1(geo1.box.x / 2, geo1.box.y / 2, geo1.box.z / 2);
	CVector3 size2(geo2.box.x / 2, geo2.box.y / 2, geo2.box.z / 2);

	// 两个盒体的旋转缩放平移矩阵
	CMatrix4 R1 = body1->GetSceneNode()->m_cWorldMatrix;
	CMatrix4 R2 = body2->GetSceneNode()->m_cWorldMatrix;
	// 去除缩放和平移分量，转换为正交矩阵
	R1(0, 3) = R1(1, 3) = R1(2, 3) = 0.0f;
	R2(0, 3) = R2(1, 3) = R2(2, 3) = 0.0f;
	for (int i = 0; i < 3; i++) {
		float scale1 = 1.0f / sqrtf(R1(0, i) * R1(0, i) + R1(1, i) * R1(1, i) + R1(2, i) * R1(2, i));
		float scale2 = 1.0f / sqrtf(R2(0, i) * R2(0, i) + R2(1, i) * R2(1, i) + R2(2, i) * R2(2, i));
		R1(0, i) *= scale1;
		R1(1, i) *= scale1;
		R1(2, i) *= scale1;
		R2(0, i) *= scale2;
		R2(1, i) *= scale2;
		R2(2, i) *= scale2;
	}
	// 计算 Box2 相对于 Box1 的旋转矩阵
	CMatrix4 Inv_R1 = R1;
	CMatrix4 R = Inv_R1.Transpose() * R2;
	// 相对于 Box1 的 distance
	CVector3 refDis1 = Inv_R1 * distance;
	// 矩阵 R 中所有分量的绝对值
	float Q11 = fabs(R(0, 0)); float Q12 = fabs(R(0, 1)); float Q13 = fabs(R(0, 2));
	float Q21 = fabs(R(1, 0)); float Q22 = fabs(R(1, 1)); float Q23 = fabs(R(1, 2));
	float Q31 = fabs(R(2, 0)); float Q32 = fabs(R(2, 1)); float Q33 = fabs(R(2, 2));

	// 分离轴测试
	int normalIndex = 0;
	bool invertNormal = false;
	float smallestGap = -FLT_MAX;
	float s, ds;
	// A1 轴
	s = fabs(refDis1[0]) - (size1[0] + size2[0] * Q11 + size2[1] * Q12 + size2[2] * Q13);
	if (s > 0) return 0;
	if (s > smallestGap) {
		smallestGap = s;
		normalIndex = 0;
		invertNormal = refDis1[0] < 0;
	}
	// A2 轴
	s = fabs(refDis1[1]) - (size1[1] + size2[0] * Q21 + size2[1] * Q22 + size2[2] * Q23);
	if (s > 0) return 0;
	if (s > smallestGap) {
		smallestGap = s;
		normalIndex = 1;
		invertNormal = refDis1[1] < 0;
	}
	// A3 轴
	s = fabs(refDis1[2]) - (size1[2] + size2[0] * Q31 + size2[1] * Q32 + size2[2] * Q33);
	if (s > 0) return 0;
	if (s > smallestGap) {
		smallestGap = s;
		normalIndex = 2;
		invertNormal = refDis1[2] < 0;
	}
	// 相对于 Box2 的 distance
	CMatrix4 Inv_R2 = R2;
	CVector3 refDis2 = Inv_R2.Transpose() * distance;
	// B1 轴
	s = fabs(refDis2[0]) - (size2[0] + size1[0] * Q11 + size1[1] * Q21 + size1[2] * Q31);
	if (s > 0) return 0;
	if (s > smallestGap) {
		smallestGap = s;
		normalIndex = 3;
		invertNormal = refDis2[0] < 0;
	}
	// B2 轴
	s = fabs(refDis2[1]) - (size2[1] + size1[0] * Q12 + size1[1] * Q22 + size1[2] * Q32);
	if (s > 0) return 0;
	if (s > smallestGap) {
		smallestGap = s;
		normalIndex = 4;
		invertNormal = refDis2[1] < 0;
	}
	// B3 轴
	s = fabs(refDis2[2]) - (size2[2] + size1[0] * Q13 + size1[1] * Q23 + size1[2] * Q33);
	if (s > 0) return 0;
	if (s > smallestGap) {
		smallestGap = s;
		normalIndex = 5;
		invertNormal = refDis2[2] < 0;
	}

	// A1 x B1 轴（0,-R31,R21）
	ds = refDis1[2] * R(1, 0) - refDis1[1] * R(2, 0);
	s = fabs(ds) - (size1[1] * Q31 + size1[2] * Q21 + size2[1] * Q13 + size2[2] * Q12);
	if (s > 0) return 0;
	float n = sqrtf(Q31 * Q31 + Q21 * Q21);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 6;
			invertNormal = ds < 0;
		}
	}
	// A1 x B2 轴（0,-R32,R22）
	ds = refDis1[2] * R(1, 1) - refDis1[1] * R(2, 1);
	s = fabs(ds) - (size1[1] * Q32 + size1[2] * Q22 + size2[0] * Q13 + size2[2] * Q11);
	if (s > 0) return 0;
	n = sqrtf(Q32 * Q32 + Q22 * Q22);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 7;
			invertNormal = ds < 0;
		}
	}
	// A1 x B3 轴（0,-R33,R23）
	ds = refDis1[2] * R(1, 2) - refDis1[1] * R(2, 2);
	s = fabs(ds) - (size1[1] * Q33 + size1[2] * Q23 + size2[0] * Q12 + size2[1] * Q11);
	if (s > 0) return 0;
	n = sqrtf(Q33 * Q33 + Q23 * Q23);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 8;
			invertNormal = ds < 0;
		}
	}
	// A2 x B1 轴（R31,0,-R11）
	ds = refDis1[0] * R(2, 0) - refDis1[2] * R(0, 0);
	s = fabs(ds) - (size1[0] * Q31 + size1[2] * Q11 + size2[1] * Q23 + size2[2] * Q22);
	if (s > 0) return 0;
	n = sqrtf(Q31 * Q31 + Q11 * Q11);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 9;
			invertNormal = ds < 0;
		}
	}
	// A2 x B2 轴（R32,0,-R12）
	ds = refDis1[0] * R(2, 1) - refDis1[2] * R(0, 1);
	s = fabs(ds) - (size1[0] * Q32 + size1[2] * Q12 + size2[0] * Q23 + size2[2] * Q21);
	if (s > 0) return 0;
	n = sqrtf(Q32 * Q32 + Q12 * Q12);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 10;
			invertNormal = ds < 0;
		}
	}
	// A2 x B3 轴（R33,0,-R13）
	ds = refDis1[0] * R(2, 2) - refDis1[2] * R(0, 2);
	s = fabs(ds) - (size1[0] * Q33 + size1[2] * Q13 + size2[0] * Q22 + size2[1] * Q21);
	if (s > 0) return 0;
	n = sqrtf(Q33 * Q33 + Q13 * Q13);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 11;
			invertNormal = ds < 0;
		}
	}
	// A3 x B1 轴（-R21,R11,0）
	ds = refDis1[1] * R(0, 0) - refDis1[0] * R(1, 0);
	s = fabs(ds) - (size1[0] * Q21 + size1[1] * Q11 + size2[1] * Q33 + size2[2] * Q32);
	if (s > 0) return 0;
	n = sqrtf(Q21 * Q21 + Q11 * Q11);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 12;
			invertNormal = ds < 0;
		}
	}
	// A3 x B2 轴（-R22,R12,0）
	ds = refDis1[1] * R(0, 1) - refDis1[0] * R(1, 1);
	s = fabs(ds) - (size1[0] * Q22 + size1[1] * Q12 + size2[0] * Q33 + size2[2] * Q31);
	if (s > 0) return 0;
	n = sqrtf(Q22 * Q22 + Q12 * Q12);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 13;
			invertNormal = ds < 0;
		}
	}
	// A3 x B3 轴（-R23,R13,0）
	ds = refDis1[1] * R(0, 2) - refDis1[0] * R(1, 2);
	s = fabs(ds) - (size1[0] * Q23 + size1[1] * Q13 + size2[0] * Q32 + size2[1] * Q31);
	if (s > 0) return 0;
	n = sqrtf(Q23 * Q23 + Q13 * Q13);
	if (n > 0) {
		s /= n;
		if (s > smallestGap) {
			smallestGap = s;
			normalIndex = 14;
			invertNormal = ds < 0;
		}
	}

	// 计算碰撞法向量
	CVector3 norm_world;
	switch (normalIndex) {
	case 0:
	case 1:
	case 2:
		norm_world[0] = R1(0, normalIndex);
		norm_world[1] = R1(1, normalIndex);
		norm_world[2] = R1(2, normalIndex);
		break;
	case 3:
	case 4:
	case 5:
		norm_world[0] = R2(0, normalIndex - 3);
		norm_world[1] = R2(1, normalIndex - 3);
		norm_world[2] = R2(2, normalIndex - 3);
		break;
	case 6: norm_world = R1 * CVector3(0, -R(2, 0), R(1, 0)); break;
	case 7: norm_world = R1 * CVector3(0, -R(2, 1), R(1, 1)); break;
	case 8: norm_world = R1 * CVector3(0, -R(2, 2), R(1, 2)); break;
	case 9: norm_world = R1 * CVector3(R(2, 0), 0, -R(0, 0)); break;
	case 10: norm_world = R1 * CVector3(R(2, 1), 0, -R(0, 1)); break;
	case 11: norm_world = R1 * CVector3(R(2, 2), 0, -R(0, 2)); break;
	case 12: norm_world = R1 * CVector3(-R(1, 0), R(0, 0), 0); break;
	case 13: norm_world = R1 * CVector3(-R(1, 1), R(0, 1), 0); break;
	case 14: norm_world = R1 * CVector3(-R(1, 2), R(0, 2), 0); break;
	}
	if (invertNormal) norm_world.Scale(-1.0f);

	// 边-边接触
	if (normalIndex > 5) {
		// 找到在 Box1 相交边上的一点 pa
		CVector3 norm1 = Inv_R1 * norm_world;
		for (int i = 0; i < 3; i++) norm1[i] = norm1[i] < 0? size1[i]: -size1[i];
		CVector3 pa = pos1 + (R1 * norm1);
		// 找到在 Box2 相交边上的一点 pb
		CVector3 norm2 = Inv_R2 * norm_world;
		for (int i = 0; i < 3; i++) norm2[i] = norm2[i] > 0? size2[i]: -size2[i];
		CVector3 pb = pos2 + (R2 * norm2);
		// Box1 和 Box2 上相交边的方向向量
		int axis1 = (normalIndex - 6) / 3;
		int axis2 = (normalIndex - 6) % 3;
		CVector3 ua(R1(0, axis1), R1(1, axis1), R1(2, axis1), 0);
		CVector3 ub(R2(0, axis2), R2(1, axis2), R2(2, axis2), 0);

		// 计算两条边的最近点
		CVector3 uab = ua.CrossProduct(ub);
		float d = uab[0] * uab[0] + uab[1] * uab[1] + uab[2] * uab[2];
		if (d > 0.000001f) {
			d = 1.0f / d;
			CVector3 p = pb - pa;
			float t1 = uab.DotProduct(p.CrossProduct(ub)) * d;
			float t2 = uab.DotProduct(p.CrossProduct(ua)) * d;
			pa += ua * t1;
			pb += ub * t2;
		}
        // 取两个点的中点作为交点
		CVector3 contactPoint = (pa + pb) * 0.5f;
		contact[0].point = contactPoint;
		contact[0].normal = norm_world;
		contact[0].rel1 = contactPoint - pos1;
		contact[0].rel2 = contactPoint - pos2;
		contact[0].penetration = smallestGap;
        return 1;
	}

	// 面-点接触
	const float *Rb, *Sa, *Sb;
	CVector3 center;
	CVector3 normLocal;

	// 计算非参考盒体局部坐标系内的接触法向量
	if (normalIndex < 3) {
		Rb = R2.m_fValue;
		Sa = size1.m_fValue;
		Sb = size2.m_fValue;
		normLocal = Inv_R2 * norm_world;
		center = pos2 - pos1;
	} else {
		Rb = R1.m_fValue;
		Sa = size2.m_fValue;
		Sb = size1.m_fValue;
		normLocal = Inv_R1 * norm_world;
		normLocal.Scale(-1.0f);
		center = pos1 - pos2;
	}
	// 计算该法向量的最大分量方向
	int lanr;
	if (fabs(normLocal[0]) > fabs(normLocal[1])) {
		if (fabs(normLocal[0]) > fabs(normLocal[2])) lanr = 0;
		else lanr = 2;
	} else {
		if (fabs(normLocal[1]) > fabs(normLocal[2])) lanr = 1;
		else lanr = 2;
	}
	// 计算相对于参考面的投影面中心点
	if (normLocal[lanr] < 0) center -= CVector3(Rb[lanr * 4], Rb[lanr * 4 + 1], Rb[lanr * 4 + 2]) * Sb[lanr];
	else center += CVector3(Rb[lanr * 4], Rb[lanr * 4 + 1], Rb[lanr * 4 + 2]) * Sb[lanr];

	// 参考面的坐标轴分量下标 ix, iy 区别于分离面法向量
	int ix = (normalIndex % 3 + 1) % 3;
	int iy = (normalIndex % 3 + 2) % 3;
	int iz = normalIndex % 3;
	// 投影面的坐标轴分量下标 jx, jy 区别于投影面法向量
	int jx = (lanr + 1) % 3;
	int jy = (lanr + 2) % 3;

	// 计算投影到参考面的四个点的坐标
	CMatrix4& matProj = normalIndex < 3? Inv_R1: Inv_R2;
	CVector3 ao = matProj * center;
	CVector3 ax = matProj * (CVector3(Rb[jx * 4], Rb[jx * 4 + 1], Rb[jx * 4 + 2]) * Sb[jx]);
	CVector3 ay = matProj * (CVector3(Rb[jy * 4], Rb[jy * 4 + 1], Rb[jy * 4 + 2]) * Sb[jy]);
	CVector3 proj[4] = { ao + ax + ay, ao + ax - ay, ao - ax - ay, ao - ax + ay };

	// 求接触点世界坐标的变换矩阵和偏移
	CMatrix4& matUnProj = normalIndex < 3? R1: R2;
	CVector3& offset = normalIndex < 3? pos1: pos2;

	// 对投影四边形与参考面（矩形）进行相交测试
	float rect[2] = { Sa[ix], Sa[iy] };
	float point[24] = {
		proj[0][ix], proj[0][iy], proj[0][iz],
		proj[1][ix], proj[1][iy], proj[1][iz],
		proj[2][ix], proj[2][iy], proj[2][iz],
		proj[3][ix], proj[3][iy], proj[3][iz]
	};
	int numPoint = 4;
	float buffer[24];

	// 将四边形顶点作为初始点与矩形四条边测试，每次使用一条矩形边测试
	// 若一个顶点在矩形边的内侧则将该点加入，等待后续测试
	// 若一个顶点与相邻的下一个顶点中只有一个在矩形边的内侧，则计算交点并加入，等待后续测试
	float* pp = point;
	float* pb = buffer;
	for (int dir = 0; dir < 2; dir++) {
		for (int sign=0; sign<2; sign++) {
			float* pSrc = pp;
			float* pDst = pb;
			int count = 0;
			for (int i = 0; i < numPoint; i++) {
				bool inside = (sign? pSrc[dir]: -pSrc[dir]) < rect[dir];
				if (inside) {
					pDst[0] = pSrc[0];
					pDst[1] = pSrc[1];
					pDst[2] = pSrc[2];
					pDst += 3;
					count++;
				}
				float* pSrcNext = pp + ((i + 1) % numPoint) * 3;
				if (inside ^ ((sign? pSrcNext[dir]: -pSrcNext[dir]) < rect[dir])) {
					// 计算两点 pSrc, pSrcNext 组成的线段与边 sign rect[dir] 的交点
					float k = ((sign? rect[dir]: -rect[dir]) - pSrc[dir]) / (pSrcNext[dir] - pSrc[dir]);
					const int dstX = dir;
					const int dstY = 1 - dir;
					const int dstZ = 2;
					pDst[dstX] = sign? rect[dstX]: -rect[dstX];
					pDst[dstY] = pSrc[dstY] + (pSrcNext[dstY] - pSrc[dstY]) * k;
					pDst[dstZ] = pSrc[dstZ] + (pSrcNext[dstZ] - pSrc[dstZ]) * k;
					pDst += 3;
					count++;
				}
				// 下一个顶点
				pSrc += 3;
			}
			numPoint = count;
			pp = pb;
			pb = (pp == buffer)? point: buffer;
		}
	}

	// 对计算出来的接触点进行深度比较，只返回有接触深度的点，注意接触法向量是指向物体 A 的
	int penetrate = 0;
	const bool compare = normalIndex < 3 ? invertNormal : !invertNormal;
	const float reference = compare ? Sa[iz] : -Sa[iz];
	for (int i = 0; i < numPoint; i++) {
		const float* pt = pp + i * 3;
		float depth = compare ? pt[2] - reference: reference - pt[2];
		if (depth <= 0.0f) {
			float contactPoint[3];
			contactPoint[ix] = pt[0];
			contactPoint[iy] = pt[1];
			contactPoint[iz] = pt[2];
			// 转换接触点到世界坐标系
			contact[penetrate].point = matUnProj * CVector3(contactPoint) + offset;
			contact[penetrate].normal = norm_world;
			contact[penetrate].rel1 = contact[penetrate].point - pos1;
			contact[penetrate].rel2 = contact[penetrate].point - pos2;
			contact[penetrate].penetration = depth;
			penetrate++;
		}
	}
	return penetrate;
}

/**
* 计算盒体与球体
*/
int CContactDetector::BoxSphereTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	CVector3 distance = pos2 - pos1;
	const SGeometry& geo1 = body1->GetGeometry();
	const SGeometry& geo2 = body2->GetGeometry();
	CVector3 size(geo1.box.x * 0.5f, geo1.box.y * 0.5f, geo1.box.z * 0.5f);
	float radius = geo2.sphere.r;

	// 盒体三个轴向单位向量
	CMatrix4& m = body1->GetSceneNode()->m_cWorldMatrix;
	CVector3 axis[3] = {
		CVector3(m(0, 0), m(1, 0), m(2, 0)).Normalize(),
		CVector3(m(0, 1), m(1, 1), m(2, 1)).Normalize(),
		CVector3(m(0, 2), m(1, 2), m(2, 2)).Normalize()
	};
	// 计算盒体上面距离球心的最近点
	CVector3 closest = pos1;
	for (int i = 0; i < 3; i++) {
		float len = distance.DotProduct(axis[i]);
		if (len > size[i]) len = size[i];
		else if (len < -size[i]) len = -size[i];
		closest += axis[i] * len;
	}
	// 检查是否碰撞
	distance = closest - pos2;
	float len = distance.DotProduct(distance);
	if (len >= radius * radius) return 0;
	if (len > 0.0f) {
		len = sqrtf(len);
		distance.Scale(1.0f / len);
	} else {
		// 球心与接触点重合情况
		distance = (pos1 - pos2).Normalize();
	}
	contact[0].point = closest;
	contact[0].normal = distance;
	contact[0].rel1 = closest - pos1;
	contact[0].rel2 = closest - pos2;
	contact[0].penetration = len - radius;
	return 1;
}

/**
* 计算盒体与平面
*/
int CContactDetector::BoxPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();

	// 盒体三个轴向与平面法向的单位向量
	CMatrix4& m1 = body1->GetSceneNode()->m_cWorldMatrix;
	CMatrix4& m2 = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 axis[4] = {
		CVector3(m1(0, 0), m1(1, 0), m1(2, 0)).Normalize(),
		CVector3(m1(0, 1), m1(1, 1), m1(2, 1)).Normalize(),
		CVector3(m1(0, 2), m1(1, 2), m1(2, 2)).Normalize(),
		CVector3(m2(0, 2), m2(1, 2), m2(2, 2)).Normalize()
	};
	// 获取盒体主轴(与平面法向最接近)
	float dot[3] = {
		axis[0].DotProduct(axis[3]),
		axis[1].DotProduct(axis[3]),
		axis[2].DotProduct(axis[3])
	};
	int dominate;
	const float fx = fabs(dot[0]);
	const float fy = fabs(dot[1]);
	const float fz = fabs(dot[2]);
	if (fx > fy && fx > fz) dominate = 0;
	else if (fy > fx && fy > fz) dominate = 1;
	else dominate = 2;
	// 计算接触面上的四个点
	axis[0].Scale(body1->GetGeometry().box.x * 0.5f);
	axis[1].Scale(body1->GetGeometry().box.y * 0.5f);
	axis[2].Scale(body1->GetGeometry().box.z * 0.5f);
	if (dot[dominate] > 0.0f) axis[dominate].Scale(-1.0f);
	CVector3 center = pos1 + axis[dominate];
	CVector3 point[4] = {
		center + axis[(dominate + 1) % 3] + axis[(dominate + 2) % 3],
		center + axis[(dominate + 1) % 3] - axis[(dominate + 2) % 3],
		center - axis[(dominate + 1) % 3] + axis[(dominate + 2) % 3],
		center - axis[(dominate + 1) % 3] - axis[(dominate + 2) % 3]
	};
	// 计算与平面的交点
	CPlane plane(pos2, axis[3]);
	int count = 0;
	for (int i = 0; i < 4; i++) {
		float len = plane.Distance(point[i]);
		if (len < 0.0f) {
			contact[count].point = point[i];
			contact[count].normal = axis[3];
			contact[count].rel1 = contact[count].point - pos1;
			contact[count].rel2 = contact[count].point - pos2;
			contact[count].penetration = len;
			count++;
		}
	}
	return count;
}

/**
* 计算球体与球体
*/
int CContactDetector::SphereSphereTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	CVector3 distance = pos1 - pos2;
	float radius1 = body1->GetGeometry().sphere.r;
	float radius2 = body2->GetGeometry().sphere.r;

	// 计算两个球体之间的距离平方
	float len = distance.DotProduct(distance);
	float rad = radius1 + radius2;
	if (len > 0 && len < rad * rad) {
		len = sqrtf(len);
		distance.Scale(1.0f / len);
		contact[0].point = pos2 + distance * radius2;
		contact[0].normal = distance;
		contact[0].rel1 = contact[0].point - pos1;
		contact[0].rel2 = contact[0].point - pos2;
		contact[0].penetration = len - rad;
		return 1;
	}
	return 0;
}

/**
* 计算球体与胶囊体
*/
int CContactDetector::SphereCapsuleTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	float radius = body1->GetGeometry().sphere.r;
	float capsuleRadius = body2->GetGeometry().capsule.r;
	float capsuleHeight = body2->GetGeometry().capsule.h;

	// 胶囊体 Z 轴方向的单位向量
	CMatrix4& m = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz = CVector3(m(0, 2), m(1, 2), m(2, 2)).Normalize();

	// 将投影位置限制到两个端点之间
	const float halfHeight = capsuleHeight * 0.5f;
	float t = nz.DotProduct(pos1);
	if (t > halfHeight) t = halfHeight;
	else if (t < -halfHeight) t = -halfHeight;
	CVector3 projectPoint = nz * t + pos2;

	const float minimum = radius + capsuleRadius;
	CVector3 distance = pos1 - projectPoint;
	float len = distance.DotProduct(distance);
	if (len < minimum * minimum) {
		CVector3 point = (projectPoint + pos1) * 0.5f;
		len = sqrtf(len);
		distance.Scale(1.0f / len);
		contact[0].point = point;
		contact[0].normal = distance;
		contact[0].rel1 = point - pos1;
		contact[0].rel2 = point - pos2;
		contact[0].penetration = len - minimum;
		return 1;
	}
	return 0;
}

/**
* 计算球体与圆柱体
*/
int CContactDetector::SphereCylinderTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	CVector3 distance = pos1 - pos2;
	float radius = body1->GetGeometry().sphere.r;
	float cylinderHeight = body2->GetGeometry().cylinder.h * 0.5f;
	float cylinderRadius = body2->GetGeometry().cylinder.r;

	// 圆柱体 Z 轴方向的单位向量
	CMatrix4& m = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz = CVector3(m(0, 2), m(1, 2), m(2, 2)).Normalize();
	// 计算圆柱体轴线到碰撞点的向量 r
	float lenz = distance.DotProduct(nz);
	CVector3 r = distance - (nz * lenz);

	// 如果球心在圆柱体两端，则检查球心距离圆柱体轴线的距离是否小于圆柱体半径
	if (lenz > cylinderHeight || lenz < -cylinderHeight) {
		float lenr = r.Length();
		if (lenr > cylinderRadius) r.Scale(cylinderRadius / lenr);
	} else {
		r.Normalize();
		r.Scale(cylinderRadius);
	}
	// 最近点沿圆柱体轴向方向的偏移
	if (lenz > cylinderHeight) lenz = cylinderHeight;
	else if (lenz < -cylinderHeight) lenz = -cylinderHeight;

	// 计算距离球体圆柱体上面的最近点
	CVector3 closest = pos2 + r + nz.Scale(lenz);
	// 检查是否碰撞
	distance = pos1 - closest;
	float len = distance.DotProduct(distance);
	if (len < radius * radius) {
		len = sqrtf(len);
		distance.Scale(1.0f / len);
		contact[0].point = closest;
		contact[0].normal = distance;
		contact[0].rel1 = closest - pos1;
		contact[0].rel2 = closest - pos2;
		contact[0].penetration = len - radius;
		return 1;
	}
	return 0;
}

/**
* 计算球体与圆环
*/
int CContactDetector::SphereTorusTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	CVector3 distance = pos1 - pos2;
	float radius = body1->GetGeometry().sphere.r;
	float torusR = body2->GetGeometry().torus.r;
	float torusC = body2->GetGeometry().torus.c;

	// 圆环 Z 轴向单位向量
	CMatrix4& m = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz = CVector3(m(0, 2), m(1, 2), m(2, 2)).Normalize();

	// 计算圆环截圆到球体球心的向量
	CVector3 xoy = distance - nz * distance.DotProduct(nz);
	CVector3 tempCenter = xoy.Normalize() * torusR;
	distance -= tempCenter;

	// 比较平方距离，检查是否发生接触
	float len = distance.DotProduct(distance);
	float rad = radius + torusC;
	if (len > 0 && len < rad * rad) {
		len = sqrtf(len);
		distance.Scale(1.0f / len);
		contact[0].point = tempCenter + pos2 + distance * torusC;
		contact[0].normal = distance;
		contact[0].rel1 = contact[0].point - pos1;
		contact[0].rel2 = contact[0].point - pos2;
		contact[0].penetration = len - rad;
		return 1;
	}
	return 0;
}

/**
* 计算球体与圆锥体
*/
int CContactDetector::SphereConeTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	CVector3 distance = pos1 - pos2;
	float radius = body1->GetGeometry().sphere.r;
	float coneHeight = body2->GetGeometry().cone.h;
	float coneRadius = body2->GetGeometry().cone.r;
	const float coneHeightTop = 0.75f * coneHeight;
	const float coneHeightBottom = -0.25f * coneHeight;
	// 圆锥体 Z 轴方向的单位向量
	CMatrix4& m = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz = CVector3(m(0, 2), m(1, 2), m(2, 2)).Normalize();
	CVector3 closest = pos2;
	// 如果球心在圆锥体的底面之下
	float lenz = distance.DotProduct(nz);
	CVector3 r = distance - (nz * lenz);
	float lenr = r.Length();
	if (lenz < coneHeightBottom) {
		if (lenr > coneRadius) r.Scale(coneRadius / lenr);
		closest += nz.Scale(coneHeightBottom) + r;
	} else {
		// 球心在圆锥体顶端
		if (lenr == 0.0f) {
			if (lenz > coneHeightTop) lenz = coneHeightTop;
			closest += nz.Scale(lenz);
		} else {
			float theta = atan2f(coneRadius, coneHeight);
			float r1 = coneHeightTop * sinf(theta);
			float ht = coneHeightTop * cosf(theta);
			float hb = ht - coneRadius / sinf(theta);
			CVector3 nz1 = (nz.Scale(coneHeight) - r.Scale(coneRadius / lenr)).Normalize();
			float lenz1 = distance.DotProduct(nz1);
			CVector3 nr1 = (distance - (nz1 * lenz1)).Normalize();
			if (lenz1 > ht) lenz1 = ht;
			else if (lenz1 < hb) lenz1 = hb;
			closest += nz1.Scale(lenz1) + nr1.Scale(r1);
		}
	}
	// 计算距离球体圆柱体上面的最近点检查是否碰撞
	distance = pos1 - closest;
	float len = distance.DotProduct(distance);
	if (len < radius * radius) {
		len = sqrtf(len);
		distance.Scale(1.0f / len);
		contact[0].point = closest;
		contact[0].normal = distance;
		contact[0].rel1 = closest - pos1;
		contact[0].rel2 = closest - pos2;
		contact[0].penetration = len - radius;
		return 1;
	}
	return 0;
}

/**
* 计算球体与平面
*/
int CContactDetector::SpherePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	CVector3 distance = pos1 - pos2;
	float radius = body1->GetGeometry().sphere.r;

	// 平面法向的单位向量
	CMatrix4& m = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz = CVector3(m(0, 2), m(1, 2), m(2, 2)).Normalize();
	// 计算球体到平面的距离
	float len = distance.DotProduct(nz);
	if (len < radius) {
		contact[0].point = pos1 - nz * radius;
		contact[0].normal = nz;
		contact[0].rel1 = contact[0].point - pos1;
		contact[0].rel2 = contact[0].point - pos2;
		contact[0].penetration = len - radius;
		return 1;
	}
	return 0;
}

/**
* 计算胶囊体与胶囊体
*/
int CContactDetector::CapsuleCapsuleTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	float radius1 = body1->GetGeometry().capsule.r;
	float radius2 = body2->GetGeometry().capsule.r;
	float height1 = body1->GetGeometry().capsule.h;
	float height2 = body2->GetGeometry().capsule.h;
	// 胶囊体 Z 轴方向的向量
	CMatrix4& m1 = body1->GetSceneNode()->m_cWorldMatrix;
	CMatrix4& m2 = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz1 = CVector3(m1(0, 2), m1(1, 2), m1(2, 2)).Normalize().Scale(height1);
	CVector3 nz2 = CVector3(m2(0, 2), m2(1, 2), m2(2, 2)).Normalize().Scale(height2);

	// 计算由代表胶囊体的两线段之间的最近点
	CVector3 bottomPoint1 = pos1 - nz1 * 0.5f;
	CVector3 bottomPoint2 = pos2 - nz2 * 0.5f;
	CVector3 distance = bottomPoint1 - bottomPoint2;
	const float a = nz1.DotProduct(nz1);
	const float b = nz1.DotProduct(nz2);
	const float c = nz2.DotProduct(nz2);
	const float d = nz1.DotProduct(distance);
	const float e = nz2.DotProduct(distance);
	float dt = a * c - b * b;
	// 直线参数 s(0, 1) 且 t(0, 1)
	float s = 0.0f;
	float t = 0.0f;
	if (dt > 1E-6) {
		s = b * e - c * d;
		if (s < 0.0f) s = 0.0f;
		else if (s > dt) s = 1.0f;
		else s = s / dt;
	}
	t = b * s + e;
	if (t < 0.0f) {
		t = 0.0f;
		s = -d;
		if (s < 0.0f) s = 0.0f;
		else if (s > a) s = 1.0f;
		else s = s / a;
	} else if (t > c) {
		t = 1.0f;
		s = b - d;
		if (s < 0.0f) s = 0.0f;
		else if (s > a) s = 1.0f;
		else s = s / a;
	} else {
		t = t / c;
	}
	// 得到线段上两个最近点
	CVector3 p1 = bottomPoint1 + nz1 * s;
	CVector3 p2 = bottomPoint2 + nz2 * t;
	distance = p1 - p2;
	const float minimum = radius1 + radius2;
	float len = distance.DotProduct(distance);
	if (len < minimum * minimum) {
		len = sqrtf(len);
		CVector3 point = distance * (radius2 / len) + p2;
		distance.Scale(1.0f / len);
		contact[0].point = point;
		contact[0].normal = distance;
		contact[0].rel1 = point - pos1;
		contact[0].rel2 = point - pos2;
		contact[0].penetration = len - minimum;
		return 1;
	}
	return 0;
}

/**
* 计算胶囊体与平面
*/
int CContactDetector::CapsulePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	float radius = body1->GetGeometry().capsule.r;
	float height = body1->GetGeometry().capsule.h * 0.5f;

	// 胶囊体与平面法向的单位向量
	CMatrix4& m1 = body1->GetSceneNode()->m_cWorldMatrix;
	CMatrix4& m2 = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz1 = CVector3(m1(0, 2), m1(1, 2), m1(2, 2)).Normalize();
	CVector3 nz2 = CVector3(m2(0, 2), m2(1, 2), m2(2, 2)).Normalize();

	// 计算胶囊体两个端点
	nz1.Scale(height);
	CVector3 point1 = pos1 + nz1;
	CVector3 point2 = pos1 - nz1;
	// 计算与平面的交点
	CPlane plane(pos2, nz2);
	float len1 = plane.Distance(point1);
	float len2 = plane.Distance(point2);
	int count = 0;
	if (len1 < radius) {
		contact[count].point = point1 - nz2 * radius;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len1 - radius;
		count++;
	}
	if (len2 < radius) {
		contact[count].point = point2 - nz2 * radius;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len2 - radius;
		count++;
	}
	return count;
}

/**
* 计算圆柱体与平面
*/
int CContactDetector::CylinderPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	float radius = body1->GetGeometry().cylinder.r;
	float height = body1->GetGeometry().cylinder.h * 0.5f;

	// 圆柱体与平面法向的单位向量
	CMatrix4& m1 = body1->GetSceneNode()->m_cWorldMatrix;
	CMatrix4& m2 = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz1 = CVector3(m1(0, 2), m1(1, 2), m1(2, 2)).Normalize();
	CVector3 nz2 = CVector3(m2(0, 2), m2(1, 2), m2(2, 2)).Normalize();
	// 计算圆柱近侧面在平面上可表示的线段
	CVector3 tangent = nz1.CrossProduct(nz2);
	if (tangent.Length() == 0.0f) tangent.SetValue(nz1.Tangent());
	CVector3 lineCenter = pos1 + nz1.CrossProduct(tangent).Normalize().Scale(radius);
	nz1.Scale(height);
	CVector3 point1 = lineCenter + nz1;
	CVector3 point2 = lineCenter - nz1;
	// 计算与平面的交点
	CPlane plane(pos2, nz2);
	float len1 = plane.Distance(point1);
	float len2 = plane.Distance(point2);
	int count = 0;
	if (len1 < 0.0f) {
		contact[count].point = point1;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len1;
		count++;
	}
	if (len2 < 0.0f) {
		contact[count].point = point2;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len2;
		count++;
	}
	return count;
}

/**
* 计算圆环与平面
*/
int CContactDetector::TorusPlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	float torusR = body1->GetGeometry().torus.r;
	float torusC = body1->GetGeometry().torus.c;

	// 圆环与平面法向的单位向量
	CMatrix4& m1 = body1->GetSceneNode()->m_cWorldMatrix;
	CMatrix4& m2 = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz1 = CVector3(m1(0, 2), m1(1, 2), m1(2, 2)).Normalize();
	CVector3 nz2 = CVector3(m2(0, 2), m2(1, 2), m2(2, 2)).Normalize();
	// 计算圆环环面在平面上可表示的线段
	CVector3 tangent = nz2.CrossProduct(nz1);
	if (tangent.Length() == 0.0f) tangent.SetValue(nz1.Tangent());
	CVector3 lineVec = nz1.CrossProduct(tangent).Normalize().Scale(torusR);
	CVector3 point1 = pos1 + lineVec;
	CVector3 point2 = pos1 - lineVec;
	// 计算与平面的交点
	CPlane plane(pos2, nz2);
	float len1 = plane.Distance(point1);
	float len2 = plane.Distance(point2);
	int count = 0;
	if (len1 < torusC) {
		contact[count].point = point1 - nz2 * torusC;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len1 - torusC;
		count++;
	}
	if (len2 < torusC) {
		contact[count].point = point2 - nz2 * torusC;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len2 - torusC;
		count++;
	}
	return count;
}

/**
* 计算圆锥体与平面
*/
int CContactDetector::ConePlaneTest(const CRigidBody* body1, const CRigidBody* body2, SContact* contact) {
	CVector3 pos1 = body1->GetSceneNode()->GetWorldPosition();
	CVector3 pos2 = body2->GetSceneNode()->GetWorldPosition();
	float radius = body1->GetGeometry().cylinder.r;
	float height = body1->GetGeometry().cylinder.h * 0.5f;

	// 圆锥体与平面法向的单位向量
	CMatrix4& m1 = body1->GetSceneNode()->m_cWorldMatrix;
	CMatrix4& m2 = body2->GetSceneNode()->m_cWorldMatrix;
	CVector3 nz1 = CVector3(m1(0, 2), m1(1, 2), m1(2, 2)).Normalize();
	CVector3 nz2 = CVector3(m2(0, 2), m2(1, 2), m2(2, 2)).Normalize();
	// 计算圆锥体底面在平面上可表示的线段
	CVector3 tangent = nz2.CrossProduct(nz1);
	if (tangent.Length() == 0.0f) tangent.SetValue(nz1.Tangent());
	CVector3 lineVec = nz1.CrossProduct(tangent).Normalize().Scale(radius);
	CVector3 point0 = pos1 + (nz1 * 0.75f);
	CVector3 point1 = pos1 - (nz1 * 0.25f) + lineVec;
	CVector3 point2 = pos1 - (nz1 * 0.25f) - lineVec;
	// 计算锥顶及线段两端与平面的交点
	CPlane plane(pos2, nz2);
	float len0 = plane.Distance(point0);
	float len1 = plane.Distance(point1);
	float len2 = plane.Distance(point2);
	int count = 0;
	if (len0 < 0.0f) {
		contact[count].point = point0;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len0;
		count++;
	}
	if (len1 < 0.0f) {
		contact[count].point = point1;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len1;
		count++;
	}
	if (len2 < 0.0f) {
		contact[count].point = point2;
		contact[count].normal = nz2;
		contact[count].rel1 = contact[count].point - pos1;
		contact[count].rel2 = contact[count].point - pos2;
		contact[count].penetration = len2;
		count++;
	}
	return count;
}