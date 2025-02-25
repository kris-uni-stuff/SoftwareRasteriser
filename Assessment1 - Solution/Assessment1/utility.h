#pragma once

void CounterEndAndPrint(LARGE_INTEGER StartingTime, LARGE_INTEGER* EndingTime, LARGE_INTEGER Frequency)
{
    QueryPerformanceCounter(EndingTime);

    LARGE_INTEGER ElapsedMicroseconds;
    ElapsedMicroseconds.QuadPart = (*EndingTime).QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << (float)ElapsedMicroseconds.QuadPart / (float)1000000 << std::endl;
}

void writeColToDisplayBuffer(vec3 col, int pixel_x, int pixel_y)
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

	colour_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 0] = pixel_r;
	colour_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 1] = pixel_g;
	colour_buffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 2] = pixel_b;

}
