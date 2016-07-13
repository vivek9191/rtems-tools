/*
 * Copyright (c) 2016, Vivek Kukreja <vivekkukreja5@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/**
 * @file
 *
 * @ingroup rtems_utils
 *
 * @brief Fetch type sizes from ELF object file. We need this because sizeof()
 * does not work in the C language processor.
 *
 */

#include <rld.h>

#include <string.h>
#include <vector>

namespace rld
{
  namespace size_of
  {
    /*
     * The type we wish to find the size of.
     */
    class type
    {
    public:
      /**
       * Construct the type.
       */
      type ()
      {
      }

      /**
       * Construct the type.
       */
      type (const std::string n)
      {
        name = n;
      }

      /**
       * Get the name.
       */
      const std::string& get_name () const
      {
        return name;
      }

      /**
       * Get the size.
       */
      size_t get_size () const
      {
        return size;
      }

      /**
       * Less than operator to sort on the type name.
       */
      bool operator < (const type& rhs) const
      {
        return name < rhs.name;
      }

      /**
       * The equals operator for the type name.
       */
      bool operator == (const type& rhs) const
      {
        return name == rhs.name;
      }

      /**
       * Set the size.
       */
      void set_size (size_t s)
      {
        size = s;
      }

    private:

      std::string name;  /**< The type's name, ie decl. */
      size_t      size;  /**< The type's size, ie sizeof(decl) */

    };

    /**
     * A container of types.
     */
    typedef std::vector < type > types;

    /**
     * Get the sizeof() of the container of types.
     */
    void get_sizeof (types& types_, const std::string& prefix_code, std::string headers);

  }
}