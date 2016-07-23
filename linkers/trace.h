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
 * @ingroup rtems_rld
 *
 * @brief Define different tracers.
 *
 */

namespace rld
{
  namespace trace
  {
    /**
     * Dump on error user option.
     */
    bool dump_on_error;

    /**
     * A container of arguments.
     */
    typedef std::vector < std::string > function_args;

    /**
     * A container of user-type members.
     */
    typedef std::vector < std::string > type_members;

    /**
     * The return value.
     */
    typedef std::string function_return;

    /**
     * An option is a name and value pair. We consider options as global.
     */
    struct option
    {
      std::string name;      /**< The name of this option. */
      std::string value;     /**< The option's value.. */

      /**
       * Load the option.
       */
      option (const std::string& name, const std::string& value)
        : name (name),
          value (value) {
      }
    };

    /**
     * A container of options.
     */
    typedef std::vector < option > options;

    /**
     * A function's signature.
     */
    struct signature
    {
      std::string     name; /**< The function's name. */
      function_args   args; /**< The function's list of arguments. */
      function_return ret;  /**< The fuctions return value. */

      /**
       * The default constructor.
       */
      signature ();

      /**
       * Construct the signature loading it from the configuration.
       */
      signature (const rld::config::record& record);

      /**
       * Has the signature got a return value ?
       */
      bool has_ret () const;

      /**
       * Has the signature got any arguments ?
       */
      bool has_args () const;

      /**
       * Return the function's declaration.
       */
      const std::string decl (const std::string& prefix = "") const;
    };

    /**
     * A container of signatures.
     */
    typedef std::map < std::string, signature > signatures;

    /**
     * A user type.
     */
    struct type
    {
      std::string     name; /**< The type's name. */
      type_members    mems; /**< The type's list of members. */
      size_t          size; /**< The type's size. */

      /**
       * The default constructor.
       */
      type ();

      /**
       * Construct the type loading it from the configuration.
       */
      type (const rld::config::record& record);

      /**
       * Return the type's declaration.
       */
      const std::string decl () const;
    };

    /**
     * A container of types.
     */
    typedef std::vector < type > types;

    /**
     * A function is list of function signatures headers and defines that allow
     * a function to be wrapped.
     */
    struct function
    {
      std::string  name;        /**< The name of this wrapper. */
      rld::strings headers;     /**< Include statements. */
      rld::strings defines;     /**< Define statements. */
      signatures   signatures_; /**< Signatures in this function. */

      /**
       * Load the function.
       */
      function (rld::config::config& config,
                const std::string&   name);

      /**
       * Dump the function.
       */
      void dump (std::ostream& out) const;
    };

    /**
     * A container of functions.
     */
    typedef std::vector < function > functions;

    /**
     * A generator and that contains the functions used to trace arguments and
     * return values. It also provides the implementation of those functions.
     */
    struct generator
    {
      std::string  name;            /**< The name of this wrapper. */
      std::string  lock_model;      /**< The lock model if provided, default "alloc". */
      std::string  lock_local;      /**< Code template to declare a local lock variable. */
      std::string  lock_acquire;    /**< The lock acquire if provided. */
      std::string  lock_release;    /**< The lock release if provided. */
      std::string  buffer_local;    /**< Code template to declare a local buffer variable. */
      rld::strings headers;         /**< Include statements. */
      rld::strings defines;         /**< Define statements. */
      std::string  entry_trace;     /**< Code template to trace the function entry. */
      std::string  entry_alloc;     /**< Code template to perform a buffer allocation. */
      std::string  arg_trace;       /**< Code template to trace an argument. */
      std::string  exit_trace;      /**< Code template to trace the function exit. */
      std::string  exit_alloc;      /**< Code template to perform a buffer allocation. */
      std::string  ret_trace;       /**< Code template to trace the return value. */
      rld::strings code;            /**< Code block inserted before the trace code. */
      std::string  ctf_header_trace; /**< Code template to trace ctf headers. */

      /**
       * Default constructor.
       */
      generator ();

      /**
       * Load the generator.
       */
      generator (rld::config::config& config,
                 const std::string&   name);

      /**
       * Dump the generator.
       */
      void dump (std::ostream& out) const;
    };

    /**
     * Tracer.
     */
    class tracer
    {
    public:
      tracer ();

      /**
       * Load the user's configuration.
       */
      void load (rld::config::config& config,
                 const std::string&   section);

      /**
       * Process any script based options.
       */
      void load_options (rld::config::config&        config,
                         const rld::config::section& section);

      /**
       * Process any script based options.
       */
      void load_types (rld::config::config&        config,
                         const rld::config::section& section);

      /**
       * The defines for the trace.
       */
      void load_defines (rld::config::config&        config,
                         const rld::config::section& section);

      /**
       * The functions for the trace.
       */
      void load_functions (rld::config::config&        config,
                           const rld::config::section& section);

      /**
       * The enables for the tracer.
       */
      void load_enables (rld::config::config&        config,
                         const rld::config::section& section);

      /**
       * The triggers for the tracer.
       */
      void load_triggers (rld::config::config&        config,
                          const rld::config::section& section);

      /**
       * The traces for the tracer.
       */
      void load_traces (rld::config::config&        config,
                        const rld::config::section& section);

      /**
       * Get option from the options section.
       */
      const std::string get_option (const std::string& name) const;

      /**
       * Generate the wrapper object file.
       */
      void generate (rld::process::tempfile& c);

      /**
       * Generate the trace names as a string table.
       */
      void generate_names (rld::process::tempfile& c);

      /**
       * Generate the trace signature tables.
       */
      void generate_signatures (rld::process::tempfile& c);

      /**
       * Generate the enabled trace bitmap.
       */
      void generate_enables (rld::process::tempfile& c);

      /**
       * Generate the triggered trace bitmap.
       */
      void generate_triggers (rld::process::tempfile& c);

      /**
       * Generate the functions.
       */
      void generate_functions (rld::process::tempfile& c);

      /**
       * Generate the trace functions.
       */
      virtual void generate_traces (rld::process::tempfile& c);

      /**
       * Generate a bitmap.
       */
      void generate_bitmap (rld::process::tempfile& c,
                            const rld::strings&     names,
                            const std::string&      label,
                            const bool              global_set);

      /**
       * Function macro replace.
       */
      void macro_func_replace (std::string&       text,
                               const signature&   sig,
                               const std::string& index);

      /**
       * Find the function given a name.
       */
      const function& find_function (const std::string& name) const;

      /**
       * Get the traces.
       */
      const rld::strings& get_traces () const;

      /**
       * Dump the wrapper.
       */
      void dump (std::ostream& out) const;

      /**
       * Generate sizes of types defined in configuration file.
       */
      void generate_sizeof ();

      /**
       * Generate ordered type declarations.
       */
      std::string generate_ordered_type_decls ();

      /**
       * Generate headers.
       */
      std::string get_headers ();

      /**
       * Copy sizes to container types_.
       */
      void copy_sizeof_from (rld::size_of::types rtypes);

      /**
       * Append types_ to rtypes to find sizes.
       */
      void append_user_types_to (rld::size_of::types& rtypes);


    protected:

      std::string  name;          /**< The name of the trace. */
      rld::strings defines;       /**< Define statements. */
      rld::strings enables;       /**< The default enabled functions. */
      rld::strings triggers;      /**< The default trigger functions. */
      rld::strings traces;        /**< The functions to trace. */
      options      options_;      /**< The options. */
      functions    functions_;    /**< The functions that can be traced. */
      generator    generator_;    /**< The tracer's generator. */
      types        types_;        /**< The usertypes. */

    };

    /**
     * CTF Tracer.
     */
    class ctf_tracer: public tracer
    {
    public:
      ctf_tracer ()
      {
      }

      void
      append_ctf_metadata_block_start (std::string name,
                                       rld::process::tempfile& meta)
      {
        meta.write_line (name + " {");
      }

      void
      append_ctf_metadata_block_end (rld::process::tempfile& meta)
      {
        meta.write_line ("};\n");
      }

      void
      append_ctf_metadata_event_block_end (rld::process::tempfile& meta)
      {
        meta.write_line ("    };\n};\n");
      }

      void
      append_ctf_metadata_event_block_start (rld::process::tempfile& meta,
                                             std::string streamid, std::string evid, const char* evname)
      {
        std::stringstream sss;
        append_ctf_metadata_block_start ("event", meta);
        sss << "    name = \"" << rld::to_string (evname) << "\";" << std::endl
            << "    id = " << evid << ";" << std::endl
            << "    stream_id = " << streamid << ";" << std::endl
            << "    fields := struct {";
        meta.write_line(sss.str ());
      }

      void
      append_ctf_metadata_event_field (rld::process::tempfile& meta,
                                       std::string arg_type, const char* fieldname)
      {
        std::stringstream sss;
        if (!strcmp (fieldname, "entry-field"))
        {
          sss << "        struct {" << std::endl
              << "            uint32_t func_index;" << std::endl
              << "            uint32_t tbg_executing_id;" << std::endl
              << "            uint32_t tbg_executing_status;" << std::endl
              << "            uint32_t rtld_tbg_executing_state;" << std::endl
              << "            timestamp_t now;" << std::endl
              << "        };";
        }
        else if (!strcmp (fieldname, "argument-field"))
        {
          sss << "        " << arg_type << " data;";
        }
        else if (!strcmp (fieldname, "return-field"))
        {
          sss << "        " << arg_type << " data;";
        }
        else if (!strcmp (fieldname, "exit-field"))
        {
          sss << "        struct {" << std::endl
              << "            uint32_t func_index;" << std::endl
              << "            uint32_t tbg_executing_id;" << std::endl
              << "            uint32_t tbg_executing_status;" << std::endl
              << "            uint32_t rtld_tbg_executing_state;" << std::endl
              << "            timestamp_t now;" << std::endl
              << "        };";
        }
        meta.write_line (sss.str ());
      }

      void
      append_ctf_metadata_trace_block (rld::process::tempfile& meta)
      {
        std::stringstream sss;
        append_ctf_metadata_block_start ("trace", meta);
        sss << "    major = 0;" << std::endl
            << "    minor = 1;" << std::endl
            << "    uuid = @UUID@;" << std::endl
            << "    byte_order = @BO@;" << std::endl
            << "    packet.header := struct {" << std::endl
            << "        uint32_t magic;" << std::endl
            << "        uint8_t  uuid[16];" << std::endl
            << "        uint32_t stream_id;" << std::endl
            << "    };";
        meta.write_line (sss.str ());
        append_ctf_metadata_block_end (meta);
      }

      void
      append_ctf_metadata_clock_block (rld::process::tempfile& meta)
      {
        std::stringstream sss;
        append_ctf_metadata_block_start ("clock", meta);
        sss << "    name = monotonic;" << std::endl
            << "    uuid = @CLOCKUUID@;" << std::endl
            << "    description = \"monotonic clock\";" << std::endl
            << "    freq = 1000000000;" << std::endl
            << "    offset_s = @CLOCKOFF_S@;" << std::endl
            << "    offset = @CLOCKOFF@;" << std::endl
            << "    precision = @CLOCKPREC@;" << std::endl
            << "    absolute = @CLOCKABS@;";
        meta.write_line (sss.str ());
        append_ctf_metadata_block_end (meta);
      }

      void
      append_ctf_metadata_start (rld::process::tempfile& meta)
      {
        std::stringstream sss;
        sss << "/* CTF 1.8 */"
            << "typealias integer { size = 8; align = 8; signed = false; } := uint8_t;" << std::endl
            << "typealias integer { size = 16; align = 8; signed = false; } := uint16_t;" << std::endl
            << "typealias integer { size = 32; align = 8; signed = false; } := uint32_t;" << std::endl
            << "typealias integer { size = 64; align = 8; signed = false; } := uint64_t;" << std::endl
            << "typealias integer { size = 64; align = 8; signed = false; map = clock.monotonic.value; } := timestamp_t;" << std::endl;
        meta.write_line (sss.str ());
        append_ctf_metadata_trace_block (meta);
        append_ctf_metadata_clock_block (meta);
      }

      void
      append_ctf_metadata_stream_block (rld::process::tempfile& meta,
                                        std::string streamid)
      {
        std::stringstream sss;
        append_ctf_metadata_block_start ("stream", meta);
        sss << "    id = " << streamid << ";" << std::endl
            << "    event.header := struct {" << std::endl
            << "        uint1_tid;" << std::endl
            << "        timestamp_t timestamp;" << std::endl
            << "    };";
        meta.write_line (sss.str ());
        append_ctf_metadata_block_end (meta);
      }

      void append_ctf_metadata_types (rld::process::tempfile& meta)
      {
        std::stringstream sss;
        std::string       l;
        rld::strings      nam;
        for (types::iterator i = types_.begin ();
             i != types_.end ();
             ++i)
        {
          if ((*i).mems.size () > 1)
          {
            if ((*i).name.find ("enumeration") != std::string::npos)//enum
            {
              rld::split (nam, (*i).name, ' ');
              std::string l = rld::find_replace ((*i).decl (), " " + *(nam.begin () + 1) + "_e", "");
              l = rld::find_replace (l, "{", " : int32_t {");
              sss << l << std::endl;
            }
            else//struct
            {
              rld::split (nam, (*i).name, ' ');
              std::string l = rld::find_replace ((*i).decl (), "typedef struct ", "struct ");
              l = rld::find_replace (l, *(nam.begin ()) + "_s", *(nam.begin ()));
              l = rld::find_replace (l, "} " + *(nam.begin()) + ";\n", "};\n");
              sss << l << std::endl;
            }
          }
          else if ((*i).name.find ("array") != std::string::npos)//array
          {
            sss << "typedef " << (*i).decl () << std::endl;
          }  
          else//int
          {
            sss << "typealias integer { size = " << (*i).size << "; align = 8; signed = false; } := " << (*i).name << ";" << std::endl << std::endl;
          }
        }
        meta.write_line (sss.str ());
      }


      void
      generate_traces (rld::process::tempfile& c)
      {
        rld::process::tempfile meta (".txt");
        meta.override ("metadata");
        meta.keep ();
        meta.open (true);
        c.write_line ("/*");
        c.write_line (" * Wrappers.");
        c.write_line (" */");

        append_ctf_metadata_start(meta);

        append_ctf_metadata_types(meta);

        size_t count = 0;
        for (rld::strings::const_iterator ti = traces.begin ();
             ti != traces.end ();
             ++ti)
        {
          const std::string& trace = *ti;
          bool               found = false;

          for (functions::const_iterator fi = functions_.begin ();
               fi != functions_.end ();
               ++fi)
          {
            const function&            funcs = *fi;
            signatures::const_iterator si = funcs.signatures_.find (trace);

            if (si != funcs.signatures_.end ())
            {
              found = true;

              const signature& sig = (*si).second;

              std::stringstream lss;
              lss << count;

              c.write_line("");

              std::string ds;
              std::string des;
              std::string drs;
              bool        ds_added = false;
              bool        des_added = false;
              bool        drs_added = false;
              ds  = "#define FUNC_DATA_SIZE_" + sig.name + " (";
              des = "#define FUNC_DATA_ENTRY_SIZE_" + sig.name + " (";
              drs = "#define FUNC_DATA_RET_SIZE_" + sig.name + " (";

              if (sig.has_args ())
              {
                for (size_t a = 0; a < sig.args.size (); ++a)
                {
                  if (ds_added)
                    ds += " + ";
                  else
                    ds_added = true;
                  if (des_added)
                    des += " + ";
                  else
                    des_added = true;
                  ds += "sizeof(" + sig.args[a] + ')';
                  des += "sizeof(" + sig.args[a] + ')';
                }
              }
              if (sig.has_ret () && !generator_.ret_trace.empty ())
              {
                if (ds_added)
                  ds += " + ";
                else
                  ds_added = true;
                if (drs_added)
                  drs += " + ";
                else
                  drs_added = true;
                ds += "sizeof(" + sig.ret + ')';
                drs += "sizeof(" + sig.ret + ')';
              }
              if (!ds_added)
                ds += '0';
              ds += ')';
              if (!des_added)
                des += '0';
              des += ')';
              if (!drs_added)
                drs += '0';
              drs += ')';
              c.write_line(ds);
              c.write_line(des);
              c.write_line(drs);

              c.write_line(sig.decl () + ";");
              c.write_line(sig.decl ("__real_") + ";");
              c.write_line(sig.decl ("__wrap_"));
              c.write_line("{");

              if (!generator_.lock_local.empty ())
                c.write_line(generator_.lock_local);

              if (!generator_.buffer_local.empty ())
                c.write_line(generator_.buffer_local);

              if (sig.has_ret ())
                c.write_line(" " + sig.ret + " ret;");

              std::string l;

              if (!generator_.lock_acquire.empty ())
                c.write_line(generator_.lock_acquire);

              if (!generator_.entry_alloc.empty ())
              {
                l = rld::find_replace (generator_.entry_alloc, ");", " + 4 * sizeof(uint32_t));");
                l = " " + l;
                macro_func_replace (l, sig, lss.str ());
                c.write_line(l);

                append_ctf_metadata_stream_block(meta, lss.str ());
              }

              if (!generator_.lock_release.empty () &&
                  (generator_.lock_model.empty () || (generator_.lock_model == "alloc")))
                c.write_line(generator_.lock_release);

              if (!generator_.ctf_header_trace.empty ())
              {
                l = " " + generator_.ctf_header_trace;
                macro_func_replace (l, sig, lss.str ());
                c.write_line(l);
              }

              if (!generator_.entry_trace.empty ())
              {
                l = rld::find_replace (generator_.entry_trace, ");", " + 4 * sizeof(uint32_t));"); // ?
                l = " " + l;
                macro_func_replace (l, sig, lss.str ());
                c.write_line(l);

                append_ctf_metadata_event_block_start(meta, lss.str(), "0", "event");
                append_ctf_metadata_event_field(meta, "ENTRYEVENT", "entry-field");
              }

              if (sig.has_args ())
              {
                for (size_t a = 0; a < sig.args.size (); ++a)
                {
                  std::string n = rld::to_string ((int) (a + 1));
                  l = " " + generator_.arg_trace;
                  l = rld::find_replace (l, "@ARG_NUM@", n);
                  l = rld::find_replace (l, "@ARG_TYPE@", '"' + sig.args[a] + '"');
                  l = rld::find_replace (l, "@ARG_SIZE@", "sizeof(" + sig.args[a] + ')');
                  l = rld::find_replace (l, "@ARG_LABEL@", "a" + n);
                  c.write_line(l);

                  append_ctf_metadata_event_field(meta, sig.args[a], "argument-field");
                }
              }
              append_ctf_metadata_event_block_end(meta);

              if (!generator_.lock_release.empty () && generator_.lock_model == "trace")
                c.write_line(generator_.lock_release);

              l.clear ();

              if (sig.has_ret ())
                l = " ret =";

              l += " __real_" + sig.name + '(';
              if (sig.has_args ())
              {
                for (size_t a = 0; a < sig.args.size (); ++a)
                {
                  if (a)
                    l += ", ";
                  l += "a" + rld::to_string ((int) (a + 1));
                }
              }
              l += ");";
              c.write_line(l);

              if (!generator_.lock_acquire.empty ())
                c.write_line(generator_.lock_acquire);

              if (!generator_.exit_alloc.empty ())
              {
                l = " " + generator_.exit_alloc;
                macro_func_replace (l, sig, lss.str ());
                c.write_line(l);
              }

              if (!generator_.lock_release.empty () &&
                  (generator_.lock_model.empty () || (generator_.lock_model == "alloc")))
                c.write_line(generator_.lock_release);

              if (!generator_.exit_trace.empty ())
              {
                l = " " + generator_.exit_trace;
                macro_func_replace (l, sig, lss.str ());
                c.write_line(l);

                append_ctf_metadata_event_block_start(meta, lss.str(), "1", "event");
                append_ctf_metadata_event_field(meta, "EXITEVENT", "exit-field");
              }

              if (sig.has_ret () && !generator_.ret_trace.empty ())
              {
                std::string l = " " + generator_.ret_trace;
                l = rld::find_replace (l, "@RET_TYPE@", '"' + sig.ret + '"');
                l = rld::find_replace (l, "@RET_SIZE@", "sizeof(" + sig.ret + ')');
                l = rld::find_replace (l, "@RET_LABEL@", "ret");
                c.write_line(l);

              }

              if (!generator_.lock_release.empty ())
                c.write_line(generator_.lock_release);

              if (sig.has_ret ())
              {
                c.write_line(" return ret;");
                append_ctf_metadata_event_field (meta, sig.ret, "argument-field");
                append_ctf_metadata_event_block_end(meta);
              }
              c.write_line("}");
            }
          }

          if (!found)
            throw rld::error ("not found", "trace function: " + trace);

          ++count;
        }
      }

    };

  }
}
