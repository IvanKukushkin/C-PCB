#ifndef LAYER_H
#define LAYER_H

#include "mymath.h"
#include <memory>

extern float length_2d(const point_2d &p);
extern float det_2d(const point_2d &p1, const point_2d &p2);
extern float dot_2d(const point_2d &p1, const point_2d &p2);
extern point_2d sub_2d(const point_2d &p1, const point_2d &p2);
extern point_2d perp_2d(const point_2d &p);
extern point_2d scale_2d(const point_2d &p, float s);
extern bool collide_thick_lines_2d(const point_2d &tl1_p1, const point_2d &tl1_p2,
	 					const point_2d &tl2_p1, const point_2d &tl2_p2, float r);

//layer class
class layer
{
public:
	struct dims
	{
		int m_width;
		int m_height;
	};

	struct line
	{
		bool operator==(const line &l) const
		{
			return std::tie(l.m_p1, l.m_p2, l.m_radius, l.m_gap) == std::tie(m_p1, m_p2, m_radius, m_gap);
		}
		point_2d m_p1;
		point_2d m_p2;
		float m_radius;
		float m_gap;
	};

	struct record
	{
		record(int id, const line &l)
		 	: m_id(id)
			, m_line(l)
		{
			auto pv = perp_2d(sub_2d(l.m_p2, l.m_p1));
			m_lv_norm = scale_2d(pv, 1.0f / length_2d(pv));
			m_lv_dist = dot_2d(m_lv_norm, l.m_p1);

			// m_pv_norm = perp_2d(m_lv_norm);
			// m_pv_dist1 = dot_2d(m_pv_norm, l.m_p1);
			// m_pv_dist2 = m_pv_dist1 - len;
		}
		auto hit(const line &l, float d)
		{
			auto dp1 = dot_2d(m_lv_norm, l.m_p1) - m_lv_dist;
			auto dp2 = dot_2d(m_lv_norm, l.m_p2) - m_lv_dist;
			if (dp1 > d && dp2 > d) return false;
			if (dp1 < -d && dp2 < -d) return false;

			// dp1 = dot_2d(m_pv_norm, l.m_p1);
			// dp2 = dot_2d(m_pv_norm, l.m_p2);
			// if (dp1 - m_pv_dist1 > d && dp2 - m_pv_dist1 > d) return false;
			// if (dp1 - m_pv_dist2 < -d && dp2 - m_pv_dist2 < -d) return false;

			return collide_thick_lines_2d(l.m_p1, l.m_p2, m_line.m_p1, m_line.m_p2, d);
		}
		int m_id;
		line m_line;
		point_2d m_lv_norm;
		float m_lv_dist;

		// point_2d m_pv_norm;
		// float m_pv_dist1;
		// float m_pv_dist2;
	};

	struct aabb
	{
		int m_minx;
		int m_miny;
		int m_maxx;
		int m_maxy;
	};

	typedef std::vector<std::shared_ptr<record>> bucket;
	typedef std::vector<bucket> buckets;

	layer(const dims &dims, float s);
	~layer();
	aabb get_aabb(const line &l);
	void add_line(const line &l);
	void sub_line(const line &l);
	bool hit_line(const line &l);

private:
	int m_width;
	int m_height;
	float m_scale;
	buckets m_buckets;
	int m_test;
};

//layers class
class layers
{
public:
	struct dims
	{
		int m_width;
		int m_height;
		int m_depth;
	};

	layers(const dims &dims, float s);
	~layers();
	void add_line(const point_3d &p1, const point_3d &p2, float r, float g);
	void sub_line(const point_3d &p1, const point_3d &p2, float r, float g);
	bool hit_line(const point_3d &p1, const point_3d &p2, float r, float g);

private:
	int m_depth;
	std::vector<std::shared_ptr<layer>> m_layers;
};

#endif
