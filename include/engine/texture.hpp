#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <cstdint>
#include <string>

class Shader;

class Texture {
public:
    enum class Wrap {
        Repeat,
        Mirrored_Repeat,
        Clamp_To_Border,
        Clamp_To_Edge
    };

    enum class Filter {
        None,
        Nearest,
        Linear
    };

    enum class Format {
        RGB,
        RGBA
    };

    Texture(const char* path, Format format, const std::string& name = "");
    Texture() = delete;
    ~Texture();

    Texture(const Texture&) = default;
    Texture(Texture&&) = default;
    Texture& operator=(const Texture&) = default;
    Texture& operator=(Texture&&) = default;

    void activate(const Shader& shader, uint32_t unit = 0, const char* name = nullptr);

    void setWrap(Wrap s, Wrap t) {}
    void setFilter(Filter mag, Filter min, Filter mipMapMag = Filter::None, Filter mipMapMin = Filter::None) {}

private:
    std::string _name = "";
    uint32_t _id = 0;
    Format _format = Format::RGB;
    int32_t _width = 0, _height = 0;
    int32_t _depth = 0;
    std::pair<Wrap, Wrap>_wrap{ Wrap::Repeat, Wrap::Repeat };
    std::pair<Filter, Filter>_filter{ Filter::Linear, Filter::Linear };
    std::pair<Filter, Filter>_filterMipMap{ Filter::None, Filter::None };
};

#endif