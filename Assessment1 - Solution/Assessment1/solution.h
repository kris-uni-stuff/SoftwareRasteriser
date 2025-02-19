#pragma once
#include "bmp.h"
#include "display.h"
#include "maths.h"
#include "vertices.h"
#include "obj.h"

#include <algorithm>

//glm::vec3 *colour_buffer;
vec3 clear_col;

/*void SetNumSamples(int n)
{
	colour_buffer = (glm::vec3*)malloc(sizeof(glm::vec3) * (PIXEL_W * n) * (PIXEL_H * n));
	BUFFER_W = PIXEL_W * n;
	BUFFER_H = PIXEL_H * n;
}
*/

void ClearColourBuffer(float col[4])
{
	for (int y = 0; y < PIXEL_H; y++)
	{
		for (int x = 0; x < PIXEL_W; x++)
		{
			display_buffer[(y * PIXEL_W * 3) + (x * 3) + 0] = col[0] * 255.f;
			display_buffer[(y * PIXEL_W * 3) + (x * 3) + 1] = col[1] * 255.f;
			display_buffer[(y * PIXEL_W * 3) + (x * 3) + 2] = col[2] * 255.f;

			depth_buffer[(y * PIXEL_W) + x] = FLT_MAX;
		}
	}
	
	clear_col.r = col[0];
	clear_col.g = col[1];
	clear_col.b = col[2];
}

triangle* AssembleTriangles(float* verts, int num_verts, int* num_tris)
{
	(*num_tris) = num_verts / 3;
	triangle* tris = (triangle*)malloc(sizeof(triangle) * (*num_tris));

	for (int t = 0; t < (*num_tris); t++)
	{
		tris[t].v1.pos.x = verts[t * 3 * 6 + 0];
		tris[t].v1.pos.y = verts[t * 3 * 6 + 1];
		tris[t].v1.pos.z = verts[t * 3 * 6 + 2];
		tris[t].v1.col.x = verts[t * 3 * 6 + 3];
		tris[t].v1.col.y = verts[t * 3 * 6 + 4];
		tris[t].v1.col.z = verts[t * 3 * 6 + 5];

		tris[t].v2.pos.x = verts[t * 3 * 6 + 6];
		tris[t].v2.pos.y = verts[t * 3 * 6 + 7];
		tris[t].v2.pos.z = verts[t * 3 * 6 + 8];
		tris[t].v2.col.x = verts[t * 3 * 6 + 9];
		tris[t].v2.col.y = verts[t * 3 * 6 + 10];
		tris[t].v2.col.z = verts[t * 3 * 6 + 11];

		tris[t].v3.pos.x = verts[t * 3 * 6 + 12];
		tris[t].v3.pos.y = verts[t * 3 * 6 + 13];
		tris[t].v3.pos.z = verts[t * 3 * 6 + 14];
		tris[t].v3.col.x = verts[t * 3 * 6 + 15];
		tris[t].v3.col.y = verts[t * 3 * 6 + 16];
		tris[t].v3.col.z = verts[t * 3 * 6 + 17];
	}

	return tris;
}


void Translate(glm::mat4 M, glm::mat4 V, glm::mat4 P, triangle* t)
{
	glm::mat4 MV = V * M;
	glm::mat4 MVP = P * MV;
	
	t->v1.pos = MVP * t->v1.pos;
	t->v1.pos /= t->v1.pos.w;

	t->v2.pos = MVP * t->v2.pos;
	t->v2.pos /= t->v2.pos.w;

	t->v3.pos = MVP * t->v3.pos;
	t->v3.pos /= t->v3.pos.w;
}

void TransformToScreenSpace(int w, int h, triangle* t)
{
	int v1sx = (t->v1.pos.x + 1) * w / 2;
	t->v1.pos.x = v1sx;
	int v1sy = (t->v1.pos.y + 1) * h / 2;
	t->v1.pos.y = v1sy;

	int v2sx = (t->v2.pos.x + 1) * w / 2;
	t->v2.pos.x = v2sx;
	int v2sy = (t->v2.pos.y + 1) * h / 2;
	t->v2.pos.y = v2sy;

	int v3sx = (t->v3.pos.x + 1) * w / 2;
	t->v3.pos.x = v3sx;
	int v3sy = (t->v3.pos.y + 1) * h / 2;
	t->v3.pos.y = v3sy;
}

void ComputeBarycentricCoordinates(int px, int py, triangle t, float& alpha, float& beta, float& gamma)
{
	float bcp = ((t.v3.pos.y - t.v2.pos.y) * px) + ((t.v2.pos.x - t.v3.pos.x) * py) + (t.v3.pos.x * t.v2.pos.y) - (t.v2.pos.x * t.v3.pos.y);
	float bca = ((t.v3.pos.y - t.v2.pos.y) * t.v1.pos.x) + ((t.v2.pos.x - t.v3.pos.x) * t.v1.pos.y) + (t.v3.pos.x * t.v2.pos.y) - (t.v2.pos.x * t.v3.pos.y);

	float acp = ((t.v3.pos.y - t.v1.pos.y) * px) + ((t.v1.pos.x - t.v3.pos.x) * py) + (t.v3.pos.x * t.v1.pos.y) - (t.v1.pos.x * t.v3.pos.y);
	float acb = ((t.v3.pos.y - t.v1.pos.y) * t.v2.pos.x) + ((t.v1.pos.x - t.v3.pos.x) * t.v2.pos.y) + (t.v3.pos.x * t.v1.pos.y) - (t.v1.pos.x * t.v3.pos.y);

	float abp = ((t.v2.pos.y - t.v1.pos.y) * px) + ((t.v1.pos.x - t.v2.pos.x) * py) + (t.v2.pos.x * t.v1.pos.y) - (t.v1.pos.x * t.v2.pos.y);
	float abc = ((t.v2.pos.y - t.v1.pos.y) * t.v3.pos.x) + ((t.v1.pos.x - t.v2.pos.x) * t.v3.pos.y) + (t.v2.pos.x * t.v1.pos.y) - (t.v1.pos.x * t.v2.pos.y);

	alpha = bcp / bca;
	beta = acp / acb;
	gamma = abp / abc;
}

void ShadeFragment(triangle tri, float& alpha, float& beta, float& gamma, vec3& col, float &depth)
{
	tri.v1.col = vec3(tri.primID / 32.f, tri.primID / 32.f, tri.primID / 32.f);
	tri.v2.col = vec3(tri.primID / 32.f, tri.primID / 32.f, tri.primID / 32.f);
	tri.v3.col = vec3(tri.primID / 32.f, tri.primID / 32.f, tri.primID / 32.f);

	if (tri.primID == 6 || tri.primID == 7)
	{
		tri.v1.col = vec3(1, 0, 0);
		tri.v2.col = vec3(1, 0, 0);
		tri.v3.col = vec3(1, 0, 0);
	}
	if (tri.primID == 8 || tri.primID == 9)
	{
		tri.v1.col = vec3(0, 1, 0);
		tri.v2.col = vec3(0, 1, 0);
		tri.v3.col = vec3(0, 1, 0);
	}
	if (tri.primID == 10 || tri.primID == 11)
	{
		tri.v1.col = vec3(0, 0, 1);
		tri.v2.col = vec3(0, 0, 1);
		tri.v3.col = vec3(0, 0, 1);
	}



	col.r = (tri.v1.col.x * alpha) + (tri.v2.col.x * beta) + (tri.v3.col.x * gamma);
	col.g = (tri.v1.col.y * alpha) + (tri.v2.col.y * beta) + (tri.v3.col.y * gamma);
	col.b = (tri.v1.col.z * alpha) + (tri.v2.col.z * beta) + (tri.v3.col.z * gamma);

	depth = (tri.v1.pos.z * alpha) + (tri.v2.pos.z * beta) + (tri.v3.pos.z * gamma);
}

void writeCol(vec3 col, int pixel_x, int pixel_y)
{
	float r = linear_to_gamma(col.x);
	float g = linear_to_gamma(col.y);
	float b = linear_to_gamma(col.z);


	float rc = std::clamp(r, .0f, 1.f);
	float gc = std::clamp(g, .0f, 1.f);
	float bc = std::clamp(b, .0f, 1.f);

	float pixel_r = rc * 255.f;
	float pixel_g = gc * 255.f;
	float pixel_b = bc * 255.f;

	display_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 0] = pixel_r;
	display_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 1] = pixel_g;
	display_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 2] = pixel_b;

}

void Rasterise(vector<triangle> tris)
{
	for (int py = 0; py < PIXEL_H; py++)
	{
		float percf = (float)py / (float)PIXEL_H;
		int perci = percf * 100;
		std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

		for (int px = 0; px < PIXEL_W; px++)
		{

			for (int t = 0; t < tris.size(); t++)
			{
				float alpha, beta, gamma;
				ComputeBarycentricCoordinates(px, py, tris[t], alpha, beta, gamma);

				if (
					0.f <= alpha && alpha <= 1.f &&
					0.f <= beta && beta <= 1.f &&
					0.f <= gamma && gamma <= 1.f
					)
				{
					vec3 col;
					float depth;
					ShadeFragment(tris[t], alpha, beta, gamma, col, depth);
					if (depth < depth_buffer[(py * PIXEL_W) + px])
					{
						writeCol(col, px, PIXEL_H - py);
						depth_buffer[(py * PIXEL_W) + px] = depth;
					}
				}
			}
		}
	}
	std::clog << "\rFinish rendering.           \n";
}

void AppendTriangles(std::vector<triangle>* io, vector<Object> in_objs)
{
	for (auto& obj : in_objs)
	{
		io->insert(io->end(), obj.tris.begin(), obj.tris.end());
	}
}

void Draw(int num_verts)
{
	/*
	int d = BUFFER_H / PIXEL_H;
	for (int pixel_y = 0; pixel_y < PIXEL_H; ++pixel_y)
	{
		for (int pixel_x = 0; pixel_x < PIXEL_W; ++pixel_x)
		{
			float pixel_r = 0;
			float pixel_g = 0;
			float pixel_b = 0;
			for (int offset_y = 0; offset_y < d; ++offset_y)
			{
				for (int offset_x = 0; offset_x < d; ++offset_x)
				{
					pixel_r += colour_buffer[(((pixel_y * d) + offset_y) * BUFFER_W) + ((pixel_x * d) + offset_x)].x;
					pixel_g += colour_buffer[(((pixel_y * d) + offset_y) * BUFFER_W) + ((pixel_x * d) + offset_x)].y;
					pixel_b += colour_buffer[(((pixel_y * d) + offset_y) * BUFFER_W) + ((pixel_x * d) + offset_x)].z;
				}
			}
			int d2 = d * d;
			pixel_r /= d2;
			pixel_g /= d2;
			pixel_b /= d2;
			display_buffer[pixel_x][pixel_y].x = pixel_r;
			display_buffer[pixel_x][pixel_y].y = pixel_g;
			display_buffer[pixel_x][pixel_y].z = pixel_b;
		}
	}
	*/
}
