#pragma once

#ifndef TESC_H
#define TESC_H

static_assert(__cplusplus >= 201100L, "C++11 or higher is required");

#include <cstdint>
#include <exception>
#include <iostream>

/**
 * \namespace tesc
 * 
 * \brief Escape-based console text stylizer
 * \author Qzminsky
 * 
 * \version 0.1.0
 * \date 2020/03/08
*/
namespace tesc
{
    // ANCHOR Exceptions classes
    using invalid_argument = std::invalid_argument;

    /**
     * \brief Manipulator. Resets current style to the default
     * 
     * \param os Stylized output stream
     * 
     * \return Reference to the output stream
    */
    auto reset (std::ostream& os) -> std::ostream&
    {
        return os << "\033[0m";
    }

    /**
     * \enum face
     * 
     * \brief Text foreground color
    */
    enum class face : uint8_t
    {
        none = 0,
        black = 30, red, green, yellow, blue, magenta, cyan, white,
    };

    /**
     * \enum back
     * 
     * \brief Text background color
    */
    enum class back : uint8_t
    {
        none = 0,
        black = 40, red, green, yellow, blue, magenta, cyan, white,
    };

    /**
     * \enum style
     * 
     * \brief Font style
     * 
     * \note Stores non-valid ANSI codes due to easy joining
    */
    enum class style : uint8_t
    {
        normal = 0, bold = 1, italic = 2, underline = 4,
    };

    /**
     * \brief `face`-to-`back` joiner
     * 
     * \param clr_1 Text foreground color
     * \param clr_2 Text background color
     * 
     * \return Joint color code
    */
    [[nodiscard]]
    auto operator | (face clr_1, back clr_2) -> uint16_t
    {
        return (uint16_t)clr_1 << 8 | (uint8_t)clr_2;
    }

    /**
     * \brief `back`-to-`face` joiner
     * 
     * \param clr_1 Text background color
     * \param clr_2 Text foreground color
     * 
     * \return Joint color code
    */
    [[nodiscard]]
    auto operator | (back clr_1, face clr_2) -> uint16_t
    {
        return clr_2 | clr_1;
    }

    /**
     * \brief `style`-to-`style` joiner
     * 
     * \param f_1 First font style
     * \param f_2 Second font style
     * 
     * \return Joint font style
    */
    [[nodiscard]]
    auto operator | (style f_1, style f_2) -> style
    {
        return style{ (uint8_t)f_1 | (uint8_t)f_2 };
    }

    /**
     * \brief Transforms the input foreground color into its brighter version
     * 
     * \param clr Input color
     * 
     * \return Brighter color
     * 
     * \throw invalid_argument
    */
    [[nodiscard]]
    auto bright (face clr) -> face
    {
        if ((uint8_t)clr >= 90) throw invalid_argument{ "Foreground color is already brighten" };

        return face{ 60 + (uint8_t)clr };
    }

    /**
     * \brief Transforms the input background color into its brighter version
     * 
     * \param clr Input color
     * 
     * \return Brighter color
     * 
     * \throw invalid_argument
    */
    [[nodiscard]]
    auto bright (back clr) -> back
    {
        if ((uint8_t)clr >= 100) throw invalid_argument{ "Background color is already brighten" };

        return back{ 60 + (uint8_t)clr };
    }

    /**
     * \class color
     * 
     * \brief The 3/4-bit console text manipulator-colorizer
    */
    class color
    {
        /*   Color 1   Color 2
         *  ╭——˄———╮   ╭——˄———╮
         * [xxxxxxxx | xxxxxxxx]  -- representation data
         *  15     8   7      0
        */

        uint16_t _color;

    public:

        /**
         * \brief Converting constructor from a text color code
         * 
         * \param clr ANSI color code
        */
        color (uint16_t clr) : _color { clr } {}

        /**
         * \brief Converting constructor from a text foreground color
         * 
         * \param clr Text foreground color
        */
        color (face clr) : _color{ (uint8_t)clr } {}

        /**
         * \brief Converting constructor from a text background color
         * 
         * \param clr Text background color
        */
        color (back clr) : _color{ (uint8_t)clr } {}

        /**
         * \brief Applies the color settings to the output stream
         * 
         * \param os Output stream
         * \param decorator Color settings
         * 
         * \return Reference to the output stream
        */
        friend auto operator << (std::ostream& os, color const& decorator) -> std::ostream&
        {
            int clr_1 = decorator._color >> 8, clr_2 = (uint8_t)decorator._color;

            os << "\033[";

            // Zero-valued colors don't have any effect
            if (clr_1)
            {
                os << clr_1;
                if (clr_2) os << ";";
            }
            if (clr_2) os << clr_2;

            return os << "m";
        }
    };

    /**
     * \class font
     * 
     * \brief Console font style manipulator
    */
    class font
    {
        style _style = style::normal;

    public:

        /// There is no default constructor for a font-style
        font () = default;

        /**
         * \brief Converting constructor from a style
         * 
         * \param st Font style
        */
        font (style st) : _style{ st } {}

        /**
         * \brief Applies the font style to the output stream
         * 
         * \param os Output stream
         * \param decorator Font style manipulator
         * 
         * \return Reference to the output stream
        */
        friend auto operator << (std::ostream& os, font const& styler) -> std::ostream&
        {
            int modif[] = {
                styler._test(style::bold) ? 1 : 22,
                styler._test(style::italic) ? 3 : 23,
                styler._test(style::underline) ? 4 : 24,
            };

            // Sorting an array due to cancellers which must be the last
            if (modif[0] > modif[1]) std::swap(modif[0], modif[1]);
            if (modif[1] > modif[2])
            {
                std::swap(modif[1], modif[2]);
                if (modif[0] > modif[1]) std::swap(modif[0], modif[1]);
            }

            return os << "\033[" << modif[0] << ";" << modif[1] << ";" << modif[2] << "m";
        }

    private:

        /**
         * \internal
         * \brief Predicate. Checks if the style contains a given option
         * 
         * \param st Style option
        */
        [[nodiscard]]
        auto _test (style st) const -> bool
        {
            return (uint8_t)_style & (uint8_t)st;
        }
    };

}

#endif
