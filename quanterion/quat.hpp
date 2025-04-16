#pragma once

#include <algorithm>
#include <cmath>

template< typename T >
struct matrix_t
{
	T data[16];
};

template< typename T >
struct vector3_t
{
	T x, y, z;
};

template< typename T >
class Quat
{
  public:
	const T *data() const { return m_value; }

	Quat() : Quat(0, 0, 0, 0) {}

	Quat(T a, T b, T c, T d) : m_value{ b, c, d, a } {}

	Quat(T a, bool is, vector3_t< T > vec)
	{
		if (!is)
			a *= M_PI / 180;

		m_value[3] = 0;
		m_value[0] = vec.x;
		m_value[1] = vec.y;
		m_value[2] = vec.z;
		T norm = norma(*this);
		if (norm == 0)
			norm = 1;

		m_value[3] = std::cos(a / 2);
		T s = std::sin(a / 2);
		for (int i = 0; i < 4 - 1; i++)
			m_value[i] = (m_value[i] / norm) * s;
	}

	Quat< T > operator-(Quat< T > value) const
	{
		Quat< T > newVal;
		return getAddOrSub(&value, &newVal, -1);
	}

	Quat< T > operator+(Quat< T > value) const
	{
		Quat< T > newVal;
		return getAddOrSub(&value, &newVal, 1);
	}

	Quat< T > &operator+=(Quat< T > const value)
	{
		getAddOrSub(&value, this, 1);
		return *this;
	}

	Quat< T > &operator-=(Quat< T > value)
	{
		getAddOrSub(&value, this, -1);
		return *this;
	}

	Quat< T > operator*(Quat< T > value) const
	{
		Quat< T > newQ;
		for (short i = 0; i < 4; i++)
			newQ.m_value[i] = m_value[i];
		newQ = value.scalarVec(m_value[3]) + newQ.scalarVec(value.m_value[3]);
		newQ = vecMul(&newQ, &value);
		newQ.m_value[3] =
			m_value[3] * value.m_value[3] -
			((m_value[0] * value.m_value[0]) + (m_value[1] * value.m_value[1]) + (m_value[2] * value.m_value[2]));
		return newQ;
	}

	Quat< T > operator*(T scalar) const
	{
		Quat< T > newQ;
		for (short i = 0; i < 4; i++)
			newQ.m_value[i] = m_value[i] * scalar;
		return newQ;
	}

	Quat< T > operator*(vector3_t< T > value) const
	{
		Quat< T > q(0, value.x, value.y, value.z);
		q = *this * q;
		return q;
	}

	Quat< T > operator~() const { return Quat< T >(m_value[3], -m_value[0], -m_value[1], -m_value[2]); }

	bool operator==(Quat< T > const value) const { return std::equal(m_value, m_value + 4, value.m_value); }

	bool operator!=(Quat< T > const value) const { return !(*this == value); }
	matrix_t< T > rotation_matrix() const
	{
		Quat< T > nq = *this;
		toNormalize(&nq);
		T *temp = nq.m_value;
		T x_2 = temp[0] * temp[0];
		T y_2 = temp[1] * temp[1];
		T z_2 = temp[2] * temp[2];

		T xy = temp[0] * temp[1];
		T yz = temp[1] * temp[2];
		T xz = temp[0] * temp[2];
		T wx = temp[3] * temp[0];
		T wy = temp[3] * temp[1];
		T wz = temp[3] * temp[2];

		return {
			1 - 2 * y_2 - 2 * z_2,
			2 * (xy + wz),
			2 * (xz - wy),
			0,
			2 * (xy - wz),
			1 - 2 * x_2 - 2 * z_2,
			2 * (yz + wx),
			0,
			2 * (xz + wy),
			2 * (yz - wx),
			1 - 2 * x_2 - 2 * y_2,
			0,
			0,
			0,
			0,
			1
		};
	}

	matrix_t< T > matrix() const
	{
		T x = m_value[0];
		T y = m_value[1];
		T z = m_value[2];
		T w = m_value[3];
		return { w, -x, -y, -z, x, w, -z, y, y, z, w, -x, z, -y, x, w };
	}

	T angle(bool flag) const
	{
		T angle = 2 * std::acos(m_value[3]);
		if (!flag)
			return angle * (180.0 / M_PI);
		return angle;
	}

	T angle() const { return angle(true); }

	vector3_t< T > apply(vector3_t< T > vec) const
	{
		Quat< T > newQ(0, vec.x, vec.y, vec.z);
		Quat< T > s = (*this);
		toNormalize(&s);
		newQ = s * newQ * ~s;
		return { newQ.m_value[0], newQ.m_value[1], newQ.m_value[2] };
	}

	explicit operator T() const { return norma(*this); }

  private:
	T m_value[4];

	Quat< T > getAddOrSub(Quat< T > const *val, Quat< T > *ret, short unit) const
	{
		for (int i = 0; i < 4; i++)
			ret->m_value[i] = m_value[i] + unit * val->m_value[i];
		return *ret;
	}

	Quat< T > scalarVec(T scalar) const
	{
		return Quat< T >(m_value[3], m_value[0] * scalar, m_value[1] * scalar, m_value[2] * scalar);
	}

	Quat< T > vecMul(Quat< T > *newQ, Quat< T > *value) const
	{
		for (int i = 1; i < 4; i++)
		{
			newQ->m_value[i - 1] += m_value[i % 3] * value->m_value[(i + 1) % 3] - m_value[(i + 1) % 3] * value->m_value[i % 3];
		}
		return *newQ;
	}

	static T norma(Quat< T > q)
	{
		T norm = 0;
		for (auto &i : q.m_value)
			norm += i * i;
		return std::sqrt(norm);
	}

	static void toNormalize(Quat< T > *q)
	{
		T n1 = norma(*q);
		if (n1 == 0)
			n1 = 1;
		if (n1 != 1)
		{
			for (short i = 0; i < 4; i++)
				q->m_value[i] /= n1;
		}
	}
};
