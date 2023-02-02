#pragma once
#include "core.h"
#include "../base/mathapi.h"

class FrameBuff
{
public:
    int width, height;
    uint8_t* color_buffer;
    float** depth_buffer;
    int32_t pitch;
    int32_t defaultColor = 0;

    FrameBuff(int width, int height)
    {
        pitch = width * 4;
        this->width = width;
        this->height = height;
        assert(width > 0 && height > 0);
        this->color_buffer = new uint8_t[pitch * height];
        depth_buffer = new float*[height];
        for (int j = 0; j < height; j++)
        {
            depth_buffer[j] = new float[width];
        }
        this->clear_color();
        this->clear_depth(std::numeric_limits<float>::max());
    }

    ~FrameBuff()
    {
        if (depth_buffer)
        {
            for (int j = 0; j < height; j++)
            {
                if (depth_buffer[j]) delete []depth_buffer[j];
                depth_buffer[j] = nullptr;
            }
            delete []depth_buffer;
            depth_buffer = nullptr;
        }
        if (color_buffer)
        {
            delete color_buffer;
            color_buffer = nullptr;
        }
    }

    inline void set_depth(Vector2i& p, float depth)
    {
        if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
            return;
        depth_buffer[p.y][p.x] = depth;
    }

    inline float get_depth(Vector2i& p)
    {
        if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
            return 0;
        return depth_buffer[p.y][p.x];
    }

    inline float get_depth(Vector2f& p)
    {
        if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
            return 0;
        return depth_buffer[(int)p.y][(int)p.x];
    }

    inline void set_color(int x, int y, Color color)
    {
        set_color(x, y, vector_to_color(color));
    }

    inline void set_color(int x, int y, uint32_t color)
    {
        uint8_t* buffer = color_buffer + y * pitch + x * 4;
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            memcpy(buffer, &color, sizeof(uint32_t));
        }
    }

    inline void set_color(Vector2i& p, uint32_t color)
    {
        uint8_t* buffer = color_buffer + p.y * pitch + p.x * 4;
        if (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height)
        {
            memcpy(buffer, &color, sizeof(uint32_t));
        }
    }

    inline void set_color(Vector2i& p, Color color)
    {
        set_color(p, vector_to_color(color));
    }

    inline uint32_t get_color(int x, int y) const
    {
        uint32_t color = 0;
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            memcpy(&color, color_buffer + y * pitch + x * 4, sizeof(uint32_t));
        }
        return color;
    }

    inline void clear_color()
    {
        for (int j = 0; j < height; j++)
        {
            uint32_t* row = (uint32_t*)(color_buffer + j * pitch);
            for (int i = 0; i < width; i++, row++)
                memcpy(row, &defaultColor, sizeof(uint32_t));
        }
    }

    inline void clear_depth(float depth)
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
                depth_buffer[j][i] = depth;
        }
    }
};
