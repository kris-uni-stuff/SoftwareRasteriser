#pragma once

#define _USE_MATH_DEFINES 
#define NOMINMAX
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include "bmp.h"
#include "display.h"
#include "maths.h"
#include "vertices.h"
#include "obj.h"
#include "utility.h"

#include <../glm/gtc/matrix_transform.hpp>

//glm::vec3 *colour_buffer;
//vec3 clear_col;
//glm::vec3 light_pos(4.f, 6.f, 4.f);

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
			colour_buffer[(y * PIXEL_W * 3) + (x * 3) + 0] = col[0] * 255.f;
			colour_buffer[(y * PIXEL_W * 3) + (x * 3) + 1] = col[1] * 255.f;
			colour_buffer[(y * PIXEL_W * 3) + (x * 3) + 2] = col[2] * 255.f;

			depth_buffer[(y * PIXEL_W) + x] = FLT_MAX;
		}
	}
	
//	clear_col.r = col[0];
//	clear_col.g = col[1];
//	clear_col.b = col[2];
}

void ClearDepthBuffer()
{
	for (int y = 0; y < PIXEL_H; y++)
	{
		for (int x = 0; x < PIXEL_W; x++)
		{
			depth_buffer[(y * PIXEL_W) + x] = FLT_MAX;
		}
	}
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


void ApplyTransformationMatrix(glm::mat4 T, vector<triangle> &tris)
{

	for (int t = 0; t < tris.size(); t++)
	{
		tris[t].v1.pos = T * tris[t].v1.pos;

		tris[t].v2.pos = T * tris[t].v2.pos;

		tris[t].v3.pos = T * tris[t].v3.pos;
	}
}

void ApplyPerspectiveDivision(vector<triangle>& tris)
{

	for (int t = 0; t < tris.size(); t++)
	{
		tris[t].v1.pos /= tris[t].v1.pos.w;
		tris[t].v2.pos /= tris[t].v2.pos.w;
		tris[t].v3.pos /= tris[t].v3.pos.w;
	}
}

void TransformToScreenSpace(int w, int h, vector<triangle> &tris)
{
	for (int t = 0; t < tris.size(); t++)
	{
		int v1sx = (tris[t].v1.pos.x + 1) * w / 2;
		tris[t].v1.pos.x = v1sx;
		int v1sy = (tris[t].v1.pos.y + 1) * h / 2;
		tris[t].v1.pos.y = v1sy;

		int v2sx = (tris[t].v2.pos.x + 1) * w / 2;
		tris[t].v2.pos.x = v2sx;
		int v2sy = (tris[t].v2.pos.y + 1) * h / 2;
		tris[t].v2.pos.y = v2sy;

		int v3sx = (tris[t].v3.pos.x + 1) * w / 2;
		tris[t].v3.pos.x = v3sx;
		int v3sy = (tris[t].v3.pos.y + 1) * h / 2;
		tris[t].v3.pos.y = v3sy;
	}
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
/*
glm::vec3 Diffuse(glm::vec3 col, glm::vec3 lightDir, glm::vec3 normal)
{
	float dotNL = dot(normal, lightDir);
	dotNL = std::max(dotNL, 0.f);
	glm::vec3 ret = col * dotNL;
	return ret;
}
*/

void ShadeFragment(triangle tri, float& alpha, float& beta, float& gamma, vec3& col, float &depth)
{



	col.r = (tri.v1.col.x * alpha) + (tri.v2.col.x * beta) + (tri.v3.col.x * gamma);
	col.g = (tri.v1.col.y * alpha) + (tri.v2.col.y * beta) + (tri.v3.col.y * gamma);
	col.b = (tri.v1.col.z * alpha) + (tri.v2.col.z * beta) + (tri.v3.col.z * gamma);

//	vec3 pit = (tri.v1.pos * alpha) + (tri.v2.pos * beta) + (tri.v3.pos * gamma);
//	vec3 lDir = glm::normalize(light_pos - pit);
//	col = Diffuse(col, lDir, tri.v1.nor);

	depth = (tri.v1.pos.z * alpha) + (tri.v2.pos.z * beta) + (tri.v3.pos.z * gamma);
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
						writeColToDisplayBuffer(col, px, PIXEL_H - py);
						depth_buffer[(py * PIXEL_W) + px] = depth;
					}
				}
			}
		}
	}
	std::clog << "\rFinish rendering.           \n";
}

void render(std::string MODEL_PATH)
{
	float bgd[] = { 1.f, 1.f, 1.f, 1.f };
	ClearColourBuffer(bgd);
	ClearDepthBuffer();

	std::vector<triangle> tris;
	obj_parse(MODEL_PATH.c_str(), &tris);


	glm::mat4 M(1);
	M = glm::translate(M, vec3(0.1, -2.5, -6));

	glm::mat4 V(1);

	glm::mat4 P(1);
	P = glm::perspective(glm::radians(60.f), (float)PIXEL_W / (float)PIXEL_H, 0.1f, 10.f);

//	glm::mat4 MV = V * M;
	glm::mat4 MVP = P * V * M;

	ApplyTransformationMatrix(MVP, tris);

	ApplyPerspectiveDivision(tris);

	TransformToScreenSpace(PIXEL_W, PIXEL_H, tris);

	Rasterise(tris);

}