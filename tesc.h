#pragma once

// Copyright © 2020-2021 Alex Qzminsky.
// License: MIT. All rights reserved.

/// \brief ANSI codes-based console text stylizer
///
/// \author https://github.com/qzminsky///
/// \version 2.0.1
/// \date 2020/03/17

#ifndef TESC_H
#define TESC_H

static_assert(__cplusplus >= 201700L, "C++17 or higher is required");

#include <cstdint>
#include <iostream>
#include <utility>

namespace tesc
{
    // SECTION Manipulators parameters
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
    // !SECTION

    // SECTION Parameters modifiers
    /**
     * \brief `face`-to-`back` joiner
     *
     * \param clr_1 Text foreground color
     * \param clr_2 Text background color
     *
     * \return Joint color pair
    */
    [[nodiscard]]
    auto operator | (face clr_1, back clr_2) -> std::pair<face, back>
    {
        return std::make_pair(clr_1, clr_2);
    }

    /**
     * \brief `back`-to-`face` joiner
     *
     * \param clr_1 Text background color
     * \param clr_2 Text foreground color
     *
     * \return Joint color pair
    */
    [[nodiscard]]
    auto operator | (back clr_1, face clr_2) -> std::pair<face, back>
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
    */
    [[nodiscard]]
    auto bright (face clr) -> face
    {
        if ((uint8_t)clr >= 90) return clr;

        return face{ 60 + (uint8_t)clr };
    }

    /**
     * \brief Transforms the input background color into its brighter version
     *
     * \param clr Input color
     *
     * \return Brighter color
    */
    [[nodiscard]]
    auto bright (back clr) -> back
    {
        if ((uint8_t)clr >= 100) return clr;

        return back{ 60 + (uint8_t)clr };
    }
    // !SECTION

    // SECTION Manipulators
    // ANCHOR The `color` manipulator
    /**
     * \class color
     *
     * \brief The 4-bit console text manipulator-colorizer
    */
    class color
    {
        static face _fg_color;    ///< Foreground color
        static back _bg_color;    ///< Background color

    public:

        /// There is no default constructor for a colorizer
        color () = delete;

        /**
         * \brief Converting constructor from a text color code
         *
         * \param clr ANSI color code
        */
        color (std::pair<face, back> const& clr)
        {
            _fg_color = clr.first;
            _bg_color = clr.second;
        }

        /**
         * \brief Converting constructor from a text foreground color
         *
         * \param clr Text foreground color
        */
        color (face clr)
        {
            _fg_color = clr;
        }

        /**
         * \brief Converting constructor from a text background color
         *
         * \param clr Text background color
        */
        color (back clr)
        {
            _bg_color = clr;
        }

        /**
         * \brief Getting current text foreground color
        */
        [[nodiscard]]
        static auto get_face () -> face
        {
            return _fg_color;
        }

        /**
         * \brief Getting current text background color
        */
        [[nodiscard]]
        static auto get_back () -> back
        {
            return _bg_color;
        }

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
            int clr_1 = (uint8_t)decorator.get_face(),
                clr_2 = (uint8_t)decorator.get_back();

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

    // ANCHOR The `font` manipulator
    /**
     * \class font
     *
     * \brief Console font style manipulator
    */
    class font
    {
        static style _style;

    public:

        /**
         * \brief Default constructor
        */
        font () = default;

        /**
         * \brief Converting constructor from a style
         *
         * \param st Font style
        */
        font (style st)
        {
            _style = st;
        }

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
                test_style(style::bold) ? 1 : 22,
                test_style(style::italic) ? 3 : 23,
                test_style(style::underline) ? 4 : 24,
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

        /**
         * \brief Returns current font style
        */
        [[nodiscard]]
        static auto get_style () -> style
        {
            return _style;
        }

        /**
         * \internal
         * \brief Predicate. Checks if the style contains a given option (or both are equal to 0)
         *
         * \param st Style option
        */
        [[nodiscard]]
        static auto test_style (style stl) -> bool
        {
            if (
                auto st_1 = (uint8_t)get_style(), st_2 = (uint8_t)stl;
                !st_1 && !st_2
            ) {
                return true;
            }
            else return (st_1 & st_2) == st_2;
        }
    };

    // ANCHOR The `reset` manipulator
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
    // !SECTION

    // Static fields initialization
    face color::_fg_color = face::none;
    back color::_bg_color = back::none;
    style font::_style = style::normal;

}   // end namespace tesc

#endif  // TESC_H

// MIT License
//
// Copyright (c) 2020-2021 Alex Qzminsky
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
