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


#include <rld-size-of.h>
#include <rld-cc.h>

namespace rld
{
  namespace size_of
  {
    void
    compile_wrapper (rld::process::tempfile& c,
                     rld::process::tempfile& o)
    {
      rld::process::arg_container args;

      if (rld::verbose ())
      std::cout << "wrapper O file: " << o.name () << std::endl;

      rld::cc::make_cc_command (args);
      rld::cc::append_flags (rld::cc::ft_cflags, args);

      args.push_back ("-O2");
      args.push_back ("-g");
      args.push_back ("-c");
      args.push_back ("-o");
      args.push_back (o.name ());
      args.push_back (c.name ());

      rld::process::tempfile out;
      rld::process::tempfile err;
      rld::process::status   status;

      status = rld::process::execute (rld::cc::get_cc (),
                                      args,
                                      out.name (),
                                      err.name ());

      if ((status.type != rld::process::status::normal) ||
          (status.code != 0))
      {
          err.output (rld::cc::get_cc (), std::cout);
          throw rld::error ("Compiler error", "compiling wrapper");
      }
    }

    std::string
    format_string (std::string str)
    {
      /*
       * Remove whitespaces and operators from string.
       */
      str = rld::find_replace (str, " ", "_");
      str = rld::find_replace (str, "*", "_pointer");
      return str;
    }

    int
    read_stream_int32 (std::stringstream& binary_stream)
    {
      char buffer[4];
      int32_t temp = 1;
      int32_t value = 0;

      binary_stream.read (buffer, 1);

      binary_stream.read (buffer, 4);
      for (int i = 0; i <= 3; i++)
      {
        for (int j = 0; j <= 7; j++)
        {
          if (( buffer[i] & 1 << j ) != 0)
              value = value + temp;
          temp = temp * 2;
        }
      }
      binary_stream.read (buffer, 1);
      return value;
    }

    std::string
    read_stream_string (std::stringstream& binary_stream, unsigned int size)
    {
      char buffer[64] = "";
      binary_stream.read (buffer, size);
      if (!strcmp (buffer, ""))
      {
        return "NIL";
      }
      return buffer;
    }

    void
    get_sizeof (types& types_,
                const std::string& prefix_code,
                std::string headers)
    {
      std::stringstream sss;
      std::stringstream binary_stream;
      files::sections   secs;
      std::string       s1;
      char              buf;
      unsigned int      max_size = 0;

      rld::process::tempfile c (".c");
      rld::process::tempfile o (".o");
      c.override ("tempcfile");
      o.override ("tempofile");
      c.keep ();
      o.keep ();
      c.open (true);

      sss << headers << std::endl
          << prefix_code;

      for (types::iterator ti = types_.begin ();
           ti != types_.end ();
           ++ti)
      {
        max_size = (max_size > (*ti).get_name ().size ())? max_size: (*ti).get_name ().size ();
      }

      sss << "struct type_map {"
          << std::endl
          << "  char name["
          << max_size + 1
          << "];"
          << std::endl
          << "  char tag_s;"
          << std::endl
          << "  int32_t len;"
          << std::endl
          << "  char tag_e;"
          << std::endl
          << "} __attribute__((packed));"
          << std::endl
          << std::endl;

      for (types::const_iterator ti = types_.begin ();
           ti != types_.end ();
           ++ti)
      {
        type typ = *ti;
        sss << "static struct type_map __type_map_"
            << format_string (typ.get_name ())
            << " __used __attribute__((section(\"__barectf_type_map\")))"
            << " = {  .name = \"" << typ.get_name ()
            << "\", .tag_s = '[',"
            << "  .len = sizeof(" << typ.get_name () << "),"
            << " .tag_e = ']',};"
            << std::endl << std::endl;
      }

      c.write_line (sss.str ());
      compile_wrapper (c, o);

      rld::files::object obj (o.name ());

      obj.open ();
      obj.begin ();
      if (!obj.valid ())
        throw rld::error ("Not valid: " + obj.name ().full (),
                          "init::image");
      obj.get_sections (secs);

      /*
       * Find ELF data section, convert it to binary-stream, read and record
       * sizes from the stream.
       */
      for (files::sections::iterator ii = secs.begin ();
           ii != secs.end ();
           ++ii)
      {
        const files::section& fsec = *ii;
        if (fsec.name == "__barectf_type_map")
        {
          rld::elf::section sect = obj.elf ().get_section (fsec.index);
          rld::elf::elf_data* data = sect.data ();

          for (unsigned int i=0; i < data->d_size; i++)
          {
            buf = ((char*) data->d_buf)[i];
            binary_stream << buf;
          }

          while ((s1 = read_stream_string (binary_stream, max_size + 1)) != "NIL")
          {
            for (types::iterator ti = types_.begin ();
                 ti != types_.end ();
                 ++ti)
            {
              type ttype = *ti;
              if (!strcmp (s1.c_str (), ttype.get_name ().c_str ()))
              {
                type temp (s1);
                temp.set_size (read_stream_int32 (binary_stream));
                types_.erase (ti);
                types_.insert (ti, temp);
              }
            }
          }
          break;
        }
      }
    }

  }
}
